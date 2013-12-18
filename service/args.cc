// Copyright 2013 Duncan Smith

#include "service/args.h"

#include <iostream>

namespace po = boost::program_options;

namespace osoa {

Args::Args(int argc, char* argv)
    : usage_(new po::options_description("Options:")) {
}

}  // namespace osoa
