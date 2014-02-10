// Copyright 2014 Duncan Smith

#ifndef SERVICE_COMMS_H_
#define SERVICE_COMMS_H_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "boost/asio.hpp"
#include "boost/noncopyable.hpp"

namespace osoa {

namespace asio = boost::asio;
using boost::asio::ip::tcp;

typedef std::pair<std::shared_ptr<tcp::socket>, 
                  std::shared_ptr<tcp::resolver::iterator>> SocketPointPair;

typedef std::map<std::string, SocketPointPair> ServiceMap;

class Comms : boost::noncopyable {
 public:
  Comms() : io_service_(), service_map_{} {}
  ~Comms() {}

  int Listen(const std::vector<std::string>& ports);
  int ResolveServices(const std::vector<std::string>& services);
 
  void Connect(const std::string& service) const;

  const ServiceMap& service_map() const { return service_map_; }

 private:
  asio::io_service io_service_;
  asio::io_service& io_service() { return io_service_; }
  
  ServiceMap service_map_;
  ServiceMap& service_map() { return service_map_; }
};

}  // namespace osoa

#endif  // SERVICE_COMMS_H_
