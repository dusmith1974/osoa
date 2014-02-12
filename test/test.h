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

#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include "boost/noncopyable.hpp"

#include "service/service.h"

namespace osoa {

class Test final : public Service, private boost::noncopyable {
 public:
  Test() : msg_count_(10) {}
  ~Test() {}

  int Initialize(int argc, const char *argv[]) override;

  int Start() override;
  int Stop() override;

 private:
  typedef Service super;

  int msg_count() { return msg_count_; }

  int msg_count_;
};

}  // namespace osoa
#endif  // TEST_TEST_H_
