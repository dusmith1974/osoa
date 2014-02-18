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

#include "service/comms.h"

#include <iostream>

#include "boost/algorithm/string.hpp"
#include "boost/array.hpp"

#include "service/logging.h"
#include "service/service.h"

namespace osoa {

Comms::Comms()
  : io_service_(),
    service_map_{} {}

Comms::~Comms() {}

// Create the listening port for an iterative server (that handles one 
// connection at a time). Resolve the service name or port against localhost to
// get the port number and then open the listening port to accept connections.
Error Comms::Listen(const std::string port) {
  try {
    BOOST_LOG_SEV(*Logging::logger(), blt::debug) << "Listening for port <"
      << port << ">";

    // Get the port number (as we may be dealing with a service name).
    int port_number = 0;
    tcp::resolver resolver(io_service());
    tcp::resolver::query query(tcp::v4(), "127.0.0.1", port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    for (; endpoint_iterator != tcp::resolver::iterator();
          ++endpoint_iterator)
      port_number = endpoint_iterator->endpoint().port();

    // Open the listening port and handle incoming connections.
    tcp::acceptor acceptor(io_service(),
                           tcp::endpoint(tcp::v4(), port_number));
    for (;;) {
      tcp::socket socket(io_service());
      acceptor.accept(socket);

      boost::system::error_code ignored_error;
      asio::write(socket, asio::buffer(OnConnect()), ignored_error);
    }
  }
  catch (std::exception& e) {
    BOOST_LOG_SEV(*Logging::logger(), blt::info)
      << "Could not open listening port <" << port << ">"
      << std::endl << e.what();
    return Error::kCouldNotOpenListeningPort;
  }

  return Error::kSuccess;
}

// For each service URI passed in services, resolve the service and populate the
// service map with the socket.
Error Comms::ResolveServices(const std::vector<std::string>& services) {
  for (auto& service : services) {
    // Split the hostname and service/port. 
    std::vector<std::string> server_service;
    boost::split(server_service, service, boost::is_any_of(":"));
    if (2 != server_service.size()) {
      BOOST_LOG_SEV(*Logging::logger(), blt::info)
        << "Invalid URI specified for service <" << service << ">" << std::endl
        << "Expected: hostname:port|service";
        return Error::kInvalidURI;
    } 

    // Resolve each service and place the connection socket in the service map.
    try {
      tcp::resolver resolver(io_service());
      tcp::resolver::query query(server_service[0], server_service[1]);
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

      service_map()[server_service[1]] = std::make_pair(
        std::make_shared<tcp::socket>(io_service()),
        std::make_shared<tcp::resolver::iterator>(resolver.resolve(query)));

    } catch (std::exception& e) {
      BOOST_LOG_SEV(*Logging::logger(), blt::info)
        << "Could not resolve service <" << service << ">"
        << std::endl << e.what();

      return Error::kCouldNotResolveService;
    }
  }

  return Error::kSuccess;
}

boost::optional<std::string> Comms::Connect(const std::string& service) const {
  // TODO(ds) find in map w/o loop
  std::stringstream ss;
  for (const auto& socket_pair : service_map()) {
    if (socket_pair.first == service) {
      try {
        asio::connect(*socket_pair.second.first, *socket_pair.second.second);

        for (;;) {
          boost::array<char, 128> buf;
          boost::system::error_code error;

          size_t len = socket_pair.second.first->read_some(asio::buffer(buf),
                                                           error);
          if (error == asio::error::eof)
            break;
          else if (error)  // ie not boost::system::errc::success
            throw boost::system::system_error(error);

          ss.write(buf.data(), len);
        }
      } catch (std::exception& e) {
        BOOST_LOG_SEV(*Logging::logger(), blt::debug)
          << "Exception thrown in Comms::Connect <" << e.what() << ">";

        return boost::optional<std::string>();
      }

      break;
    }
  }

  return boost::optional<std::string>(ss.str());
}

std::string Comms::OnConnect() {
  std::stringstream ss;
  ss << "OSOA comms connection" << std::endl;
  return ss.str();
}

asio::io_service& Comms::io_service() { return io_service_; }

Comms::ServiceMap& Comms::service_map() { return service_map_; }
const Comms::ServiceMap& Comms::service_map() const { return service_map_; }

}  // namespace osoa

// Timer examples
// #include "boost/bind.hpp"
// #include "boost/date_time/posix_time/posix_time.hpp"

/*void print(const boost::system::error_code&) {
  std::cout << "async waited" << std::endl;
}

void print_count(const boost::system::error_code&,
                 boost::asio::deadline_timer* t,
                 int* count) {
  if (*count < 5) {
    std::cout << "count " << *count << std::endl;
    ++(*count);

    t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
    t->async_wait(boost::bind(print_count, boost::asio::placeholders::error, t, count));
  }
}*/

/*void Comms::Test() {
  // sync wait
  boost::asio::io_service io;
  boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
  std::cout << "waiting" << std::endl;
  t.wait();
  std::cout << "waited" << std::endl;

  // async wait
  std::cout << "async wait" << std::endl;
  boost::asio::deadline_timer t2(io, boost::posix_time::seconds(1));
  t2.async_wait(&print);
  io.run();

  int count = 0;
  boost::asio::deadline_timer t3(io, boost::posix_time::seconds(1));
  t3.async_wait(boost::bind(print_count,
    boost::asio::placeholders::error, &t3, &count));

  io.reset();
  io.run();

  std::cout << "final countdown " << count << std::endl;
}*/
