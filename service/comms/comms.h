// Copyright 2014 Duncan Smith

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

// Provides socket communications (Tcp) for inter-process communication between
// services.

#ifndef SERVICE_COMMS_H_
#define SERVICE_COMMS_H_

#include <thread>

#include "boost/asio.hpp"
#include "boost/noncopyable.hpp"

/*#include <functional>
#include <map>
#include <utility>
#include <vector>

#include "boost/bind.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/optional.hpp"
#include "boost/shared_ptr.hpp" */

#include <memory>
#include <string>

#include "service/service_fwd.h"

namespace osoa {

class Server;

namespace asio = boost::asio;

/*using boost::asio::ip::tcp;
using boost::optional;*/

// Provides a TCP publisher-subscriber link between a server and clients.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
class Comms final : boost::noncopyable {
#pragma GCC diagnostic pop
 public:
  Comms();
  ~Comms();

  // Opens an async listening port for the published topics.
  // Returns kSuccess.
  Error PublishChannel(const std::string& port);

  void PublishMessage(const std::string& msg);

  Error Subscribe(const std::string& host, const std::string& port);

  void Shutdown();

  const std::string& publisher_port() const;
  void set_publisher_port(const std::string& val);

  asio::io_service& io_service();

 private:
  std::unique_ptr<Server> server_;
  std::string publisher_port_;
  asio::io_service io_service_;
  std::thread publisher_thread_;
};

}  // naespace osoa
#endif  // SERVICE_COMMS_H_