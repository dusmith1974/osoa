// Copyright 2013 Duncan Smith

#include "service/service.h"

#include <iostream>

#include "boost/log/core.hpp"

#include "service/args.h"
#include "service/comms.h"
#include "service/logging.h"
#include "util/timing.h"

namespace osoa {

namespace blt = boost::log::trivial;

Service::Service() :
  args_(new Args()),
  logging_(new Logging()),
  svc_start_time_(std::chrono::steady_clock::now()),
  svc_end_time_(std::chrono::steady_clock::now()) {}

Service::~Service() {
}

int Service::Initialize(int argc, const char *argv[]) {
  int result = args()->Initialize(argc, argv);
  if (0 != result) return result;

  result = logging()->Initialize(args());
  if (0 != result) return result;

  return 0;
}

int Service::Start() {
  auto lg = logging()->svc_logger();
  BOOST_LOG_SEV(*lg, blt::info) << "Started the service.";
  BOOST_LOG_SEV(*lg, blt::debug) << "really Started the service.";

  set_svc_start_time(std::chrono::steady_clock::now());

  
  return 0;
}

int Service::Stop() {
  auto lg = logging()->svc_logger();
  BOOST_LOG_SEV(*lg, blt::info) << "service stop";

  set_svc_end_time(std::chrono::steady_clock::now());
  BOOST_LOG_SEV(*lg, blt::info) << "Service uptime: "
    << add_timestamp(std::make_pair(svc_start_time(), svc_end_time()));

  // join/stop all threads before stopping logging.

  if (nullptr != logging()->async_sink()) {
    auto core = boost::log::core::get();
    core->remove_sink(logging()->async_sink());
    logging()->async_sink()->stop();
    logging()->async_sink()->flush();
    logging()->async_sink().reset();
  }

  return 0;
}
}  // namespace osoa
