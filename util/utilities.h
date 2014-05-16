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

#ifndef UTIL_UTILITIES_H_
#define UTIL_UTILITIES_H_

#include <string>

#include "boost/algorithm/string/trim.hpp"

namespace osoa {

// Cancels unused parameter warnings.
template<class T> void unused(const T &);

// Removes EOL chars from end of strings.
std::string* TrimLastNewline(std::string* str);

// TODO(ds) mv to impl or inl.
template <typename charT, typename traits>
inline std::basic_istream<charT,traits>& ignore_line(std::basic_istream<
    charT,traits>& strm) {
  strm.ignore(std::numeric_limits<std::streamsize>::max(),
              strm.widen('\n'));

   return strm;
}
}  // namespace osoa

#endif  // UTIL_UTILITIES_H_
