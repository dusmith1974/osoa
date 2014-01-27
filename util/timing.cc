// Copyright 2013 Duncan Smith

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
