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

#include "util/timing.h"

#include <sstream>

namespace osoa {

std::string add_timestamp(const SteadyTimepointPair& timepoints) {
  auto micro = std::chrono::duration_cast<std::chrono::microseconds>(
    timepoints.second - timepoints.first);
  auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(
    timepoints.second - timepoints.first);
  auto sec = std::chrono::duration_cast<std::chrono::seconds>(
    timepoints.second - timepoints.first);

  std::stringstream ss;
  auto elapsed = timepoints.second - timepoints.first;
  ss << elapsed.count() << " ticks of "
    << std::chrono::steady_clock::period::num
    << "/" << std::chrono::steady_clock::period::den << " (";

  if (sec.count())
    ss << sec.count() << " seconds)";
  else if (milli.count())
    ss << milli.count() << " milli-seconds)";
  else
    ss << micro.count() << " micro-seconds)";

  return (std::string(ss.str()));
}

}  // namespace osoa
