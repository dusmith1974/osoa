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

#include "osoa_pch.h"

#include "service/comms/comms.h"

#include "service/comms/client.h"
#include "service/comms/server.h"
#include "service/logging.h"
#include "service/service.h"

namespace osoa {

Comms::Comms()
  : server_(nullptr),
    publisher_port_(""),
    io_service_{},
    publisher_thread_{} {
}

Comms::~Comms() {
}

// TODO(ds) trap sigint with asio and stop comms logging threads etc.

// TODO(DS) remove? unused?
Error Comms::PublishChannel(const std::string& port) {
  // TODO(ds) need property?
  //set_publisher_port(port);

  if (port.empty()) {
    std::cerr << "Usage: server <listen_port>\n";
    return Error::kInvalidArgs;
  }

  try {
    tcp::endpoint listen_endpoint(tcp::v4(), static_cast<unsigned short>(atoi(port.c_str())));
    server_ = std::unique_ptr<Server>(new Server(io_service_, listen_endpoint));

    //io_service_.post(bind(&Server::PublishMessage, server_.get(), "000"));

    publisher_thread_ = std::thread([&](){ io_service_.run(); });
    std::string abc("abc");
    for (;;) {
      io_service_.post(bind(&Server::PublishMessage, server_.get(), abc));
      boost::this_thread::sleep(boost::posix_time::seconds(1));
      break;
    }

    // TODO(ds) return t and join from caller
    //publisher_thread_.join();
    // server_.start();
  } catch (std::exception& e) { // does throw?
    std::cerr << e.what() << std::endl; // blog
  }

  return Error::kSuccess;
}

void Comms::PublishMessage(const std::string& msg) {
  if (server_)
    io_service_.post(bind(&Server::PublishMessage, server_.get(), msg));
  // TODO(ds) else ASSERT, post called before service start (null server).
}

Error Comms::Subscribe(const std::string& host, const std::string& port) {
  try {
    if (host.empty() || port.empty()) {
      std::cerr << "Usage: client <host> <port>\n";
      return Error::kInvalidArgs;
    }

    tcp::resolver resolver(io_service_);
    Client client(io_service_);

    client.Start(resolver.resolve(tcp::resolver::query(host, port)));
    io_service_.run();
  } catch (const std::exception& ex) {
    // TODO(ds) log errors to cerr also
    BOOST_LOG_SEV(*Logging::logger(), blt::debug)
      << "Exception thrown in Comms::Subscribe <" << ex.what() << ">";
    return Error::kCouldNotSubscribeToService;
  }

  return Error::kSuccess;
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