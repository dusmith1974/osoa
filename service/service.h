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

#ifndef SERVICE_SERVICE_H_
#define SERVICE_SERVICE_H_

#include <chrono>
#include <memory>

namespace osoa {

class Args;
class Logging;
class Comms;

class Service {
 public:
  Service();
  virtual ~Service() {}

  virtual int Initialize(int argc, const char *argv[]);

  virtual int Start();
  virtual int Stop();

 protected:
  std::shared_ptr<Args> args() { return args_; }
  std::shared_ptr<Logging> logging() { return logging_; }
  std::shared_ptr<Comms> comms() { return comms_; }

 private:
  typedef std::chrono::time_point<std::chrono::steady_clock> timepoint;

  const timepoint& svc_start_time() const { return svc_start_time_; }
  void set_svc_start_time(const timepoint& val) { svc_start_time_ = val; }

  const timepoint& svc_end_time() const { return svc_end_time_; }
  void set_svc_end_time(const timepoint& val) { svc_end_time_ = val; }

  timepoint svc_start_time_;
  timepoint svc_end_time_;

  std::shared_ptr<Args> args_;
  std::shared_ptr<Logging> logging_;
  std::shared_ptr<Comms> comms_;
};

}  // namespace osoa
#endif  // SERVICE_SERVICE_H_
