// Copyright 2013 Duncan Smith

#include "service/logging.h"

#include <iostream>
#include <sstream>
#include <string>

#include "boost/asio/ip/host_name.hpp"
#include "boost/log/attributes/current_process_name.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/sinks.hpp"
#include "boost/log/support/date_time.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/log/utility/setup/console.hpp"

#include "service/args.h"

namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;
namespace fs = boost::filesystem;
namespace sinks = boost::log::sinks;

namespace osoa {

namespace bl = boost::log;
namespace blt = boost::log::trivial;

std::string Logging::log_header_("");

Logging::Logging() :
  svc_logger_(new src::severity_logger_mt<blt::severity_level>()),
  async_sink_(nullptr) {}
Logging::~Logging() {
}

void RotateHeader(sinks::text_file_backend::stream_type& file) {  // NOLINT
  file << Logging::log_header() << std::endl;
}

int Logging::Initialize(const Args& args) {
  bl::add_common_attributes();
  bl::register_simple_formatter_factory<blt::severity_level, char>("Severity");

  fs::path full_path(args.module_path());
  fs::path leaf(full_path.filename().string());
  if (!args.log_dir().empty()) {
    full_path = args.log_dir();
    full_path = full_path / leaf;
  }

  if (!args.no_log_file()) {
    boost::shared_ptr<sinks::text_file_backend> backend =
      boost::make_shared<sinks::text_file_backend>(
        bl::keywords::file_name = full_path.string() +
          "_%Y-%m-%d_%H-%M-%S.%N.log",
        bl::keywords::rotation_size = args.rotation_size() * 1024 * 1024,
        bl::keywords::time_based_rotation =
          sinks::file::rotation_at_time_point(0, 0, 0));
    backend->auto_flush(args.auto_flush_log());

    auto expr_format = expr::stream
      << "[" << expr::format_date_time<boost::posix_time::ptime>(
        "TimeStamp", "%H:%M:%S.%f") << "]"
      << " [" << expr::attr<std::string>("Process") << "]"
      << " [" << expr::attr<blt::severity_level>("Severity") << "]"
      << " [" << expr::attr<bl::attributes::current_thread_id::value_type>(
        "ThreadID") << "] "
      << expr::attr<std::string>("Message");

    auto expr_filter =
      expr::attr<blt::severity_level>("Severity") >= blt::debug;

    if (args.async_log()) {
      async_sink_ = boost::shared_ptr<async_sink_t>(new async_sink_t(backend));
      async_sink()->set_formatter(expr_format);
      async_sink()->set_filter(expr_filter);
      async_sink()->locked_backend()->set_open_handler(&RotateHeader);
      bl::core::get()->add_sink(async_sink());
    } else {
      boost::shared_ptr<sync_sink_t> sync_frontend;
      sync_frontend = boost::shared_ptr<sync_sink_t>(new sync_sink_t(backend));
      sync_frontend->set_formatter(expr_format);
      sync_frontend->set_filter(expr_filter);
      sync_frontend->locked_backend()->set_open_handler(&RotateHeader);
      bl::core::get()->add_sink(sync_frontend);
    }
  }

  // todo - also provide async console option sink frontends std::clog
  bl::add_console_log()->set_filter(
    blt::severity >= ((args.verbose()) ? blt::debug : blt::info));

  bl::core::get()->add_global_attribute(
    "ThreadID", attrs::current_thread_id());
  bl::core::get()->add_global_attribute(
    "Process", attrs::current_process_name());

  WriteLogHeader(args);

  return 0;
}

void Logging::WriteLogHeader(const Args& args) {
  std::stringstream ss;

  char* user_name = getenv("USER");
  if (!user_name)
    user_name = getenv("USERNAME");

  std::string hostname = boost::asio::ip::host_name();

  namespace pt = boost::posix_time;
  pt::ptime now = pt::second_clock::local_time();

  ss << "program started by : "
    << ((user_name) ? user_name : "unknown") << " on "
    << hostname << " at " << now << std::endl;

  if (!args.config_file().empty())
    ss << "using config-file: " << args.config_file() << std::endl;
  else
    ss << "not using config-file" << std::endl;

  if (!args.log_dir().empty())
    ss << "using log-dir: " << args.log_dir() << std::endl;
  else
    ss << "not using log-dir" << std::endl;

  ss << "no-log-file: " << args.no_log_file() << std::endl;
  ss << "verbose: " << args.verbose() << std::endl;

  ss << "log file rotation-size: " << args.rotation_size()
    << " MiB" << std::endl;

  ss << "async-log: " << args.async_log() << std::endl;
  ss << "auto-flush-log: " << args.auto_flush_log() << std::endl;

  set_log_header(ss.str());
}
}  // namespace osoa
