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
#include "boost/noncopyable.hpp"
#include "boost/optional.hpp"

#include "service/service_fwd.h"

namespace osoa {

namespace asio = boost::asio;

using boost::asio::ip::tcp;
using boost::optional;

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
  Error Listen(const std::string port);

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

  asio::io_service& io_service();

  // Returns the map of available services.
  ServiceMap& service_map();
  const ServiceMap& service_map() const;

  // Returns the function pointer to the OnConnect callback handler.
  OnConnectCallback& on_connect_callback();

  asio::io_service io_service_;
  
  // The map of available services.
  ServiceMap service_map_;

  // Function pointer to the OnConnect callback handler.
  OnConnectCallback on_connect_callback_;
};

}  // namespace osoa

#endif  // SERVICE_COMMS_H_
