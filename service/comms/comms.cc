// Copyright 2014 Duncan Smith
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

#include "service/comms.h"

/*#include <iostream>
#include <thread>

#include "boost/algorithm/string.hpp"
#include "boost/array.hpp"

#include "service/logging.h"
#include "service/service.h"
#include "util/utilities.h"*/

namespace osoa {

Comms::Comms()
  : server_(nullptr),
    publisher_port_("") {}

Comms::~Comms() {}

Error Comms::PublishTopics(const std::string& port) {
  set_publisher_port(port);

  server_ = std::unique_ptr<Server>(
      new Server());
  try {
    // server_.start();
  } catch (std::exception& e) { // does throw?
    std::cerr << e.what() << std::endl; // blog
  }

  return Error::kSuccess;
}

Error Comms::Subscribe(const std::string& subscription) {
    try {
      // client.start();
    } catch (const std::exception& ex) {
      BOOST_LOG_SEV(*Logging::logger(), blt::debug)
        << "Exception thrown in Comms::Subscribe <" << ex.what() << ">";
      return Error::kCouldNotSubscribeToService;
    }
  }

  return Error::kSuccess;
}

}  // namespace osoa
