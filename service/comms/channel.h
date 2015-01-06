// Copyright 2014 Duncan Smith
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

// Contains a class representing a Channel.

#ifndef SERVICE_COMMS_CHANNEL_H_
#define SERVICE_COMMS_CHANNEL_H_

#include <set>
#include <string>

#include "service/comms/subscriber_fwd.h"

namespace osoa {
// The Channel class.
class Channel {
 public:
  Channel();
  ~Channel();

  void Join(SubscriberPtr subscriber);
  void Leave(SubscriberPtr subscriber);
  void Deliver(const std::string& msg);

 private:
  std::set<SubscriberPtr> subscribers_;
};
}  // namespace osoa

#endif  // SERVICE_COMMS_CHANNEL_H_
