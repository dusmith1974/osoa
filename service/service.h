// Copyright 2013 Duncan Smith

#ifndef SERVICE_SERVICE_H_
#define SERVICE_SERVICE_H_

#include <chrono>
#include <memory>

namespace osoa {

typedef std::chrono::time_point<std::chrono::steady_clock> timepoint;

class Args;
class Logging;
class Comms;

class Service {
 public:
  Service();
  virtual ~Service();

  virtual int Initialize(int argc, const char *argv[]);

  virtual int Start();
  virtual int Stop();

 protected:
  std::shared_ptr<Args> args() { return args_; }
  std::shared_ptr<Logging> logging() { return logging_; }
  std::shared_ptr<Comms> comms() { return comms_; }

 private:
  std::shared_ptr<Args> args_;

  std::shared_ptr<Logging> logging_;

  timepoint svc_start_time_;
  void set_svc_start_time(const timepoint& val) { svc_start_time_ = val; }
  const timepoint& svc_start_time() { return svc_start_time_; }

  timepoint svc_end_time_;
  void set_svc_end_time(const timepoint& val) { svc_end_time_ = val; }
  const timepoint& svc_end_time() { return svc_end_time_; }

  std::shared_ptr<Comms> comms_;
};

}  // namespace osoa
#endif  // SERVICE_SERVICE_H_
