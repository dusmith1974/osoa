// Copyright 2013 Duncan Smith

#ifndef SERVICE_SERVICE_H_
#define SERVICE_SERVICE_H_

#include <memory>

namespace osoa {
class Args;

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
};

}  // namespace osoa
#endif  // SERVICE_SERVICE_H_
