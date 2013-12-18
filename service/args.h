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
  Args(int argc, char* argv);

 private:
  std::unique_ptr<po::options_description> usage_;
};

}  // namespace osoa
#endif  // SERVICE_ARGS_H_
