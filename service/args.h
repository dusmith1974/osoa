// Copyright 2013 Duncan Smith

#ifndef SERVICE_ARGS_H_
#define SERVICE_ARGS_H_

#include <memory>

#include <boost/noncopyable.hpp>  // NOLINT
#include <boost/program_options.hpp> // NOLINT


namespace osoa {

namespace po = boost::program_options;

class Args : boost::noncopyable {
 public:
  Args();
  void Initialize(int argc, const char* argv[]);

 private:
  std::unique_ptr<po::options_description> usage_;
  po::options_description& usage() { return *(usage_.get()); }

  std::unique_ptr<po::variables_map> var_map_;
  po::variables_map& var_map() { return *(var_map_.get()); }
};

}  // namespace osoa
#endif  // SERVICE_ARGS_H_
