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

#ifndef UTIL_TIMING_H_
#define UTIL_TIMING_H_

#include <chrono>
#include <string>
#include <utility>

namespace osoa {

typedef std::chrono::time_point<std::chrono::steady_clock> SteadyTimepoint;
typedef std::pair<SteadyTimepoint, SteadyTimepoint> SteadyTimepointPair;

std::string add_timestamp(const SteadyTimepointPair& timepoints);

}  // namespace osoa

#endif  // UTIL_TIMING_H_
