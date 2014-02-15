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

// Defines forward declares for using the logging class.

#ifndef SERVICE_LOGGING_FWD_H_
#define SERVICE_LOGGING_FWD_H_

#include <memory>

#include "boost/log/sinks.hpp"
#include "boost/log/trivial.hpp"

namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace blt = boost::log::trivial;

namespace osoa {
  class Logging;

  typedef sinks::asynchronous_sink<sinks::text_file_backend> AsyncSink;
  typedef src::severity_logger_mt<blt::severity_level> SeverityLogger;
  typedef std::shared_ptr<SeverityLogger> SeverityLoggerPtr;
  typedef std::shared_ptr<Logging> LoggingPtr;
}  // namespace osoa

#endif  // SERVICE_LOGGING_FWD_H_
