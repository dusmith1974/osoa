// Copyright 2013 Duncan Smith

#include "test/test.h"

#include <iostream>

#include "boost/program_options.hpp"

namespace po = boost::program_options;

namespace osoa {

Test::Test() {}
Test::~Test() {}

int Test::Initialize(int argc, const char *argv[]) {

  return Service::Initialize(argc, argv);
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
// po::options_description& config()
