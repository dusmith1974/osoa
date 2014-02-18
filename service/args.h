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
  ~Args();

  Error Initialize(int argc, const char* argv[]);

  // Generic options.
  const std::string& config_file() const;

  // Configuration options.
  const std::string& log_dir() const;
  bool no_log_file() const;
  bool verbose() const;
  bool silent() const;

  const std::string listening_port() const;

  const std::vector<std::string> services() const;

  // Hidden options.
  bool async_log() const;
  bool auto_flush_log() const;
  int rotation_size() const;

  // program_options objects.
  po::options_description& config();
  po::variables_map& var_map();

  // Path to this app.
  const std::string& module_path() const;

 private:
  static const int version_major_no_;
  static const int version_minor_no_;

  const std::string Version() const;

  int version_major_no() const;
  int version_minor_no() const;

  void set_module_path(const std::string& val);

  // Configuration options.
  void set_no_log_file(bool val);
  void set_verbose(bool val);
  void set_silent(bool val);

  // Hidden options.
  void set_async_log(bool val);
  void set_auto_flush_log(bool val);

  po::options_description& generic();
  po::options_description& hidden();

  std::string module_path_;

  // Generic options.
  std::string config_file_;

  // Configuration options.
  std::string log_dir_;
  bool no_log_file_;
  bool verbose_;
  bool silent_;
  std::string listening_port_;
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
