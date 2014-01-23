// Copyright 2013 Duncan Smith

#ifndef SERVICE_LOGGING_H_
#define SERVICE_LOGGING_H_

#include <memory>

#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/noncopyable.hpp"

namespace src = boost::log::sources;

namespace osoa {

class Args;

class Logging : boost::noncopyable {
 public:
  Logging();
  ~Logging();

  int Initialize(const Args& args);
  void WriteLogHeader(const Args& args);

  src::severity_logger_mt<boost::log::trivial::severity_level>& svc_logger() {
    return *(svc_logger_.get());
  }

  static const std::string& log_header() { return Logging::log_header_; }
  static void set_log_header(const std::string& val) { 
    Logging::log_header_ = val; 
  }

 private:
  std::unique_ptr<
    src::severity_logger_mt<boost::log::trivial::severity_level>> svc_logger_;

  static std::string log_header_;
};

}  // namespace osoa

#endif  // SERVICE_LOGGING_H_
