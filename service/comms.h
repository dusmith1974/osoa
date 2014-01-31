// Copyright 2013 Duncan Smith

#ifndef SERVICE_COMMS_H_
#define SERVICE_COMMS_H_

#include "boost/noncopyable.hpp"

namespace osoa {

class Comms : boost::noncopyable {
 public:
  Comms() {}
  ~Comms() {}

  void Test();
};

}  // namespace osoa

#endif  // SERVICE_COMMS_H_
