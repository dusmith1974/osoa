// Copyright 2013 Duncan Smith

#include "service/service.h"

#include <iostream>

#include "service/args.h"
#include "service/logging.h"

namespace osoa {

namespace blt = boost::log::trivial;

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
  BOOST_LOG_SEV(lg, blt::info) << "Started the service.";
  BOOST_LOG_SEV(lg, blt::debug) << "really Started the service.";

  for (int j = 0; j < 3;  ++j)
    BOOST_LOG_SEV(logging().svc_logger(), blt::info) << "The quick brown fox jumped over the lazy dog."; 

  return 0;
}

int Service::Stop() {
  auto lg = logging().svc_logger();
  BOOST_LOG_SEV(lg, blt::info) << "service stop";

  return 0;
}
}  // namespace osoa
