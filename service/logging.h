// Copyright 2013 Duncan Smith
// https://github.com/dusmith1974/osoa
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

// Contains the logging class.

#ifndef SERVICE_LOGGING_H_
#define SERVICE_LOGGING_H_

#include <string>

#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"

#include "service/logging_fwd.h"
#include "service/service_fwd.h"

namespace fs = boost::filesystem;

namespace osoa {

class Args;

// Uses the boost log library to provide logging to the console and a logfile.
// Logging may be synchronous and auto-flushed (slowest, but most reliable logs 
// in the event of a failure), or asynchronous and non-flushed (recommended for
// performant systems). Log files are rotated at a given size and contain a 
// header detailing the options that the service was called with.
class Logging final : boost::noncopyable {
 public:
  ~Logging();

  // Accesses the singleton.
  static Logging& Instance();

  // Initialzes the logging environment accrding to the options specified in the
  // args.
  Error Initialize(std::shared_ptr<const Args> args);
  
  // Detaches the logging front-ends from the core (shuts down logging before 
  // service termination).
  void Detach();

  // The header to write at the top of rotated log files.
  static const std::string& log_header();

  // Accesses the logger (to make log entries).
  static SeverityLoggerPtr logger();

  // Accesses the asynchronous log sink.
  boost::shared_ptr<AsyncSink> async_sink();

 private:
  typedef sinks::synchronous_sink<sinks::text_file_backend> SyncSink;
  typedef boost::shared_ptr<sinks::text_file_backend> TextFileBackend;

  // Private constructor for singleton.
  Logging();

  // Sets up the logfile in the specified path.
  void SetupLogFile(std::shared_ptr<const Args> args, const fs::path& path);
  
  // Writes a header at the top of a new logfile.
  void WriteLogHeader(std::shared_ptr<const Args> args) const;

  // Sets up the backend with the properties set by args.
  const TextFileBackend SetupTextfileBackend(std::shared_ptr<const Args> args,
                                       const fs::path& path) const;

  static void set_log_header(const std::string& val);
  static std::string log_header_;
  static SeverityLoggerPtr logger_;

  boost::shared_ptr<AsyncSink> async_sink_;
};

}  // namespace osoa

#endif  // SERVICE_LOGGING_H_
