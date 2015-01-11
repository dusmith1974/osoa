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

// Contains a class representing a WebSocket.

#ifndef SERVICE_COMMS_WEB_SOCKET_H_
#define SERVICE_COMMS_WEB_SOCKET_H_

#include <map>
#include <memory>
#include <string>

/*namespace boost {
namespace asio {
class deadline_timer;
}
namespace system {
class error_code;
}
}*/

#include "boost/asio.hpp"

namespace asio = boost::asio;

namespace osoa {
typedef std::map<uint64_t, std::string> MessageMap;

using boost::asio::io_service;
using boost::asio::deadline_timer;

// The WebSocket class.
class WebSocket { /*final : public Base*/
 public:
  WebSocket();
  /*virtual*/ ~WebSocket();

  void AbortHandler(const boost::system::error_code&);

  void Run();
  void PublishMessage(const std::string& msg);

  asio::io_service& io_service();
 private:
  MessageMap messages_;
  boost::asio::io_service io_service_;
};
}  // namespace osoa

#endif  // SERVICE_COMMS_WEB_SOCKET_H_
