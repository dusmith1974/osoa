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
    publisher_thread_{} {
  }

  Comms::~Comms() {
  }

  // TODO(ds) trap sigint with asio and stop comms logging threads etc.

  // TODO(DS) remove? unused?
  Error Comms::PublishChannel(const std::string& port) {
    if (port.empty()) {
      std::cerr << "Usage: server <listen_port>\n";
      return Error::kInvalidArgs;
    }

    try {
      // TODO(ds) ws needs to connect before tcp..
      ws_ = std::unique_ptr<osoa::WebSocket>(new osoa::WebSocket);
      BOOST_LOG_SEV(*Logging::logger(), blt::info) << "olap ws";
      web_socket_thread_ = std::thread(&osoa::WebSocket::Run, ws_.get());

      tcp::endpoint listen_endpoint(tcp::v4(),
                                    static_cast<unsigned short>(  // NOLINT
                                    atoi(port.c_str())));

      server_ = std::unique_ptr<Server>(
        new Server(listen_endpoint, &io_service_));

      publisher_thread_ = std::thread([&]() { io_service_.run(); });
    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;  // TODO(ds) to log
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
    try {
      if (host.empty() || port.empty()) {
        std::cerr << "Usage: client <host> <port>\n";
        return Error::kInvalidArgs;
      }

      for (;;) {  // reconnect on exit
        tcp::resolver resolver(io_service_);
        Client client(&io_service_);

        client.Start(resolver.resolve(tcp::resolver::query(host, port)));
        io_service_.run();

        io_service_.reset();
        BOOST_LOG_SEV(*Logging::logger(), blt::info)
          << "Reconnecting to server";
      }
    } catch (const std::exception& ex) {
      // TODO(ds) log errors to cerr also
      BOOST_LOG_SEV(*Logging::logger(), blt::debug)
        << "Exception thrown in Comms::Subscribe <" << ex.what() << ">";
      return Error::kCouldNotSubscribeToService;
    }
  }

  // TODO(ds) trap sigint etc via asio (see Trojo).
  void Comms::Shutdown() {
    if (publisher_thread_.joinable())
      publisher_thread_.join();

    // Close sockets
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
