// Copyright 2014 Duncan Smith

#ifndef SERVICE_COMMS_H_
#define SERVICE_COMMS_H_

#include <string>
#include <vector>

#include "boost/noncopyable.hpp"

namespace osoa {

class Comms : boost::noncopyable {
 public:
  Comms() {}
  ~Comms() {}

  int Publish();
  int Subscribe(const std::vector<std::string>& subscriptions);
  //void Test();
};

}  // namespace osoa

#endif  // SERVICE_COMMS_H_
