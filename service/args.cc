// Copyright 2013 Duncan Smith
// https://github.com/dusmith1974/osoa
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "service/args.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "service/service.h"

namespace po = boost::program_options;

namespace osoa {

Args::Args()
    : module_path_(""),
      config_file_(""),
      log_dir_(""),
      no_log_file_(false),
      verbose_(false),
      silent_(false),
      listening_port_{},
      services_{},
      async_log_(false),
      auto_flush_log_(false),
      rotation_size_(1e2),
      config_(new po::options_description("Configuration:")),
      generic_(new po::options_description("Genric Options:")),
      hidden_(new po::options_description("Hidden Options:")),
      var_map_(new po::variables_map()) {
}

Args::~Args() {}

Error Args::Initialize(int argc, const char* argv[]) {
  set_module_path(argv[0]);

  AddOptionDescriptions();

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
        std::cerr << "Cannot open config file: " << config_file() << std::endl;
        return Error::kCannotParseArgs;
      } else {
        po::store(po::parse_config_file(ifs, config_file_options), var_map());
        po::notify(var_map());
      }
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return Error::kCannotParseArgs;
  }

  if (var_map().count("help") /*|| 1 == argc*/) {
    std::cout << visible_options << std::endl;
    return Error::kCannotParseArgs;  // Not strictly true,
                              // but we don't want to continue running either.
  }

  if (var_map().count("version")) {
    std::cout << Version() << std::endl;
    return Error::kCannotParseArgs;
  }

  if (var_map().count("verbose")) set_verbose(true);
  if (var_map().count("silent")) set_silent(true);
  if (var_map().count("no-log-file")) set_no_log_file(true);
  if (var_map().count("async-log")) set_async_log(true);
  if (var_map().count("auto-flush-log")) set_auto_flush_log(true);

  return Error::kSuccess;
}

// Generic options.
const std::string& Args::config_file() const { return config_file_; }

// Configuration options.
const std::string& Args::log_dir() const { return log_dir_; }
bool Args::no_log_file() const { return no_log_file_; }
bool Args::verbose() const { return verbose_; }
bool Args::silent() const { return silent_; }

const std::string Args::listening_port() const {
  return listening_port_;
}

const std::vector<std::string> Args::services() const {
  return services_;
}

// Hidden options.
bool Args::async_log() const { return async_log_; }
bool Args::auto_flush_log() const { return auto_flush_log_; }
int Args::rotation_size() const { return rotation_size_; }

// program_options objects.
po::options_description& Args::config() { return *(config_.get()); }
po::variables_map& Args::var_map() { return *(var_map_.get()); }

// Path to this app.
const std::string& Args::module_path() const { return module_path_; }

const int Args::version_major_no_ = 0;
const int Args::version_minor_no_ = 1;

// Sets the long and short identifier and a description for each program option.
void Args::AddOptionDescriptions() {
  AddGenericOptionDescriptions();
  AddConfigOptionDescriptions();
  AddHiddenOptionDescriptions();
}

// Adds a description for options to display the usage, vesion and to point to a
// configuration file.
void Args::AddGenericOptionDescriptions() {
  auto config_file_option =
    new po::typed_value<decltype(config_file_)>(&config_file_);
  config_file_option->value_name("filename");

  generic().add_options()
    ("help,h", "show help message")
    ("version,V", "print version information")
    ("config,c", config_file_option, "name of (optional) config file");
}

// Adds a description for common options which control the service behavior.
void Args::AddConfigOptionDescriptions() {
  auto log_dir_option = new po::typed_value<decltype(log_dir_)>(&log_dir_);
  log_dir_option->value_name("directory");

  auto listening_port_option =
    new po::typed_value<decltype(listening_port_)>(&listening_port_);
  listening_port_option->value_name("service_name|port");

  auto services_option =
    new po::typed_value<decltype(services_)>(&services_);
  services_option->value_name("{server:(service_name|port)}");
  
  config().add_options()
    ("log-dir,d", log_dir_option, "set loggng directory")
    ("no-log-file,n", "no logging to file")
    ("verbose,v", "set verbose logging")
    ("silent,S", "minimal console logging")
    ("listening-port,p", listening_port_option, "open listening port(s)")
    ("services,s", services_option->multitoken(),
      "list of service(s)");
}

// Adds descriptions for lesser used options that we don't display on the 
// command line interface. These options are set in the config file only.
void Args::AddHiddenOptionDescriptions() {
  auto rotation_size_option =
    new po::typed_value<decltype(rotation_size_)>(&rotation_size_);
  rotation_size_option->value_name("MB");

  hidden().add_options()
    ("async-log", "asynchonous logging thread")
    ("auto-flush-log", "flush log-file on each written record")
    ("rotation-size", rotation_size_option, "rotate logs every n MB");
}

const std::string Args::Version() const {
  std::stringstream ss;
  ss << "osoa " << std::to_string(version_major_no()) << "."
    << version_minor_no() << std::endl
    << "Copyright (C) 2014 Duncan Smith" << std::endl;

  return ss.str();
}

int Args::version_major_no() const { return version_major_no_; }
int Args::version_minor_no() const { return version_minor_no_; }

void Args::set_module_path(const std::string& val) { module_path_ = val; }

// Configuration options.
void Args::set_no_log_file(bool val) { no_log_file_ = val; }
void Args::set_verbose(bool val) { verbose_ = val; }
void Args::set_silent(bool val) { silent_ = val; }

// Hidden options.
void Args::set_async_log(bool val) { async_log_ = val; }
void Args::set_auto_flush_log(bool val) { auto_flush_log_ = val; }

po::options_description& Args::generic() { return *(generic_.get()); }
po::options_description& Args::hidden() { return *(hidden_.get()); }

}  // namespace osoa
