// Copyright 2013 Duncan Smith

#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <vector>

#include "boost/noncopyable.hpp"

#include "service/service.h"

namespace osoa {

class Test final : public Service, private boost::noncopyable {
 public:
  typedef Service super;

  Test();
  virtual ~Test();

  int Initialize(int argc, const char *argv[]) override;

  int Start() override;
  int Stop() override;

  const std::vector<std::string> subscriptions() const { return subscriptions_; } 
 private:
  int msg_count_;
  int msg_count() { return msg_count_; }

  std::vector<std::string> subscriptions_; 
};

}  // namespace osoa
#endif  // TEST_TEST_H_

