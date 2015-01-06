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

// Implements the TcpSession class.

#include "osoa_pch.h"  // NOLINT

#include <string>  // NOLINT

#include "boost/bind.hpp"

#include "service/comms/tcp_session.h"
#include "service/logging.h"

#include "service/comms/channel.h"

namespace posix_time = boost::posix_time;

namespace osoa {
TcpSession::TcpSession(asio::io_service* io_service, Channel* ch)
  : channel_(*ch),
    socket_(*io_service),
    input_buffer_{},
    input_deadline_(*io_service),
    output_queue_{},
    non_empty_output_queue_(*io_service),
    output_deadline_(*io_service) {
  input_deadline_.expires_at(posix_time::pos_infin);
  output_deadline_.expires_at(posix_time::pos_infin);
  non_empty_output_queue_.expires_at(posix_time::pos_infin);
}

void TcpSession::Start() {
  channel_.Join(shared_from_this());

  std::string remote_addr = socket_.remote_endpoint().address().to_string();
  BOOST_LOG_SEV(*Logging::logger(), blt::info)
      << "TcpSession: "
      << this
      << " Accepted new client from "
      << remote_addr;

  StartRead();
  input_deadline_.async_wait(boost::bind(&TcpSession::CheckDeadline,
                                         shared_from_this(),
                                         &input_deadline_));
  AwaitOutput();
  output_deadline_.async_wait(boost::bind(&TcpSession::CheckDeadline,
                                          shared_from_this(),
                                          &output_deadline_));
}

tcp::socket& TcpSession::socket() {
  return socket_;
}

void TcpSession::Deliver(const std::string& msg) {
  output_queue_.push_back(msg + "\n");
  non_empty_output_queue_.expires_at(posix_time::neg_infin);
}

void TcpSession::Stop(error_code ec) {
  channel_.Leave(shared_from_this());

  std::string remote_addr = socket_.remote_endpoint().address().to_string();
  BOOST_LOG_SEV(*Logging::logger(), blt::info)
      << "TcpSession Error: "
      << this << " "
      << ec.message() << " "
      << remote_addr;

  socket_.close(ec);
  input_deadline_.cancel();
  non_empty_output_queue_.cancel();
  output_deadline_.cancel();
}

bool TcpSession::Stopped() const {
  return !socket_.is_open();
}

void TcpSession::StartRead() {
  input_deadline_.expires_from_now(posix_time::seconds(30));
  asio::async_read_until(socket_, input_buffer_, '\n',
                         bind(&TcpSession::HandleRead,
                              shared_from_this(), _1));
}

void TcpSession::HandleRead(const error_code& ec) {
  if (Stopped()) return;

  if (ec) {
    Stop(ec);
  } else {
    std::string msg;
    std::istream is(&input_buffer_);
    std::getline(is, msg);

    if (!msg.empty()) {
      channel_.Deliver(msg);
    } else {
      if (output_queue_.empty()) {
        output_queue_.push_back("\n");  // Return heartbeat if idle.
        non_empty_output_queue_.expires_at(posix_time::neg_infin);
      }
    }

    StartRead();
  }
}

void TcpSession::AwaitOutput() {
  if (Stopped()) return;

  if (output_queue_.empty()) {
    non_empty_output_queue_.expires_at(posix_time::pos_infin);
    non_empty_output_queue_.async_wait(bind(&TcpSession::AwaitOutput,
                                            shared_from_this()));
  } else {
    StartWrite();
  }
}

void TcpSession::StartWrite() {
  output_deadline_.expires_from_now(posix_time::seconds(30));
  asio::async_write(socket_, asio::buffer(output_queue_.front()),
                    bind(&TcpSession::HandleWrite, shared_from_this(), _1));
}

void TcpSession::HandleWrite(const error_code& ec) {
  if (Stopped()) return;

  if (!ec) {
    output_queue_.pop_front();
    AwaitOutput();
  } else {
    Stop(ec);
  }
}

void TcpSession::CheckDeadline(deadline_timer* deadline) {
  namespace sys = boost::system;
  if (Stopped()) return;

  if (deadline->expires_at() <= deadline_timer::traits_type::now())
    Stop(error_code(sys::errc::timed_out, sys::system_category()));
  else
    deadline->async_wait(bind(&TcpSession::CheckDeadline,
                              shared_from_this(), deadline));
}
}  // namespace osoa
