// Copyright 2013 Duncan Smith

#include "service/service.h"

#include <iostream>

#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/attributes/current_process_name.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/move/utility.hpp>

#include <service/args.h> // NOLINT

namespace src = boost::log::sources;
namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;

namespace osoa {

using namespace boost::log::trivial;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(svc_logger, src::severity_logger_mt<boost::log::trivial::severity_level>)

Service::Service() : args_(new Args()) {}

Service::~Service() {
  auto& lg = svc_logger::get();
  BOOST_LOG_SEV(lg, info) << "service stop";
}

void Service::Initialize(int argc, const char *argv[]) {
  args().Initialize(argc, argv); 

  using namespace boost::log;

  add_common_attributes();
  

  add_file_log (
    keywords::file_name = "sample_%N.log",                                        
    keywords::rotation_size = 10 * 1024 * 1024,                                   
    keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), 
    keywords::format = "[%TimeStamp% %Process% %ThreadID%]: %Message%",
    keywords::filter = expr::attr< severity_level>("Severity") >= trivial::debug
  );

  auto log_level = (args().verbose()) ? trivial::debug : trivial::info;
  auto sink = add_console_log();
  sink->set_filter(trivial::severity >= log_level);

  core::get()->add_global_attribute("ThreadID", attrs::current_thread_id());
  core::get()->add_global_attribute("Process", attrs::current_process_name());

  auto& lg = svc_logger::get();
  BOOST_LOG_SEV(lg, trivial::info) << "Started the service.";
  BOOST_LOG_SEV(lg, trivial::debug) << "really Started the service.";
}

}  // namespace osoa
