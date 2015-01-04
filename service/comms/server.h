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

// Contains a class representing a Server.

#ifndef SERVICE_COMMS_SERVER_H_
#define SERVICE_COMMS_SERVER_H_

#include <map>
#include <string>

#include "boost/asio.hpp"

#include "service/comms/channel.h"
#include "service/comms/tcp_session_fwd.h"

using boost::asio::ip::tcp;
using boost::system::error_code;

namespace asio = boost::asio;
namespace posix_time = boost::posix_time;

namespace osoa {
  // The Server class.
  class Server final {
  public:
    Server(const tcp::endpoint& listen_endpoint,
           asio::io_service* io_service);

    void StartAccept();
    void HandleAccept(TcpSessionPtr session, const error_code& ec);
    void PublishMessage(const std::string& msg);

  private:
    asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    Channel channel_;

    std::map<uint64_t, std::string> cache_;
  };
}  // namespace osoa
#endif  // SERVICE_COMMS_SERVER_H_
