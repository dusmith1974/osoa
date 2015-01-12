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

#include "osoa_pch.h"  // NOLINT

#include "service/service.h"

#include "boost/log/core.hpp"

#include "service/args.h"
#include "service/comms/comms.h"
#include "service/logging.h"
#include "util/timing.h"

namespace osoa {
Service::Service()
  : abort_signal_(0),
    svc_start_time_(boost::chrono::steady_clock::now()),
    svc_end_time_(boost::chrono::steady_clock::now()),
    args_(new Args()),
    comms_(std::make_shared<Comms>()),
    publishing_(false) {
}

Service::~Service() {}

// Parses the command line options (and config file) and sets up the logging
// front ends and back ends.
Error Service::Initialize(int argc, const char* argv[]) {
  Error code = args()->Initialize(argc, argv);
  if (Error::kSuccess != code) return code;

  if (!args()->listening_port().empty())
    set_publishing(true);

  code = Logging::Instance().Initialize(args());
  if (Error::kSuccess != code) return code;

  code = comms()->Initialize(args()->listening_port());
  if (Error::kSuccess != code) return code;

  return Error::kSuccess;
}

// Starts this service, sets the start time and opens the main listening port
// to accept connections.
// Uses the non-virtual interface idiom to ensure certain pre-start tasks are
// always carried out (publishing the channel, socket and websocket threads)
// before any non-blocking service specific tasks are added in the subclass. We
// then return to here and wait for SIGINT as a post start task.
Error Service::Start() {
  // Publish the channel.
  set_svc_start_time(boost::chrono::steady_clock::now());
  BOOST_LOG_SEV(*Logging::logger(), blt::info)
      << "Starting the service.";

  Error code = Error::kSuccess;
  if (args()->var_map().count("listening-port"))
    code = comms()->PublishChannel();

  if (Error::kSuccess == code)
    code = DoStart();

  // Post Start
  // wait for CTRL-C or kill
  boost::asio::io_service io_service;
  boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
  signals.async_wait(boost::bind(&Service::AbortHandler, this, _1, _2));
  io_service.run();

  std::string abort_reason;
  abort_reason = (abort_signal_ == SIGINT) ? "Ctrl-C" : "SIGTERM";
  BOOST_LOG_SEV(*Logging::logger(), blt::info)
      << std::endl << abort_reason << " received.";

  return code;
}

// Override in the subclass to queue up all of the work for the service. Note
// that all work and io_services should be spun off into threads so we don't
// block. We want to return to Start() and wait for SIGINT to close cleanly.
Error Service::DoStart() {
  // Default service behavior - do nothing.
  return Error::kSuccess;
}

// Stops the service, logs the service uptime and detaches any logging threads
// or front-ends from the core. Uses the non-virtual interface idiom to ensure
// that we always call Shutdown on the comms to stop work and join threads,
// before any service specific clean up in the subclass. Finally we detach
// logging as a post stop task.
Error Service::Stop() {
  BOOST_LOG_SEV(*Logging::logger(), blt::info)
      << "Stopping the service.";

  comms()->Shutdown();

  set_svc_end_time(boost::chrono::steady_clock::now());
  BOOST_LOG_SEV(*Logging::logger(), blt::info) << "Service uptime: "
      << add_timestamp(std::make_pair(svc_start_time(), svc_end_time()));

  Error code = DoStop();

  BOOST_LOG_SEV(*Logging::logger(), blt::debug)
      << "Detach logging.";
  Logging::Instance().Detach();

  return code;
}

Error Service::DoStop() {
  // Default service behavior - do nothing.
  return Error::kSuccess;
}

void Service::AbortHandler(const boost::system::error_code& error,
                           int signal_number) {
  if (!error) {
    abort_signal_ = signal_number;
  }
}

std::shared_ptr<Args> Service::args() { return args_; }
std::shared_ptr<Comms> Service::comms() { return comms_; }

const Service::Timepoint& Service::svc_start_time() const {
  return svc_start_time_;
}

void Service::set_svc_start_time(const Service::Timepoint& val) {
  svc_start_time_ = val;
}

const Service::Timepoint& Service::svc_end_time() const {
  return svc_end_time_;
}

void Service::set_svc_end_time(const Service::Timepoint& val) {
  svc_end_time_ = val;
}

bool Service::publishing() const {
  return publishing_;
}

void Service::set_publishing(bool val) {
  publishing_ = val;
}
}  // namespace osoa
