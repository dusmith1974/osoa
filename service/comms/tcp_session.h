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

// Contains a class representing a TcpSession.

#ifndef SERVICE_COMMS_TCP_SESSION_H_
#define SERVICE_COMMS_TCP_SESSION_H_

#include <deque>
#include <string>

#include "boost/asio.hpp"
#include "boost/enable_shared_from_this.hpp"

#include "service/comms/subscriber.h"

namespace asio = boost::asio;

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;
using boost::system::error_code;

namespace osoa {
class Channel;

// The TcpSession class.
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif
class TcpSession
  : public Subscriber,
    public boost::enable_shared_from_this <TcpSession> {
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
 public:
  TcpSession(asio::io_service* io_service, Channel* ch);

  void Start();

  tcp::socket& socket();

  void Deliver(const std::string& msg);

 private:
  void Stop(error_code ec);

  bool Stopped() const;

  void StartRead();

  void HandleRead(const error_code& ec);
  void AwaitOutput();
  void StartWrite();
  void HandleWrite(const error_code& ec);
  void CheckDeadline(deadline_timer* deadline);

  Channel& channel_;
  tcp::socket socket_;
  asio::streambuf input_buffer_;
  deadline_timer input_deadline_;
  std::deque<std::string> output_queue_;
  deadline_timer non_empty_output_queue_;
  deadline_timer output_deadline_;
};
}  // namespace osoa

#endif  // SERVICE_COMMS_TCP_SESSION_H_
