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

#include "boost/optional.hpp"
#include "boost/program_options.hpp"

#include "service/args.h"
#include "service/comms.h"
#include "service/logging.h"
#include "service/service.h"
#include "util/utilities.h"

namespace po = boost::program_options;

namespace osoa {

// msg_count is the default number of messages to write to the logfile.
Test::Test() : msg_count_(10) {}

Test::~Test() {}

// Add customizations specific to this particular service.
Error Test::Initialize(int argc, const char *argv[]) {
  po::options_description& config = args()->config();

  // Add a command line option (for the number of times to log the test msg).
  auto msg_count_option =
    new po::typed_value<decltype(msg_count_)>(&msg_count_);
  msg_count_option->value_name("number");
  config.add_options()
    ("msg-count,o", msg_count_option, "number of msgs");

  // Set the callback handler for the listening port when connections are made.
  comms()->set_on_connect_callback(std::bind(&Test::OnConnect, this));

  return Service::Initialize(argc, argv);
}

// Starts the base class service, logs messages and connects to other services.
Error Test::Start() {
  Error code = super::Start();
  if (Error::kSuccess != code) return code;

  for (size_t j = 0; j < msg_count();  ++j)
    BOOST_LOG_SEV(*Logging::logger(), blt::debug)
      << "The quick brown fox jumped over the lazy dog.";

  // TODO(ds) only log on success eg from handle_connect
  code = comms()->Subscribe("data");
  if (Error::kSuccess == code) BOOST_LOG_SEV(*Logging::logger(), blt::debug)
    << "Subscribed to data";

  auto result = comms()->Connect("osoa");
  if (result) BOOST_LOG_SEV(*Logging::logger(), blt::info)
    << *TrimLastNewline(&*result);
  result = comms()->Connect("osoa");
  if (result) BOOST_LOG_SEV(*Logging::logger(), blt::info)
    << *TrimLastNewline(&*result);
  result = comms()->Connect("daytime");
  if (result) BOOST_LOG_SEV(*Logging::logger(), blt::info)
    << *TrimLastNewline(&*result);
  result = comms()->Connect("daytime");
  if (result) BOOST_LOG_SEV(*Logging::logger(), blt::info)
    << *TrimLastNewline(&*result);

  return Error::kSuccess;
}

// No tidy up is required except to stop the base class service.
Error Test::Stop() {
  return super::Stop();
}

std::string Test::OnConnect() {
  return "FROM TEST XXX";
}

size_t Test::msg_count() { return msg_count_; }

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
