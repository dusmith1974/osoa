// Copyright 2013 Duncan Smith

#include "service/comms.h"

#include <iostream>

#include "boost/array.hpp"
#include "boost/asio.hpp"
//#include "boost/bind.hpp"
//#include "boost/date_time/posix_time/posix_time.hpp"

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

namespace asio = boost::asio;
using boost::asio::ip::tcp;

namespace osoa {

int Comms::Subscribe(const std::string& server, const std::string& service) {
  asio::io_service io_service;

  tcp::resolver resolver(io_service);
  tcp::resolver::query query(server, service);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

  tcp::socket socket(io_service);
  asio::connect(socket, endpoint_iterator);

  try {
    for (;;) {
      boost::array<char, 128> buf;
      boost::system::error_code error;

      size_t len = socket.read_some(asio::buffer(buf), error);
      if (error == asio::error::eof) // boost::system::errc::success
        break;
      else if (error) 
        throw boost::system::system_error(error);

      std::cout.write(buf.data(), len);
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  
  return 0;
}

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
  t3.async_wait(boost::bind(print_count, boost::asio::placeholders::error, &t3, &count));

  io.reset();
  io.run();
  
  std::cout << "final countdown " << count << std::endl;
}*/
}  // namespace osoa
