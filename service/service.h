// Copyright 2013 Duncan Smith

#ifndef SERVICE_SERVICE_H_
#define SERVICE_SERVICE_H_

#include <memory>

#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace src = boost::log::sources;

namespace osoa {

class Args;

class Service {
 public:
  Service();
  ~Service();

  int Initialize(int argc, const char *argv[]);

  int Start();
  int Stop();

  src::severity_logger_mt<boost::log::trivial::severity_level>& svc_logger() { return *(svc_logger_.get()); } 

 private:
  std::unique_ptr<Args> args_;
  Args& args() { return *(args_.get()); }

  std::unique_ptr<src::severity_logger_mt<boost::log::trivial::severity_level>> svc_logger_;
};

}  // namespace osoa
#endif  // SERVICE_SERVICE_H_
