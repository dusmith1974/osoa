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
// services across the network.

#ifndef SERVICE_COMMS_H_
#define SERVICE_COMMS_H_

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/noncopyable.hpp"
#include "boost/optional.hpp"
#include "boost/shared_ptr.hpp"

#include "service/service_fwd.h"

namespace osoa {

namespace asio = boost::asio;

using boost::asio::ip::tcp;
using boost::optional;

class tcp_server;
class tcp_connection;

// Handles the creation and/or resolution of services, and provides methods to
// transmit or receive data between those services.
class Comms final : boost::noncopyable {
 public:
  typedef std::function<std::string()> OnConnectCallback;

  Comms();
  ~Comms();

  // Creates a synchronous listening port for an iterative server (one request
  // at a time, handled in its entirty). Suitable for discrete simplex services
  // eg daytime.
  //
  // Returns kSuccess or kCouldNotOpenListeningPort.
  Error Listen(const std::string& port);

  // Opens an async listening port for the published topics.
  // Returns kSuccess.
  Error PublishTopics(const std::string& port, 
                      const std::vector<std::string>& topics);


  // Resolves any supplied services from args and populates the service map with
  // the sockets.
  //
  // Returns:
  // kSuccess
  // kInvalidURI if the service URI was an unexpected format.
  // kCouldNotResolveService
  Error ResolveServices(const std::vector<std::string>& services);

  // Calls upon the service. Returns the data is successful.
  optional<std::string> Connect(const std::string& service) const;

  // Accepts a function pointer that overrides the default OnConnect callback.
  void set_on_connect_callback(OnConnectCallback val);

 private:
  typedef std::pair<std::shared_ptr<tcp::socket>,
                    std::shared_ptr<tcp::resolver::iterator>> SocketPointPair;

  typedef std::map<std::string, SocketPointPair> ServiceMap;

  // The default OnConnect callback handler. Usually changed by the owner
  // through a call to set_on_connect_callback.
  std::string OnConnect();

  Error PublishTopic(const std::string& topic);

  asio::io_service& io_service();
  tcp_server& server();

  // Returns the map of available services.
  ServiceMap& service_map();
  const ServiceMap& service_map() const;

  // Returns the function pointer to the OnConnect callback handler.
  OnConnectCallback& on_connect_callback();

  const std::string& publisher_port() const;
  void set_publisher_port(const std::string& val);

  asio::io_service io_service_;
  std::unique_ptr<tcp_server> server_;

  // The map of available services.
  ServiceMap service_map_;

  // Function pointer to the OnConnect callback handler.
  OnConnectCallback on_connect_callback_;

  std::string publisher_port_;
};


class tcp_connection 
    : public boost::enable_shared_from_this<tcp_connection> {
 public: 
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(asio::io_service& io_service) {
    return pointer(new tcp_connection(io_service));
  }

  tcp::socket& socket() { return socket_; }

  void start() {
    msg_ = "first publish";

  asio::async_write(socket_, asio::buffer(msg_), 
    boost::bind(&tcp_connection::handle_write, shared_from_this(),
      asio::placeholders::error, asio::placeholders::bytes_transferred));
  }
 
 private:
  tcp_connection(asio::io_service& io_service) : socket_(io_service), msg_("") {}

  void handle_write(const boost::system::error_code& /*error*/,
    size_t /*bytes_transferred*/) {
  
  sleep(1);
  asio::async_write(socket_, asio::buffer(msg_), 
    boost::bind(&tcp_connection::handle_write, shared_from_this(),
      asio::placeholders::error, asio::placeholders::bytes_transferred));
  }

  tcp::socket socket_;
  std::string msg_;
};

class tcp_server {
 public:
  tcp_server(asio::io_service& io_service) 
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), 35008)) {
    start_accept();
  }

 private:
  void start_accept() {
    tcp_connection::pointer new_connection =
      tcp_connection::create(acceptor_.get_io_service());

    acceptor_.async_accept(new_connection->socket(), 
      boost::bind(&tcp_server::handle_accept, this, new_connection,
        boost::asio::placeholders::error));
  }

  void handle_accept(tcp_connection::pointer new_connection, 
      const boost::system::error_code& error) {
    if (!error)
      new_connection->start();

    start_accept();
  }

  tcp::acceptor acceptor_;
};
}  // namespace osoa

#endif  // SERVICE_COMMS_H_
