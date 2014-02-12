// Copyright 2013 Duncan Smith

#include "test/test.h"

#include <iostream>

#include "boost/program_options.hpp"

#include "service/args.h"
#include "service/comms.h"
#include "service/logging.h"

namespace po = boost::program_options;

namespace osoa {

int Test::Initialize(int argc, const char *argv[]) {
  po::options_description& config = args()->config();

  auto msg_count_option =
    new po::typed_value<decltype(msg_count_)>(&msg_count_);
  msg_count_option->value_name("number");

  config.add_options()
    ("msg-count,o", msg_count_option, "number of msgs");

  return Service::Initialize(argc, argv);
}

int Test::Start() {
  int result = super::Start();
  if (0 != result) return result;

  auto lg = logging()->svc_logger();
  for (int j = 0; j < msg_count();  ++j)
    BOOST_LOG_SEV(*lg, blt::debug)
      << "The quick brown fox jumped over the lazy dog.";

  comms()->Connect("osoa");
  comms()->Connect("osoa");
  comms()->Connect("daytime");
  comms()->Connect("daytime");

  return 0;
}

int Test::Stop() {
  return super::Stop();
}

}  // namespace osoa

int main(int argc, const char *argv[]) {
  osoa::Test service;

  int result = service.Initialize(argc, argv);
  if (0 != result)
    return result;

  if (0 == service.Start())
    service.Stop();

  return 0;
}
