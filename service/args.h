// Copyright 2013 Duncan Smith

#ifndef SERVICE_ARGS_H_
#define SERVICE_ARGS_H_

#include <memory>
#include <string>

#include <boost/noncopyable.hpp>  // NOLINT
#include <boost/program_options.hpp> // NOLINT

namespace osoa {

namespace po = boost::program_options;

class Args  : boost::noncopyable {
 public:
  Args();
  void Initialize(int argc, const char* argv[]);

public:
  const std::string& log_dir() { return log_dir_; }
  bool verbose() { return verbose_; }

 private:
  std::unique_ptr<po::options_description> generic_;
  po::options_description& generic() { return *(generic_.get()); }

  std::unique_ptr<po::options_description> config_;
  po::options_description& config() { return *(config_.get()); }

  std::unique_ptr<po::options_description> hidden_;
  po::options_description& hidden() { return *(hidden_.get()); }

  std::unique_ptr<po::variables_map> var_map_;
  po::variables_map& var_map() { return *(var_map_.get()); }

  int version_major_no() { return version_major_no_; } 
  int version_minor_no() { return version_minor_no_; } 

  std::string Version();

  std::string log_dir_;

  std::string config_file_;
  const std::string& config_file() { return config_file_; }

  static const int version_major_no_;
  static const int version_minor_no_;

  bool verbose_;
  void set_verbose(bool val) { verbose_ = val; }
};

}  // namespace osoa
#endif  // SERVICE_ARGS_H_
