// Copyright 2013 Duncan Smith

#include "service/logging.h"

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/current_process_name.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/move/utility.hpp>

#include "service/args.h"

namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;
namespace fs = boost::filesystem;

namespace osoa {

using namespace boost::log::trivial;
using namespace boost::log;

Logging::Logging() : 
  svc_logger_(new src::severity_logger_mt<severity_level>()) {}

Logging::~Logging() {
}

int Logging::Initialize(Args& args) {
  add_common_attributes();
  
  register_simple_formatter_factory<severity_level, char>("Severity"); 

  fs::path full_path(args.module_path());
  fs::path leaf(full_path.filename().string());

  if (!args.log_dir().empty()) {
    full_path = args.log_dir();
    full_path = full_path / leaf;
  }
  
  if (!args.no_log_file())
    add_file_log (
      keywords::file_name = full_path.string() + "_%Y-%m-%d_%H-%M-%S.%N.log",                                        
      keywords::rotation_size = 10 * 1024 * 1024,                                   
      keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), 
      keywords::format = "[%TimeStamp%] [%Process%] [%Severity%] [%ThreadID%]: %Message%",
      keywords::filter = expr::attr<severity_level>("Severity") >= debug
    );
  
  add_console_log()->set_filter(severity >= ((args.verbose()) ? debug : info));

  core::get()->add_global_attribute("ThreadID", attrs::current_thread_id());
  core::get()->add_global_attribute("Process", attrs::current_process_name());

  return 0;
}

}  // namespace osoa
