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

// Implements the Server class.

#include "osoa_pch.h"

#include "service/comms/server.h"

#include <iostream>

#include "boost/bind.hpp"

#include "service/comms/tcp_session.h"

namespace osoa {

Server::Server(asio::io_service& io_service,
     const tcp::endpoint& listen_endpoint)
  : io_service_(io_service),
    acceptor_(io_service, listen_endpoint),
    channel_{},
    cache_{} {
  StartAccept();
}

/*int Server::Listen(const std::string& port) {
  try {
    using namespace std;

  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}*/

void Server::StartAccept() {
  TcpSessionPtr new_session(new TcpSession(io_service_, channel_));

  acceptor_.async_accept(new_session->socket(),
      boost::bind(&Server::HandleAccept, this, new_session, _1));
}

void Server::HandleAccept(TcpSessionPtr session, const error_code& ec) {
  if (!ec) {
    for (const auto& msg : cache_) { // TODO(ds) use container adapter
      session->Deliver(msg.second);
    }
    session->Start();
  }

  StartAccept();
}

void Server::PublishMessage(const std::string& msg) {
  cache_[cache_.size() + 1] = msg;
  channel_.Deliver(msg);
}

}  // namespcae osoa
