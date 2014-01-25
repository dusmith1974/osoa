// Copyright 2013 Duncan Smith

#ifndef SERVICE_LOGGING_H_
#define SERVICE_LOGGING_H_

#include <memory>

#include "boost/log/trivial.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/noncopyable.hpp"
#include "boost/log/sinks.hpp"
#include "boost/log/sinks/unlocked_frontend.hpp"

namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;

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

  typedef sinks::synchronous_sink<sinks::text_file_backend> sync_sink_t;
  typedef sinks::asynchronous_sink<sinks::text_file_backend> async_sink_t;
  //std::shared_ptr<sinks::text_file_backend> backend_;

   
  //std::shared_ptr<sinks::basic_sink_frontend> frontend_;
  static std::string log_header_;
};

}  // namespace osoa

#endif  // SERVICE_LOGGING_H_
