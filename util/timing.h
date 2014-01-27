// Copyright 2013 Duncan Smith

#ifndef UTIL_TIMING_H_
#define UTIL_TIMING_H_

#include <chrono>
#include <iostream>
#include <utility>
#include <string>

namespace osoa {

typedef std::chrono::time_point<std::chrono::steady_clock> SteadyTimepoint;
typedef std::pair<SteadyTimepoint, SteadyTimepoint> SteadyTimepointPair;

std::string add_timestamp(const SteadyTimepointPair& timepoints);

}  // namespace osoa

#endif  // UTIL_TIMING_H_
