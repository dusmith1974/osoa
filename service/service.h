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

// Contains the service base class. For an example of overriding see
// service/test.h.

#ifndef SERVICE_SERVICE_H_
#define SERVICE_SERVICE_H_

#include <memory>

#include "boost/asio.hpp"
#include "boost/chrono.hpp"

namespace osoa {
class Args;
class Comms;

// Error codes returned from the service related functions,
enum class Error {
  kSuccess,
  kCouldNotResolveService,
  kCouldNotOpenListeningPort,
  kCouldNotPublishTopics,
  kCannotParseArgs,
  kInvalidURI,
  kCouldNotSubscribeToService,
  kInvalidArgs
};

// A base class for services, providing common features for parsing args,
// logging and inter-process communications.
class Service {
 public:
  Service();
  virtual ~Service();

  // Parses args and sets up the logging.
  virtual Error Initialize(int argc, const char* argv[]);

  // Starts the service and sets up any networking.
  Error Start();

  // Stops the service and disables logging.
  Error Stop();

  bool publishing() const;
  void set_publishing(bool val);

 protected:
  std::shared_ptr<Args> args();
  std::shared_ptr<Comms> comms();

 private:
  // Override in the subclass to add the jobs for the service.
  virtual Error DoStart();

  // Override in the subclass to perform the cleanup for the service.
  virtual Error DoStop();
  void AbortHandler(const boost::system::error_code& error, int signal_number);

  int abort_signal_;

  typedef boost::chrono::time_point<boost::chrono::steady_clock> Timepoint;

  const Timepoint& svc_start_time() const;
  void set_svc_start_time(const Timepoint& val);

  const Timepoint& svc_end_time() const;
  void set_svc_end_time(const Timepoint& val);

  Timepoint svc_start_time_;
  Timepoint svc_end_time_;

  std::shared_ptr<Args> args_;
  std::shared_ptr<Comms> comms_;

  bool publishing_;
};
}  // namespace osoa
#endif  // SERVICE_SERVICE_H_
