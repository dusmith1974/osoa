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

#include <iostream>

#include "boost/asio.hpp"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Format.h"

using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
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

void handler(const boost::system::error_code& error, int /*signal_number*/) {
  if (!error) {
    // process signal
  }
}

// Handle a WebSocket connection.
class WebSocketRequestHandler : public HTTPRequestHandler {
 public:
  void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    try {
      Poco::Net::WebSocket ws(request, response);
      std::cout << std::string("WebSocket connection established.") << std::endl;
      char buffer[1024];
      int flags;
      int n;
      do {
        n = ws.receiveFrame(buffer, sizeof(buffer), flags);
        std::cout << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags)) << std::endl; // use boost format?
        ws.sendFrame(buffer, n, flags);
      } while (n > 0 || (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);

      std::cout << "WebSocket connection closed." << std::endl;
    }
    catch (WebSocketException& exc) {
      std::cout << exc.what() << std::endl;
      switch (exc.code()) {
       case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
        response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
        // fallthrough
       case WebSocket::WS_ERR_NO_HANDSHAKE:
       case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
       case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
        response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
        response.setContentLength(0);
        response.send();
        break;
      }
    }
  }
};

class RequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
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
    
    return new WebSocketRequestHandler;
  }
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
    HTTPServer srv(new RequestHandlerFactory, svs, new HTTPServerParams);

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

}  // namespcae osoa
