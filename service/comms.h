// Copyright 2013 Duncan Smith

#ifndef SERVICE_COMMS_H_
#define SERVICE_COMMS_H_

#include <string>

#include "boost/noncopyable.hpp"

namespace osoa {

class Comms : boost::noncopyable {
 public:
  Comms() {}
  ~Comms() {}

  int Subscribe(const std::string& server, const std::string& service);
  //void Test();
};

}  // namespace osoa

#endif  // SERVICE_COMMS_H_
