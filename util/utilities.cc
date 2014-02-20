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

// General utility functions.

#include "util/utilities.h"

#include "boost/algorithm/string/trim.hpp"

namespace osoa {

// Template function with unnamed parameter and no-op to remove compiler
// warnings about unused parameters.
template<class T> void unused(const T&) {}

// Remove the right hand side end of line characters from the given str.
std::string* TrimLastNewline(std::string* str) {
  if (!str) return nullptr;
  boost::algorithm::trim_right_if(*str, boost::is_any_of("\r\n"));
  return str;
}

}  // namespace osoa

