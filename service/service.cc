// Copyright 2013 Duncan Smith

#include "service/service.h"

#include <iostream>

#include <service/args.h> // NOLINT
#include <service/logging.h>

namespace osoa {

using namespace boost::log::trivial;

Service::Service() : 
  args_(new Args()),
  logging_(new Logging()) {}

Service::~Service() {
}

int Service::Initialize(int argc, const char *argv[]) {
  int result = args().Initialize(argc, argv); 
  if (0 != result) return result;

  result = logging().Initialize(args());
  if (0 != result) return result;

  return 0;
}

int Service::Start() {
  auto lg = logging().svc_logger();
  BOOST_LOG_SEV(lg, info) << "Started the service.";
  BOOST_LOG_SEV(lg, debug) << "really Started the service.";

  return 0;
}

int Service::Stop() {
  auto lg = logging().svc_logger();
  BOOST_LOG_SEV(lg, info) << "service stop";

  return 0;
}
}  // namespace osoa
