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

// Contains an example service 'Test' which excercises the 'Service' base class.
// test -h shows all available options.
// Example usage:
//  test -p osoa ;Creates a listening port for the osoa service.
//  test -s localhost:daytime localhost:osoa ;Connects to two local sercices.

#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <string>

#include "boost/noncopyable.hpp"

#include "service/service.h"

namespace osoa {

// Example service class which listens for, or connects to other services,
// parses command line args and writes to a logfile.
// See comment at top of file for a complete description.
class Test final : public Service, private boost::noncopyable {
 public:
  Test();
  ~Test();

  // Initializes the service ready for use and adds command line options
  // specific to this service.
  Error Initialize(int argc, const char *argv[]) override;

  // Starts the service, logs messages and connects to other services.
  Error Start() override;

  // Stops the service.
  Error Stop() override;

 private:
  typedef Service super;

  std::string OnConnect();

  size_t msg_count();

  // Holds a count of the number of times we should log the test message.
  size_t msg_count_;
};

}  // namespace osoa
#endif  // TEST_TEST_H_
