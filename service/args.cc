// Copyright 2013 Duncan Smith

#include "service/args.h"

#include <iostream>
#include <string>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace po = boost::program_options;

namespace osoa {

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
 
  using namespace boost::log;
  auto log_level = (var_map().count("verbose")) ? trivial::debug : trivial::info;
  core::get()->set_filter(trivial::severity >= log_level);
}

}  // namespace osoa
