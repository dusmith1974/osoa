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

namespace osoa {
typedef std::map<uint64_t, std::string> MessageMap;

using boost::asio::deadline_timer;

// The WebSocket class.
class WebSocket { /*final : public Base*/
 public:
  WebSocket(std::shared_ptr<deadline_timer> work_done);
  /*virtual*/ ~WebSocket();

  void Run();
  void PublishMessage(const std::string& msg);

 private:
  void handler(const boost::system::error_code& error, int signal_number);

  int abort_signal_;
  MessageMap messages_;
  std::shared_ptr<deadline_timer> work_done_;
};
}  // namespace osoa

#endif  // SERVICE_COMMS_WEB_SOCKET_H_
