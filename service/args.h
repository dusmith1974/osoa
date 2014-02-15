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

#ifndef SERVICE_ARGS_H_
#define SERVICE_ARGS_H_

#include <memory>
#include <string>
#include <vector>

#include "boost/noncopyable.hpp"
#include "boost/program_options.hpp"

#include "service/service_fwd.h"

namespace osoa {

namespace po = boost::program_options;

class Args final : boost::noncopyable {
 public:
  Args();
  ~Args() {}

  Error Initialize(int argc, const char* argv[]);

  // Generic options.
  const std::string& config_file() const { return config_file_; }

  // Configuration options.
  const std::string& log_dir() const { return log_dir_; }
  bool no_log_file() const { return no_log_file_; }
  bool verbose() const { return verbose_; }
  bool silent() const { return silent_; }

  const std::vector<std::string> listening_ports() const {
    return listening_ports_;
  }

  const std::vector<std::string> services() const {
    return services_;
  }

  // Hidden options.
  bool async_log() const { return async_log_; }
  bool auto_flush_log() const { return auto_flush_log_; }
  int rotation_size() const { return rotation_size_; }

  // program_options objects.
  po::options_description& config() { return *(config_.get()); }
  po::variables_map& var_map() { return *(var_map_.get()); }

  // Path to this app.
  const std::string& module_path() const { return module_path_; }

 private:
  const std::string Version() const;

  int version_major_no() const { return version_major_no_; }
  int version_minor_no() const { return version_minor_no_; }

  void set_module_path(const std::string& val) { module_path_ = val; }

  // Configuration options.
  void set_no_log_file(bool val) { no_log_file_ = val; }
  void set_verbose(bool val) { verbose_ = val; }
  void set_silent(bool val) { silent_ = val; }

  // Hidden options.
  void set_async_log(bool val) { async_log_ = val; }
  void set_auto_flush_log(bool val) { auto_flush_log_ = val; }

  po::options_description& generic() { return *(generic_.get()); }
  po::options_description& hidden() { return *(hidden_.get()); }

  static const int version_major_no_;
  static const int version_minor_no_;

  std::string module_path_;

  // Generic options.
  std::string config_file_;

  // Configuration options.
  std::string log_dir_;
  bool no_log_file_;
  bool verbose_;
  bool silent_;
  std::vector<std::string> listening_ports_;
  std::vector<std::string> services_;

  // Hidden options.
  bool async_log_;
  bool auto_flush_log_;
  int rotation_size_;

  std::unique_ptr<po::options_description> config_;
  std::unique_ptr<po::options_description> generic_;
  std::unique_ptr<po::options_description> hidden_;
  std::unique_ptr<po::variables_map> var_map_;
};

}  // namespace osoa
#endif  // SERVICE_ARGS_H_
