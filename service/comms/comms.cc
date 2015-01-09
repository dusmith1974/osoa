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

#include "osoa_pch.h"  // NOLINT

#include "service/comms/comms.h"

#include "service/comms/client.h"
#include "service/comms/server.h"
#include "service/comms/web_socket.h"
#include "service/logging.h"
#include "service/service.h"

namespace osoa {
Comms::Comms()
  : server_(nullptr),
    ws_(nullptr),
    publisher_port_(""),
    io_service_{},
    publisher_thread_{},
    web_socket_thread_{} {
}

Comms::~Comms() {
}

Error Comms::PublishChannel(const std::string& port) {
  using std::shared_ptr;
  using boost::asio::deadline_timer;

  namespace posix_time = boost::posix_time;

  if (port.empty()) {
    BOOST_LOG_SEV(*Logging::logger(), blt::info)
        << "Usage: server <listen_port>\n";

    return Error::kInvalidArgs;
  }

  try {
    BOOST_LOG_SEV(*Logging::logger(), blt::info)
        << "Starting websockets";

    abort_timer_ = shared_ptr<deadline_timer>(new deadline_timer(io_service_));
    abort_timer_->expires_at(posix_time::pos_infin);
    abort_timer_->async_wait(boost::bind(&Comms::Shutdown, this));

    ws_ = std::unique_ptr<osoa::WebSocket>(new osoa::WebSocket(abort_timer_));
    web_socket_thread_ = std::thread(&osoa::WebSocket::Run, ws_.get());

    tcp::endpoint listen_endpoint(tcp::v4(),
                                  static_cast<unsigned short>(  // NOLINT
                                    atoi(port.c_str())));

    server_ = std::unique_ptr<Server>(
                new Server(listen_endpoint, &io_service_));

    publisher_thread_ = std::thread([&]() { io_service_.run(); });
  } catch (std::exception& e) {
    BOOST_LOG_SEV(*Logging::logger(), blt::info)
        << e.what() << std::endl;

    return Error::kCouldNotOpenListeningPort;
  }

  return Error::kSuccess;
}

void Comms::PublishMessage(const std::string& msg) {
  if (server_)
    io_service_.post(bind(&Server::PublishMessage, server_.get(), msg));
  // TODO(ds) else ASSERT, post called before service start (null server).

  if (ws_)
    ws_->PublishMessage(msg);
}

Error Comms::Subscribe(const std::string& host, const std::string& port) {
  namespace this_thread = boost::this_thread;
  namespace posix_time = boost::posix_time;

  try {
    if (host.empty() || port.empty()) {
      BOOST_LOG_SEV(*Logging::logger(), blt::info)
          << "Usage: client <host> <port>\n";

      return Error::kInvalidArgs;
    }

    for (;;) {  // reconnect on exit
      tcp::resolver resolver(io_service_);
      Client client(&io_service_);

      client.Start(resolver.resolve(tcp::resolver::query(host, port)));
      io_service_.run();

      io_service_.reset();
      this_thread::sleep(posix_time::milliseconds(1000));
      BOOST_LOG_SEV(*Logging::logger(), blt::info)
          << "Reconnecting to server";
    }
  } catch (const std::exception& ex) {
    BOOST_LOG_SEV(*Logging::logger(), blt::debug)
        << "Exception thrown in Comms::Subscribe <" << ex.what() << ">";
    return Error::kCouldNotSubscribeToService;
  }
}

void Comms::Shutdown() {
  if (web_socket_thread_.joinable())
    web_socket_thread_.join();

  io_service_.stop();

  // We are the publisher thread, don't self join!
  //if (publisher_thread_.joinable())
  //publisher_thread_.join();
}

const std::string& Comms::publisher_port() const {
  return publisher_port_;
}

void Comms::set_publisher_port(const std::string& val) {
  publisher_port_ = val;
}

asio::io_service& Comms::io_service() {
  return io_service_;
}
}  // namespace osoa
