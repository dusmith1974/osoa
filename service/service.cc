// Copyright 2013 Duncan Smith

#include "service/service.h"

#include <iostream>

#include <service/args.h> // NOLINT

namespace osoa {

Service::Service() : args_(new Args()) {}

Service::~Service() {}

void Service::Initialize(int argc, const char *argv[]) {
  args().Initialize(argc, argv); 
}

}  // namespace osoa
