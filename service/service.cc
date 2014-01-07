// Copyright 2013 Duncan Smith

#include "service/service.h"

#include <iostream>

#include <boost/log/trivial.hpp>

#include <service/args.h> // NOLINT

namespace osoa {

Service::Service() : args_(new Args()) {}

Service::~Service() {
  BOOST_LOG_TRIVIAL(info) << "service stop";
}

void Service::Initialize(int argc, const char *argv[]) {
  args().Initialize(argc, argv); 

  BOOST_LOG_TRIVIAL(info) << "service start";
}

}  // namespace osoa
