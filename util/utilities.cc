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

#include "osoa_pch.h"  // NOLINT

#include "util/utilities.h"

#include <cmath>  // NOLINT

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

  int BitNum(int bit_val) {
    return static_cast<int>(log2(bit_val) / log2(2));
  }

  void Round(int places, double* val) {
    if (!val || 1 > places) return;

    int factor = static_cast<int>(pow(10.0, places));
    *val = (*val >= 0.0) ?
      floor(*val * factor + 0.5) / factor :
      ceil(*val * factor - 0.5) / factor;
  }

  bool EqualToPlaces(double lhs, double rhs, int places) {
    return (abs(lhs - rhs) < pow(10.0, -places));
  }

  bool ApproximatelyEqual(float a, float b, float epsilon) {
    return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
  }

  bool EssentiallyEqual(float a, float b, float epsilon) {
    return fabs(a - b) <= ((fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
  }
}  // namespace osoa
