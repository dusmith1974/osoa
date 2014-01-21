// Copyright 2013 Duncan Smith

#include "service/logging.h"

#include <iostream>
#include <string>

#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/log/attributes/attribute.hpp"
#include "boost/log/attributes/attribute_cast.hpp"
#include "boost/log/attributes/attribute_value.hpp"
#include "boost/log/attributes/current_process_name.hpp"
#include "boost/log/attributes/current_thread_id.hpp"
#include "boost/log/core.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/sources/global_logger_storage.hpp"
#include "boost/log/sources/logger.hpp"
#include "boost/log/sources/record_ostream.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/log/utility/setup/console.hpp"
#include "boost/log/utility/setup/file.hpp"
#include "boost/move/utility.hpp"

#include "service/args.h"

namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;
namespace fs = boost::filesystem;

namespace osoa {

namespace bl = boost::log;
namespace blt = boost::log::trivial;

Logging::Logging() :
  svc_logger_(new src::severity_logger_mt<blt::severity_level>()) {}

Logging::~Logging() {
}

int Logging::Initialize(const Args& args) {
  bl::add_common_attributes();

  bl::register_simple_formatter_factory<blt::severity_level, char>("Severity");

  fs::path full_path(args.module_path());
  fs::path leaf(full_path.filename().string());

  if (!args.log_dir().empty()) {
    full_path = args.log_dir();
    full_path = full_path / leaf;
  }

  if (!args.no_log_file())
    add_file_log(
      bl::keywords::file_name = full_path.string()
        + "_%Y-%m-%d_%H-%M-%S.%N.log",
      bl::keywords::rotation_size = args.rotation_size() * 1024 * 1024,
      bl::keywords::time_based_rotation =
        bl::sinks::file::rotation_at_time_point(0, 0, 0),
      bl::keywords::format =
        "[%TimeStamp%] [%Process%] [%Severity%] [%ThreadID%]: %Message%",
      bl::keywords::filter =
        expr::attr<blt::severity_level>("Severity") >= blt::debug);

  bl::add_console_log()->set_filter(
    blt::severity >= ((args.verbose()) ? blt::debug : blt::info));

  bl::core::get()->add_global_attribute(
    "ThreadID", attrs::current_thread_id());
  bl::core::get()->add_global_attribute(
    "Process", attrs::current_process_name());

  WriteLogHeader(args);

  return 0;
}

int Logging::WriteLogHeader(const Args& args) {
  BOOST_LOG_SEV(svc_logger(), blt::info) << "Rotation size: " 
    << args.rotation_size();

  return 0;  
}
}  // namespace osoa
