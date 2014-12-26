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

// Implements the WebSocket class.

#include "osoa_pch.h"
#include "service/comms/web_socket.h"

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "boost/asio.hpp"

#include "Poco/Format.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Util/ServerApplication.h"

using Poco::Net::ServerSocket;
using Poco::Net::WebSocketException;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;

namespace osoa {

std::mutex m;
std::condition_variable cv;

void handler(const boost::system::error_code& error, int /*signal_number*/) {
  if (!error) {
    // process signal
  }
}

// Handle a WebSocket connection.
class WebSocketRequestHandler : public HTTPRequestHandler {
 public:
   WebSocketRequestHandler(MessageMap& map) : messages_(map) {
   }

  void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    try {
      int opcodes = 0;
      Poco::Net::WebSocket ws(request, response);

      // TODO(ds) to log
      std::cout << std::string("WebSocket connection established.") << std::endl;

      char buffer[1024];
      int n;
      int flags = 0;
      int beat_num = 0;
      size_t msg_num = 1;
      std::cv_status status = std::cv_status::no_timeout;
      do {
        n = ws.receiveFrame(buffer, sizeof(buffer), flags);
        opcodes = flags & Poco::Net::WebSocket::FRAME_OP_BITMASK;

        std::cout << Poco::format("Frame received (length=%d, flags=0x%x, msg=%d).", n, unsigned(flags), ++beat_num) << std::endl; 

        // Send any new messages to the web client..
        if (status == std::cv_status::no_timeout) {
          flags &= ~Poco::Net::WebSocket::FRAME_OP_BITMASK;
          flags |= Poco::Net::WebSocket::FRAME_OP_TEXT;
          while (messages_.size() >= msg_num) {
            ws.sendFrame(messages_[msg_num].data(), messages_[msg_num].size(), flags);
            msg_num++;
          }
        }

        // .. followed by a PING to keep things moving.
        flags &= ~Poco::Net::WebSocket::FRAME_OP_BITMASK;
        flags |= Poco::Net::WebSocket::FRAME_OP_PING;
        ws.sendFrame(buffer, 0, flags);

        // Wait for a new message to arrive or send another PING after 1s.
        {
          std::unique_lock<std::mutex> lk(m);
//#define OLAP_QUICK_RACE
#ifdef OLAP_QUICK_RACE
          status = cv.wait_for(lk, std::chrono::milliseconds(50));
#else
          status = cv.wait_for(lk, std::chrono::seconds(1));
#endif
          //status = cv.wait_for(lk, std::chrono::milliseconds(1));
        }
      } while (n > 0 || opcodes != Poco::Net::WebSocket::FRAME_OP_CLOSE);

      // TODO(DS) to log
      std::cout << "WebSocket connection closed." << std::endl;
    }
    catch (WebSocketException& exc) {
      std::cout << exc.what() << std::endl;
      switch (exc.code()) {
       case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
        response.set("Sec-WebSocket-Version", Poco::Net::WebSocket::WEBSOCKET_VERSION);
        // fallthrough
       case Poco::Net::WebSocket::WS_ERR_NO_HANDSHAKE:
       case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
       case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
        response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentLength(0);
        response.send();
        break;
      }
    }
  }

 public:
   //std::unique_ptr<Poco::Net::WebSocket> ws_;
   //int flags;
 private:
  MessageMap& messages_;
};

class RequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
  RequestHandlerFactory(MessageMap& messages) : messages_(messages) {
  }

  HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
  {
    std::string str = "Request from " 
      + request.clientAddress().toString()
      + ": "
      + request.getMethod()
      + " "
      + request.getURI()
      + " "
      + request.getVersion();

    std::cout << str << std::endl;
      
    for (HTTPServerRequest::ConstIterator it = request.begin(); it != request.end(); ++it) {
      str = (it->first + ": " + it->second);
      std::cout << str << std::endl;
    }
    
    return new WebSocketRequestHandler(messages_);
  }

 private:
  MessageMap& messages_;
};

WebSocket::WebSocket() {
}

WebSocket::~WebSocket() {
}

void WebSocket::Run() {
    unsigned short port = 9980;

    // set-up a server socket
    ServerSocket svs(port);

    // set-up a HTTPServer instance
    HTTPServer srv(new RequestHandlerFactory(messages_), svs, new HTTPServerParams);

    // start the HTTPServer
    srv.start();

    // wait for CTRL-C or kill
    boost::asio::io_service io_service;
    boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
    signals.async_wait(handler);
    io_service.run();

    // Stop the HTTPServer
    srv.stop();
}

void WebSocket::PublishMessage(const std::string& msg) {
  messages_[messages_.size() + 1] = msg;
  cv.notify_all();
}

}  // namespace osoa
