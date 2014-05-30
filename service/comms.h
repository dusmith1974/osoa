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

enum class Uri {
  kServer,
  kService,
  kTopic
};

// Handles the creation and/or resolution of services, and provides methods to
// transmit or receive data between those services.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
class Comms final : boost::noncopyable {
#pragma GCC diagnostic pop
 public:
  typedef std::function<std::string()> OnConnectCallback;
  typedef std::pair<std::string, std::string> ServiceTopicPair;
  typedef std::vector<ServiceTopicPair> TopicVec;

  // TODO(ds) _fwd.h
  typedef std::map<long, std::string> TopicMessageMap;
  typedef std::map<std::string, TopicMessageMap> TopicMessageMaps;

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

  Error Subscribe(const std::string& subscription);

  // Calls upon the service. Returns the data is successful.
  optional<std::string> Connect(const std::string& service) const;

  // Accepts a function pointer that overrides the default OnConnect callback.
  void set_on_connect_callback(OnConnectCallback val);

  // Returns the list of publications.
  const TopicVec& publications() const;

  // Returns the list of subscriptions.
  const TopicVec& subscriptions() const;
 private:
  typedef std::pair<std::shared_ptr<tcp::socket>,
                    std::shared_ptr<tcp::resolver::iterator>> SocketPointPair;

  typedef std::map<std::string, SocketPointPair> ServiceMap;
  typedef std::vector<std::string> UriVec;


  // The default OnConnect callback handler. Usually changed by the owner
  // through a call to set_on_connect_callback.
  std::string OnConnect();

  void handle_connect(const boost::system::error_code&,
                      const ServiceMap::mapped_type& socket_pair);

  void handle_read(const boost::system::error_code&,
                   const ServiceMap::mapped_type& socket_pair);

  Error PublishTopic(const std::string& topic);

  int ResolvePortNumber(const std::string& port);

  asio::io_service& io_service();

  // Returns the map of available services.
  ServiceMap& service_map();
  const ServiceMap& service_map() const;

  // Returns the function pointer to the OnConnect callback handler.
  OnConnectCallback& on_connect_callback();

  const std::string& publisher_port() const;
  void set_publisher_port(const std::string& val);

  Comms::TopicMessageMaps& topic_message_maps();
  const Comms::TopicMessageMaps& topic_message_maps() const;

  asio::io_service io_service_;
  std::unique_ptr<tcp_server> server_;

  // The map of available services.
  ServiceMap service_map_;

  // Function pointer to the OnConnect callback handler.
  OnConnectCallback on_connect_callback_;

  std::string publisher_port_;
  asio::streambuf response_;
  //TopicVec publications_;
  TopicVec subscriptions_;

  // TODO(ds) SWMR shared_lock (for multiple producers) - boost dev
  TopicMessageMaps topic_message_maps_;
};

// TODO(ds) mv impl to cc
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
class tcp_connection
    : public boost::enable_shared_from_this<tcp_connection> {
#pragma GCC diagnostic pop
 public:
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(asio::io_service* io_service,
                        const Comms::TopicMessageMap& messages) {
    return pointer(new tcp_connection(io_service, messages));
  }

  tcp::socket& socket() { return socket_; }

  void start() {
    //msg_ = "first publish\r\n";

    if (!topic_message_map_.empty()) {
      // TODO(ds) pump will never start if no initial message is present?
      last_message_ = topic_message_map_.begin();
      asio::async_write(socket_, asio::buffer(topic_message_map_.begin()->second),
                        boost::bind(&tcp_connection::handle_write,
                                    shared_from_this(),
                                    asio::placeholders::error,
                                    asio::placeholders::bytes_transferred));
    }
  }

 private:
  explicit tcp_connection(asio::io_service* io_service,
                          const Comms::TopicMessageMap& messages)
    : socket_(*io_service),
      msg_(""),
      topic_message_map_(messages),
      last_message_{} {
  }

  void handle_write(const boost::system::error_code& /*error*/,
                    size_t /*bytes_transferred*/) {
    // TODO(ds) wait for next message when empty
    //   condition var, signal?
    if (std::next(last_message_) != topic_message_map_.end()) {
      // Write the next message.
      ++last_message_;
      asio::async_write(socket_, asio::buffer(last_message_->second),
        boost::bind(&tcp_connection::handle_write, shared_from_this(),
          asio::placeholders::error, asio::placeholders::bytes_transferred));
    }
  }

  tcp::socket socket_;
  std::string msg_;

  // TODO(ds) mv map to comms (topicvec?) >1 connection will share data
  //std::map<long, std::string> msgs_;

  // TODO(ds) maintain our own TopicMessageMapS and set any {TopicMessageMap}
  // through the ctor to handle many topics. Just use a single topic for now.
  const Comms::TopicMessageMap& topic_message_map_;
  Comms::TopicMessageMap::const_iterator last_message_;
};

// TODO(ds) one tcp_server per group of topics on the same port.
class tcp_server {
 public:
  tcp_server(asio::io_service* io_service,
             int port,
             const Comms::TopicMessageMap& messages)
      : acceptor_(*io_service, tcp::endpoint(tcp::v4(), port)),
        topic_message_map_(messages)  {
    start_accept();
  }

 private:
  void start_accept() {
    tcp_connection::pointer new_connection =
      tcp_connection::create(&acceptor_.get_io_service(), topic_message_map_);

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
  const Comms::TopicMessageMap& topic_message_map_;
};
}  // namespace osoa

#endif  // SERVICE_COMMS_H_
