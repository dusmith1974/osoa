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

#include "test/test.h"

#include <iostream>

#include "boost/program_options.hpp"

#include "service/args.h"
#include "service/comms.h"
#include "service/logging.h"
#include "service/service.h"

namespace po = boost::program_options;

namespace osoa {

// Adds options to the command line args and initializes the base class.
Error Test::Initialize(int argc, const char *argv[]) {
  po::options_description& config = args()->config();

  auto msg_count_option =
    new po::typed_value<decltype(msg_count_)>(&msg_count_);
  msg_count_option->value_name("number");

  config.add_options()
    ("msg-count,o", msg_count_option, "number of msgs");

  return Service::Initialize(argc, argv);
}

// Starts the base class service, logs messages and connects to other services.
Error Test::Start() {
  Error code = super::Start();
  if (Error::kSuccess != code) return code;

  for (int j = 0; j < msg_count();  ++j)
    BOOST_LOG_SEV(*Logging::logger(), blt::debug)
      << "The quick brown fox jumped over the lazy dog.";

  comms()->Connect("osoa");
  comms()->Connect("osoa");
  comms()->Connect("daytime");
  comms()->Connect("daytime");

  return Error::kSuccess;
}

// No tidy up is required except to stop the base class service.
Error Test::Stop() {
  return super::Stop();
}

}  // namespace osoa

// Creates and runs the service.
int main(int argc, const char *argv[]) {
  osoa::Test service;

  osoa::Error code = service.Initialize(argc, argv);
  if (osoa::Error::kSuccess != code)
    return static_cast<int>(code);

  if (osoa::Error::kSuccess == service.Start())
    service.Stop();

  return static_cast<int>(osoa::Error::kSuccess);
}
