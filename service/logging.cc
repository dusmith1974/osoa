// Copyright 2013 Duncan Smith

#include "service/logging.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "boost/asio/ip/host_name.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/log/attributes/attribute.hpp"
#include "boost/log/attributes/attribute_cast.hpp"
#include "boost/log/attributes/attribute_value.hpp"
#include "boost/log/attributes/current_process_name.hpp"
#include "boost/log/attributes/current_thread_id.hpp"
#include "boost/log/core.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/expressions/formatters/date_time.hpp"
#include "boost/log/sinks.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/sources/global_logger_storage.hpp"
#include "boost/log/sources/logger.hpp"
#include "boost/log/sources/record_ostream.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/support/date_time.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/log/utility/setup/console.hpp"
#include "boost/log/utility/setup/file.hpp"
#include "boost/move/utility.hpp"
#include <boost/log/utility/setup/settings.hpp>
#include <boost/log/utility/setup/from_settings.hpp>

#include "service/args.h"

namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;
namespace fs = boost::filesystem;

namespace osoa {

namespace bl = boost::log;
namespace blt = boost::log::trivial;

std::string Logging::log_header_("");

Logging::Logging() :
  svc_logger_(new src::severity_logger_mt<blt::severity_level>())
  /*backend_(nullptr),
  frontend_(nullptr)*/ {}

Logging::~Logging() {
}

void RotateHeader(boost::log::sinks::text_file_backend::stream_type& file) {
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
      boost::make_shared< sinks::text_file_backend>(
        bl::keywords::file_name = full_path.string() + "_%Y-%m-%d_%H-%M-%S.%N.log",
        bl::keywords::rotation_size = args.rotation_size() * 1024 * 1024,
        bl::keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0));

    typedef sinks::asynchronous_sink<sinks::text_file_backend> sink_t;
    boost::shared_ptr<sink_t> sink(new sink_t(backend));
    sink->set_formatter(expr::stream 
      << "[" << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f") << "]"
      << " [" << expr::attr<std::string>("Process") << "]"
      << " [" << expr::attr<blt::severity_level>("Severity") << "]"
      << " [" << expr::attr<bl::attributes::current_thread_id::value_type>("ThreadID") << "] "
      << expr::attr<std::string>("Message")); 
    sink->set_filter(expr::attr<blt::severity_level>("Severity") >= blt::debug);
    bl::core::get()->add_sink(sink);
    
    sink->locked_backend()->set_open_handler(&RotateHeader);
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
       
  using namespace boost::posix_time;
  ptime now = second_clock::local_time();

  if (user_name)
   ss << "program started by : " << user_name << " on " 
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
