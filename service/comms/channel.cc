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

// Implements the Channel class.

#include <algorithm>

#include "boost/bind.hpp"

#include "service/comms/channel.h"
#include "service/comms/subscriber.h"

namespace osoa {

Channel::Channel() : subscribers_{} {
}

Channel::~Channel() {
}

void Channel::Join(SubscriberPtr subscriber) {
  subscribers_.insert(subscriber);
}

void Channel::Leave(SubscriberPtr subscriber) {
  subscribers_.erase(subscriber);
}

void Channel::Deliver(const std::string& msg) {
  std::for_each(subscribers_.begin(), subscribers_.end(),
      bind(&Subscriber::Deliver, _1, boost::ref(msg)));
}

}  // namespcae osoa
