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

#ifndef SERVICE_COMMS_COMMS_H_
#define SERVICE_COMMS_COMMS_H_

#include <memory>
#include <string>
#include <thread>

#include "boost/asio.hpp"
#include "boost/noncopyable.hpp"

#include "service/service_fwd.h"

namespace osoa {
class Server;
class WebSocket;

namespace asio = boost::asio;

using boost::asio::deadline_timer;

// Provides a TCP publisher-subscriber link between a server and clients.
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif
class Comms final : boost::noncopyable {
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

 public:
  Comms();
  ~Comms();

  Error Initialize(const std::string& port);

  // Opens an async listening port for the published topics.
  // Returns kSuccess.
  Error PublishChannel();

  void PublishMessage(const std::string& msg);

  Error Subscribe(const std::string& host, const std::string& port);

  void Shutdown();

  const std::string& publisher_port() const;
  void set_publisher_port(const std::string& val);

 private:
  std::unique_ptr<Server> server_;
  std::unique_ptr<osoa::WebSocket> ws_;

  std::string publisher_port_;
  asio::io_service io_service_;

  std::thread socket_thread_;
  std::thread web_socket_thread_;
};
}  // namespace osoa
#endif  // SERVICE_COMMS_COMMS_H_
