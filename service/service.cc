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

#include "service/service.h"

#include "boost/log/core.hpp"

#include "service/args.h"
#include "service/comms.h"
#include "service/logging.h"
#include "util/timing.h"

namespace osoa {

Service::Service()
    : svc_start_time_(std::chrono::steady_clock::now()),
      svc_end_time_(std::chrono::steady_clock::now()),
      args_(new Args()),
      comms_(std::make_shared<Comms>()) {
}

int Service::Initialize(int argc, const char *argv[]) {
  int result = args()->Initialize(argc, argv);
  if (0 != result) return result;

  result = Logging::Instance().Initialize(args());
  if (0 != result) return result;

  return 0;
}

int Service::Start() {
  BOOST_LOG_SEV(*Logging::logger(), blt::info) << "Started the service.";

  set_svc_start_time(std::chrono::steady_clock::now());

  int result = 0;
  if (args()->var_map().count("services"))
    result = comms()->ResolveServices(args()->services());

  if (!result && args()->var_map().count("listening-ports"))
    result = comms()->Listen(args()->listening_ports());

  return result;
}

int Service::Stop() {
  BOOST_LOG_SEV(*Logging::logger(), blt::info) << "service stop";

  set_svc_end_time(std::chrono::steady_clock::now());
  BOOST_LOG_SEV(*Logging::logger(), blt::info) << "Service uptime: "
    << add_timestamp(std::make_pair(svc_start_time(), svc_end_time()));

  // join/stop all threads before stopping logging.

  Logging::Instance().Detach();

  return 0;
}

}  // namespace osoa
