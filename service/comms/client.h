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

// Contains a class representing a Client.

#ifndef SERVICE_COMMS_CLIENT_H_
#define SERVICE_COMMS_CLIENT_H_

#include "boost/asio.hpp"

using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::deadline_timer;

namespace osoa {
// Async TCP client to a pub/sub server.
class Client final {
 public:
  explicit Client(boost::asio::io_service* io_service);
  ~Client();

  int Connect(int argc, char* argv[]);
  void Start(tcp::resolver::iterator endpoint_iter);
  void Stop();

 private:
  void StartConnect(tcp::resolver::iterator endpoint_iter);

  void HandleConnect(const error_code& ec,
                     tcp::resolver::iterator endpoint_iter);
  void StartRead();
  void HandleRead(const error_code& ec);
  void StartWrite();
  void HandleWrite(const error_code& ec);
  void CheckDeadline();

  bool stopped_;
  tcp::socket socket_;
  boost::asio::streambuf input_buffer_;
  deadline_timer deadline_;
  deadline_timer heartbeat_timer_;
};
}  // namespace osoa

#endif  // SERVICE_COMMS_CLIENT_H_
