// Copyright 2013 Duncan Smith

#ifndef SERVICE_LOGGING_H_
#define SERVICE_LOGGING_H_

#include <memory>
#include <string>

#include "boost/log/trivial.hpp"
#include "boost/log/sinks.hpp"
#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"

namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace blt = boost::log::trivial;

typedef sinks::synchronous_sink<sinks::text_file_backend> sync_sink_t;
typedef sinks::asynchronous_sink<sinks::text_file_backend> async_sink_t;

namespace osoa {

class Args;

class Logging : boost::noncopyable {
 public:
  Logging();
  ~Logging();

  int Initialize(const Args& args);
  void WriteLogHeader(const Args& args);

  std::shared_ptr<src::severity_logger_mt<blt::severity_level>> svc_logger() {
    return svc_logger_;
  }

  boost::shared_ptr<async_sink_t> async_sink() { return async_sink_; }

  static const std::string& log_header() { return Logging::log_header_; }
  static void set_log_header(const std::string& val) {
    Logging::log_header_ = val;
  }

 private:
  std::shared_ptr<
    src::severity_logger_mt<boost::log::trivial::severity_level>> svc_logger_;

  boost::shared_ptr<async_sink_t> async_sink_;

  static std::string log_header_;
};

}  // namespace osoa

#endif  // SERVICE_LOGGING_H_
