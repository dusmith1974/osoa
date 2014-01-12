// Copyright 2013 Duncan Smith

#include "service/args.h"

#include <iostream>
#include <sstream>
#include <string>

namespace po = boost::program_options;

namespace osoa {

const int Args::version_major_no_ = 0;
const int Args::version_minor_no_ = 1;

Args::Args()
    : usage_(new po::options_description("Options:")),
      var_map_(new po::variables_map()) {
}

void Args::Initialize(int argc, const char* argv[]) {
  auto log_dir_option = new po::typed_value<decltype(log_dir_)>(&log_dir_);
  log_dir_option->value_name("directory");

  usage().add_options()
    ("help,h", "show help message")
    ("log-dir,l", log_dir_option, "set loggng directory")
    ("no-log-file,n", "no logging to file")
    ("verbose,v", "set verbose logging")
    ("version,V", "print version information and exit") 
    ;
 
  try {
    po::store(po::parse_command_line(argc, argv, usage()), var_map());  
    po::notify(var_map());
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (var_map().count("help") /*|| 1 == argc*/) {
    std::cout << usage() << std::endl;
    exit(EXIT_SUCCESS);
  }
 
  if (var_map().count("version")) {
    std::cout << Version() << std::endl;
    exit(EXIT_SUCCESS);
  }

  if (var_map().count("verbose")) 
    set_verbose(true);
}

std::string Args::Version() {
  std::stringstream ss;
  ss << "osoa " << std::to_string(version_major_no()) << "." << version_minor_no() << std::endl
    << "Copyright (C) 2014 Duncan Smith" << std::endl;

  return ss.str();
}

}  // namespace osoa
