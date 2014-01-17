// Copyright 2013 Duncan Smith

#ifndef SERVICE_SERVICE_H_
#define SERVICE_SERVICE_H_

#include <memory>

namespace osoa {

class Args;
class Logging;

class Service {
 public:
  Service();
  ~Service();

  int Initialize(int argc, const char *argv[]);

  int Start();
  int Stop();

 private:
  std::unique_ptr<Args> args_;
  Args& args() { return *(args_.get()); }
  
  std::unique_ptr<Logging> logging_;
  Logging& logging() { return *(logging_.get()); }
};

}  // namespace osoa
#endif  // SERVICE_SERVICE_H_
