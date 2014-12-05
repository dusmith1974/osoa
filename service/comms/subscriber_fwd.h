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

// Contains forward declarations for the Subscriber class.

#ifndef SERVICE__COMMS__SUBSCRIBER_FWD_H_
#define SERVICE__COMMS__SUBSCRIBER_FWD_H_

#include "boost/shared_ptr.hpp"

namespace osoa {

class Subscriber;
typedef boost::shared_ptr<Subscriber> SubscriberPtr;

}  // namespace osoa

#endif  // SERVICE__COMMS__SUBSCRIBER_FWD_H_