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
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <service/args.h> // NOLINT

namespace osoa {

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

  BOOST_LOG_TRIVIAL(info) << "service start";
}

}  // namespace osoa
