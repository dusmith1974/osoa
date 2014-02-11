// Copyright 2014 Duncan Smith

#include "service/comms.h"

#include <iostream>

#include "boost/algorithm/string.hpp"
#include "boost/array.hpp"


namespace osoa {

// Iterative Server (handle one connection at a time).
int Comms::Listen(const std::vector<std::string>& ports) {
  for (auto& port : ports) {
    try {
      tcp::resolver resolver(io_service());
      tcp::resolver::query query(tcp::v4(), "127.0.0.1", port);
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
      
      int port_number = 0;
      for (; endpoint_iterator != tcp::resolver::iterator(); 
            ++ endpoint_iterator)
        port_number = endpoint_iterator->endpoint().port();

      tcp::acceptor acceptor(io_service(),
                             tcp::endpoint(tcp::v4(), port_number));

      int connections = 0;
      for (;;) {
        tcp::socket socket(io_service());
        acceptor.accept(socket);

        std::stringstream ss;
        ss << "OSOA comms connection: " << ++connections;

        boost::system::error_code ignored_error;
        asio::write(socket, asio::buffer(ss.str()), ignored_error);
      }
    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }

  return 0;
}

// Make a synchronous connection.
int Comms::ResolveServices(const std::vector<std::string>& services) {

  for (auto& service : services) {
    std::vector<std::string> server_service;
    boost::split(server_service, service, boost::is_any_of(":"));
    if (server_service.size() < 2)
      continue;

    try {
      tcp::resolver resolver(io_service());
      tcp::resolver::query query(server_service[0], server_service[1]);
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

      service_map()[server_service[1]] = std::make_pair(
        std::make_shared<tcp::socket>(io_service()), 
        std::make_shared<tcp::resolver::iterator>(resolver.resolve(query)));

      tcp::socket socket(io_service_);

      std::cout << std::endl;
    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }

  return 0;
}

void Comms::Connect(const std::string& service) const {
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

          std::cout.write(buf.data(), len);
        }
      } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
      }

      std::cout << std::endl;
      break;
    }
  }
}
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
