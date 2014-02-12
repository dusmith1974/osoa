// Copyright 2013 Duncan Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SERVICE_LOGGING_H_
#define SERVICE_LOGGING_H_

#include <memory>
#include <string>

#include "boost/log/sinks.hpp"
#include "boost/log/trivial.hpp"
#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"

namespace fs = boost::filesystem;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace blt = boost::log::trivial;

namespace osoa {

class Args;

class Logging final : boost::noncopyable {
 public:
  typedef sinks::asynchronous_sink<sinks::text_file_backend> AsyncSink;

  Logging();
  ~Logging() {}

  int Initialize(std::shared_ptr<const Args> args);

  static const std::string& log_header() { return Logging::log_header_; }

  std::shared_ptr<src::severity_logger_mt<blt::severity_level>> svc_logger() {
    return svc_logger_;
  }

  boost::shared_ptr<AsyncSink> async_sink() { return async_sink_; }

 private:
  typedef sinks::synchronous_sink<sinks::text_file_backend> SyncSink;
  typedef boost::shared_ptr<sinks::text_file_backend> TextFileBackend;

  void SetupLogFile(std::shared_ptr<const Args> args, const fs::path& path);
  void WriteLogHeader(std::shared_ptr<const Args> args) const;

  const TextFileBackend SetupTextfileBackend(std::shared_ptr<const Args> args,
                                       const fs::path& path) const;

  static void set_log_header(const std::string& val) {
    Logging::log_header_ = val;
  }

  static std::string log_header_;

  std::shared_ptr<
    src::severity_logger_mt<boost::log::trivial::severity_level>> svc_logger_;

  boost::shared_ptr<AsyncSink> async_sink_;
};

}  // namespace osoa

#endif  // SERVICE_LOGGING_H_
