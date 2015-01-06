// Copyright 2013 Duncan Smith
// https://github.com/dusmith1974/osoa
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "osoa_pch.h"  // NOLINT

#include "service/service.h"

#include "boost/log/core.hpp"

#include "service/args.h"
#include "service/comms/comms.h"
#include "service/logging.h"
#include "util/timing.h"

namespace osoa {
Service::Service()
  : svc_start_time_(boost::chrono::steady_clock::now()),
    svc_end_time_(boost::chrono::steady_clock::now()),
    args_(new Args()),
    comms_(std::make_shared<Comms>()) {
}

Service::~Service() {}

// Parses the command line options (and config file) and sets up the logging
// front ends and back ends.
Error Service::Initialize(int argc, const char* argv[]) {
  Error code = args()->Initialize(argc, argv);
  if (Error::kSuccess != code) return code;

  code = Logging::Instance().Initialize(args());
  if (Error::kSuccess != code) return code;

  return Error::kSuccess;
}

// Starts this service, sets the start time, resolves any services that may be
// called upon during execution and opens the main listening port to accept
// connections.
Error Service::Start() {
  BOOST_LOG_SEV(*Logging::logger(), blt::info) << "Started the service.";

  set_svc_start_time(boost::chrono::steady_clock::now());

  Error code = Error::kSuccess;
  /*if (args()->var_map().count("services"))
    code = comms()->ResolveServices(args()->services());*/

  // Either open an async listening port for subscribers to connect to
  // publications, or open a sync listening port for a simple iterative server
  if (Error::kSuccess == code && args()->var_map().count("listening-port"))
    code = comms()->PublishChannel(args()->listening_port());

  return code;
}

// Stops the service, logs the service uptime and detaches any logging threads
// or front-ends from the core. Any active threads should be joined here.
Error Service::Stop() {
  BOOST_LOG_SEV(*Logging::logger(), blt::info) << "service stop";

  set_svc_end_time(boost::chrono::steady_clock::now());
  BOOST_LOG_SEV(*Logging::logger(), blt::info) << "Service uptime: "
      << add_timestamp(std::make_pair(svc_start_time(), svc_end_time()));

  // join/stop all threads before stopping logging.

  Logging::Instance().Detach();

  return Error::kSuccess;
}

std::shared_ptr<Args> Service::args() { return args_; }
std::shared_ptr<Comms> Service::comms() { return comms_; }

const Service::Timepoint& Service::svc_start_time() const {
  return svc_start_time_;
}

void Service::set_svc_start_time(const Service::Timepoint& val) {
  svc_start_time_ = val;
}

const Service::Timepoint& Service::svc_end_time() const {
  return svc_end_time_;
}

void Service::set_svc_end_time(const Service::Timepoint& val) {
  svc_end_time_ = val;
}
}  // namespace osoa
