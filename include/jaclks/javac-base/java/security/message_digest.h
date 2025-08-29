#pragma once

#include <memory>

#include "jaclks/javac-base/lang/string.h"

namespace jaclks::javac_base {

class MessageDigestSpi;

class MessageDigest {
 public:
  static MessageDigest GetInstance(String algorithm);

  void Update(const char *data, std::size_t num);

 private:
  static const int kInitial;
  static const int kInProgress;

  MessageDigest(String algorithm, std::unique_ptr<MessageDigestSpi> digest);

  int state_;
  String algorithm_;

  std::unique_ptr<MessageDigestSpi> digest_;
};

}  // namespace jaclks::javac_base
