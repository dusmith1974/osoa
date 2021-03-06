// Copyright 2013 Duncan Smith
// https://github.com/dusmith1974/osoa
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "osoa_pch.h"  // NOLINT

#include "service/logging.h"

#include <fstream>  // NOLINT
#include <iostream>  // NOLINT
#include <sstream>  // NOLINT

#include "boost/asio/ip/host_name.hpp"
#include "boost/log/attributes/current_process_name.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/sinks.hpp"
#include "boost/log/support/date_time.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/log/utility/setup/console.hpp"

#include "service/args.h"
#include "service/service.h"

namespace attrs = boost::log::attributes;
namespace bl = boost::log;
namespace expr = boost::log::expressions;

namespace osoa {
std::string Logging::log_header_("");

// Writes the header into the rotated file.
namespace {
void RotateHeader(sinks::text_file_backend::stream_type& file) {  // NOLINT
  file << Logging::log_header() << std::endl;
}
}  // namespace

// Returns the path to the logfile using the directory specified in the
// --log-dir options (if any).
namespace {
fs::path GetFullPath(std::shared_ptr<const Args> args) {
  fs::path full_path(args->module_path());
  fs::path leaf(full_path.filename().string());
  if (!args->log_dir().empty()) {
    full_path = args->log_dir();
    full_path = full_path / leaf;
  }

  return full_path;
}
}  // namespace //  NOLINT

// Sets the sink up for a default message prefix format, filtering and adds
// the sink to the core.
namespace {
template<typename T>
void SetupSink(T sink) {
  if (nullptr == sink) return;

  using expr::attr;
  using bl::attributes::current_thread_id;
  auto expr_format = expr::stream
                     << "[" << expr::format_date_time<boost::posix_time::ptime>(
                       "TimeStamp", "%H:%M:%S.%f") << "]"
                     << " [" << attr<std::string>("Process") << "]"
                     << " [" << attr<blt::severity_level>("Severity") << "]"
                     << " [" << attr<current_thread_id::value_type>(
                       "ThreadID") << "] "
                     << expr::attr<std::string>("Message");
  sink->set_formatter(expr_format);

  auto expr_filter =
    expr::attr<blt::severity_level>("Severity") >= blt::debug;
  sink->set_filter(expr_filter);

  sink->locked_backend()->set_open_handler(&RotateHeader);
  bl::core::get()->add_sink(sink);
}
}  // namespace  // NOLINT

Logging::~Logging() {}

// Returns the only instance of the logging class.
Logging& Logging::Instance() {
  static Logging instance;
  return instance;
}

// Sets the logfile name, adds the common attributes for the message prefix,
// sets the filter level for console logging (info or none if --silent).
// Writes a header into the first logfile.
Error Logging::Initialize(std::shared_ptr<const Args> args) {
  bl::add_common_attributes();
  bl::register_simple_formatter_factory<blt::severity_level, char>(
    "Severity");

  fs::path full_path = GetFullPath(args);
  if (!args->no_log_file())
    SetupLogFile(args, full_path);

  if (!args->silent())
    bl::add_console_log()->set_filter(
      blt::severity >= ((args->verbose()) ? blt::debug : blt::info));

  bl::core::get()->add_global_attribute(
    "ThreadID", attrs::current_thread_id());
  bl::core::get()->add_global_attribute(
    "Process", attrs::current_process_name());

  WriteLogHeader(args);

  return Error::kSuccess;
}

// Detaches the sinks from the core to stop any logging. Sinks are flushed to
// ensure there contents are fully written.
void Logging::Detach() {
  if (nullptr != async_sink()) {
    auto core = boost::log::core::get();
    core->remove_sink(async_sink());
    Logging::async_sink()->stop();
    Logging::async_sink()->flush();
    Logging::async_sink().reset();
  }
}

const std::string& Logging::log_header() { return Logging::log_header_; }
SeverityLoggerPtr Logging::logger() { return logger_; }
boost::shared_ptr<AsyncSink> Logging::async_sink() { return async_sink_; }

Logging::Logging() : async_sink_(nullptr) {}

// Sets up a textfile back-end with the appropriate options for synchroncity
// and flushing.
void Logging::SetupLogFile(std::shared_ptr<const Args> args,
                           const fs::path& path) {
  TextFileBackend backend = SetupTextfileBackend(args, path);
  backend->auto_flush(args->auto_flush_log());

  if (args->async_log()) {
    async_sink_ = boost::shared_ptr<AsyncSink>(new AsyncSink(backend));
    SetupSink<boost::shared_ptr<AsyncSink>>(async_sink());
  } else {
    boost::shared_ptr<SyncSink> sync_frontend = boost::shared_ptr<SyncSink>(
          new SyncSink(backend));
    SetupSink<boost::shared_ptr<SyncSink>>(sync_frontend);
  }
}

// Writes the header to the top of logfiles. Information in the header
// includes the user account under which the service was started, service
// start time, this hostname, and the options that the service was called
// with.
void Logging::WriteLogHeader(std::shared_ptr<const Args> args) const {
  std::stringstream ss;

  char* user_name;
  user_name = getenv("USER");
  if (!user_name)
    user_name = getenv("USERNAME");

  std::string hostname = boost::asio::ip::host_name();

  namespace pt = boost::posix_time;
  pt::ptime now = pt::second_clock::local_time();

  ss << "program started by : "
     << ((user_name) ? user_name : "unknown") << " on "
     << hostname << " at " << now << std::endl;

  // Display the invocation..
  ss << args->cmdline() << std::endl << std::endl;

  // ..and the contents of the options file.
  if (!args->config_file().empty()) {
    std::string line;
    std::ifstream file;
    file.open(args->config_file());
    if (file.is_open()) {
      ss << "<" << args->config_file() << ">" << std::endl;
      while (std::getline(file, line))
        ss << "  "  << line << std::endl;
      ss << "</" << args->config_file() << ">" << std::endl << std::endl;
    }
  } else {
    ss << "not using config-file" << std::endl;
  }

  if (!args->log_dir().empty())
    ss << "using log-dir: " << args->log_dir() << std::endl;
  else
    ss << "not using log-dir" << std::endl;

  ss << "no-log-file: " << args->no_log_file() << std::endl;
  ss << "verbose: " << args->verbose() << std::endl;

  ss << "log file rotation-size: " << args->rotation_size()
     << " MiB" << std::endl;

  ss << "async-log: " << args->async_log() << std::endl;
  ss << "auto-flush-log: " << args->auto_flush_log() << std::endl;

  set_log_header(ss.str());
}

// Sets the textfile back-ens with the filename including the date and
// timestamp and rotation size.
const Logging::TextFileBackend Logging::SetupTextfileBackend(
  std::shared_ptr<const Args> args,
  const fs::path& path) const {
  return boost::make_shared<sinks::text_file_backend>(
           bl::keywords::file_name = path.string() +
                                     "_%Y-%m-%d_%H-%M-%S.%N.log",
           bl::keywords::rotation_size = args->rotation_size() * 1024 * 1024,
           bl::keywords::time_based_rotation =
             sinks::file::rotation_at_time_point(0, 0, 0));
}

void Logging::set_log_header(const std::string& val) {
  Logging::log_header_ = val;
}

SeverityLoggerPtr Logging::logger_(new SeverityLogger());
}  // namespace osoa
