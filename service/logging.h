// Copyright 2013 Duncan Smith 
// https://github.com/dusmith1974/osoa

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

#include "logging_fwd.h"

#include <string>

#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"

namespace fs = boost::filesystem;

namespace osoa {

class Args;

class Logging final : boost::noncopyable {
 public:
  ~Logging() {}

  static Logging& Instance();

  int Initialize(std::shared_ptr<const Args> args);
  void Detach();

  static const std::string& log_header() { return Logging::log_header_; }

  static SeverityLoggerPtr logger() { return logger_; }

  boost::shared_ptr<AsyncSink> async_sink() { return async_sink_; }

 private:
  typedef sinks::synchronous_sink<sinks::text_file_backend> SyncSink;
  typedef boost::shared_ptr<sinks::text_file_backend> TextFileBackend;

  Logging() : async_sink_(nullptr) {}

  void SetupLogFile(std::shared_ptr<const Args> args, const fs::path& path);
  void WriteLogHeader(std::shared_ptr<const Args> args) const;

  const TextFileBackend SetupTextfileBackend(std::shared_ptr<const Args> args,
                                       const fs::path& path) const;

  static void set_log_header(const std::string& val) {
    Logging::log_header_ = val;
  }

  static std::string log_header_;

  static SeverityLoggerPtr logger_;

  boost::shared_ptr<AsyncSink> async_sink_;
};

}  // namespace osoa

#endif  // SERVICE_LOGGING_H_
