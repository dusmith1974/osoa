// Copyright 2013 Duncan Smith

#ifndef SERVICE_ARGS_H_
#define SERVICE_ARGS_H_

#include <memory>
#include <string>

#include "boost/noncopyable.hpp"
#include "boost/program_options.hpp"

namespace osoa {

namespace po = boost::program_options;

class Args : boost::noncopyable {
 public:
  Args();
  virtual ~Args();

  int Initialize(int argc, const char* argv[]);

  const std::string& config_file() const { return config_file_; }
  const std::string& log_dir() const { return log_dir_; }
  const std::string& module_path() const { return module_path_; }
  bool no_log_file() const { return no_log_file_; }
  bool async_log() const { return async_log_; }
  bool auto_flush_log() const { return auto_flush_log_; }
  int rotation_size() const { return rotation_size_; }

  bool verbose() const { return verbose_; }
  bool silent() const { return silent_; }

  po::options_description& config() { return *(config_.get()); }
  po::variables_map& var_map() { return *(var_map_.get()); }

 private:
  std::unique_ptr<po::options_description> generic_;
  po::options_description& generic() { return *(generic_.get()); }

  std::unique_ptr<po::options_description> config_;

  std::unique_ptr<po::options_description> hidden_;
  po::options_description& hidden() { return *(hidden_.get()); }

  std::unique_ptr<po::variables_map> var_map_;

  int version_major_no() { return version_major_no_; }
  int version_minor_no() { return version_minor_no_; }

  std::string Version();

  std::string log_dir_;

  std::string module_path_;
  void set_module_path(const std::string& val) { module_path_ = val; }

  bool no_log_file_;
  void set_no_log_file(bool val) { no_log_file_ = val; }

  std::string config_file_;

  bool async_log_;
  void set_async_log(bool val) { async_log_ = val; }

  bool auto_flush_log_;
  void set_auto_flush_log(bool val) { auto_flush_log_ = val; }

  int rotation_size_;

  static const int version_major_no_;
  static const int version_minor_no_;

  bool verbose_;
  void set_verbose(bool val) { verbose_ = val; }

  bool silent_;
  void set_silent(bool val) { silent_ = val; }
};

}  // namespace osoa
#endif  // SERVICE_ARGS_H_
