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

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "boost/asio.hpp"
#include "boost/noncopyable.hpp"

#include "service/service_fwd.h"

namespace osoa {

namespace asio = boost::asio;
using boost::asio::ip::tcp;

// Handles the creation and/or resolution of services, and provides methods to
// transmit or receive data between those services.
class Comms final : boost::noncopyable {
 public:
  Comms();
  ~Comms();

  // Creates a synchronous listening port for an iterative server (one request
  // at a time, handled in its entirty). Suitable for discrete simplex services
  // eg daytime. A list of ports to open are passed in the vector ports.
  //
  // Returns 0 on success.
  Error Listen(const std::vector<std::string>& ports);

  Error ResolveServices(const std::vector<std::string>& services);

  void Connect(const std::string& service) const;

 private:
  typedef std::pair<std::shared_ptr<tcp::socket>,
                    std::shared_ptr<tcp::resolver::iterator>> SocketPointPair;

  typedef std::map<std::string, SocketPointPair> ServiceMap;

  asio::io_service& io_service();

  ServiceMap& service_map();
  const ServiceMap& service_map() const;

  asio::io_service io_service_;
  ServiceMap service_map_;
};

}  // namespace osoa

#endif  // SERVICE_COMMS_H_
