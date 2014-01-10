// Copyright 2013 Duncan Smith

#include "service/service.h"

#include <iostream>

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

namespace osoa {

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(svc_logger, src::severity_logger_mt<boost::log::trivial::severity_level>)

Service::Service() : args_(new Args()) {}

Service::~Service() {
  BOOST_LOG_TRIVIAL(info) << "service stop";
}

void Service::Initialize(int argc, const char *argv[]) {
  args().Initialize(argc, argv); 

  using namespace boost::log;

  add_common_attributes();
  add_console_log();
  add_file_log (
    keywords::file_name = "sample_%N.log",                                        
    keywords::rotation_size = 10 * 1024 * 1024,                                   
    keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), 
    keywords::format = "[%TimeStamp%]: %Message%"
  );

  auto log_level = (args().verbose()) ? trivial::debug : trivial::info;
  core::get()->set_filter(trivial::severity >= log_level);

  auto& lg = svc_logger::get();
  BOOST_LOG_SEV(lg, trivial::info) << "Started the service.";
  BOOST_LOG_SEV(lg, trivial::debug) << "really Started the service.";
}

}  // namespace osoa
