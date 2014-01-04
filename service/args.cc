// Copyright 2013 Duncan Smith

#include "service/args.h"

#include <iostream>
#include <string>

namespace po = boost::program_options;

namespace osoa {

Args::Args()
    : usage_(new po::options_description("Options:")),
      var_map_(new po::variables_map()) {
}

void Args::Initialize(int argc, const char* argv[]) {
  usage().add_options()
    ("help", "show help message")
    ("logdir", po::value<std::string>(), "set loggng directory");
 
  po::store(po::parse_command_line(argc, argv, usage()), var_map());  
  po::notify(var_map());

  if (var_map().count("help")) {
    std::cout << usage() << std::endl;
    exit(0);
  }
}

}  // namespace osoa
