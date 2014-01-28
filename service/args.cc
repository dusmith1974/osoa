// Copyright 2013 Duncan Smith

#include "service/args.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace po = boost::program_options;

namespace osoa {

const int Args::version_major_no_ = 0;
const int Args::version_minor_no_ = 1;

Args::Args()
    : generic_(new po::options_description("Genric Options:")),
      config_(new po::options_description("Configuration:")),
      hidden_(new po::options_description("Hidden Options:")),
      var_map_(new po::variables_map()),
      log_dir_(""),
      module_path_(""),
      no_log_file_(false),
      config_file_(""),
      async_log_(false),
      auto_flush_log_(false),
      rotation_size_(1e2),
      verbose_(false),
      silent_(false) {
}

Args::~Args() {
}

int Args::Initialize(int argc, const char* argv[]) {
  set_module_path(argv[0]);

  auto log_dir_option = new po::typed_value<decltype(log_dir_)>(&log_dir_);
  log_dir_option->value_name("directory");

  auto config_file_option =
    new po::typed_value<decltype(config_file_)>(&config_file_);
  config_file_option->value_name("filename");

  generic().add_options()
    ("help,h", "show help message")
    ("version,V", "print version information")
    ("config,c", config_file_option, "name of (optional) config file");

  config().add_options()
    ("log-dir,l", log_dir_option, "set loggng directory")
    ("no-log-file,n", "no logging to file")
    ("verbose,v", "set verbose logging")
    ("silent,s", "minimal console logging");

  auto rotation_size_option =
    new po::typed_value<decltype(rotation_size_)>(&rotation_size_);
  rotation_size_option->value_name("MB");

  hidden().add_options()
    ("async-log", "asynchonous logging thread")
    ("auto-flush-log", "flush log-file on each written record")
    ("rotation-size", rotation_size_option, "rotate logs every n MB");

  po::options_description cmdline_options;
  cmdline_options.add(generic()).add(config()).add(hidden());

  po::options_description config_file_options;
  config_file_options.add(config()).add(hidden());

  po::options_description visible_options("Allowed options");
  visible_options.add(generic()).add(config());

  try {
    po::store(po::parse_command_line(argc, argv, cmdline_options), var_map());
    po::notify(var_map());

    if (var_map().count("config")) {
      std::ifstream ifs(config_file().c_str());
      if (!ifs) {
        std::cout << "Cannot open config file: " << config_file() << std::endl;
        return 1;
      } else {
        po::store(po::parse_config_file(ifs, config_file_options), var_map());
        po::notify(var_map());
      }
    }
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }

  if (var_map().count("help") /*|| 1 == argc*/) {
    std::cout << visible_options << std::endl;
    return 1;
  }

  if (var_map().count("version")) {
    std::cout << Version() << std::endl;
    return 1;
  }

  if (var_map().count("verbose")) set_verbose(true);
  if (var_map().count("silent")) set_silent(true);
  if (var_map().count("no-log-file")) set_no_log_file(true);
  if (var_map().count("async-log")) set_async_log(true);
  if (var_map().count("auto-flush-log")) set_auto_flush_log(true);

  return 0;
}

std::string Args::Version() {
  std::stringstream ss;
  ss << "osoa " << std::to_string(version_major_no()) << "."
    << version_minor_no() << std::endl
    << "Copyright (C) 2014 Duncan Smith" << std::endl;

  return ss.str();
}

}  // namespace osoa
