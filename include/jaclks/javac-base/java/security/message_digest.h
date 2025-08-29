#pragma once

#include <memory>

#include "jaclks/javac-base/lang/string.h"

namespace jaclks::javac_base {

class MessageDigestSpi;

class MessageDigest {
 public:
  void Update(char input);

 private:
  static const int kInitial;
  static const int kInProgress;

  int state_;
  String algorithm_;

  std::unique_ptr<MessageDigestSpi> digest_;
};

}  // namespace jaclks::javac_base
