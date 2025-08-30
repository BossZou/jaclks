#pragma once

#include <memory>

#include "jaclks/javac-base/java/lang/string.h"

namespace jaclks::javac_base {

class MessageDigestSpi;

class MessageDigest {
 public:
  static MessageDigest GetInstance(String algorithm);

  void Update(const std::uint8_t *data, std::size_t num);

  String Digest();

  void Reset();

 private:
  static const int kInitial;
  static const int kInProgress;

  MessageDigest(String algorithm, std::unique_ptr<MessageDigestSpi> digest);

  int state_;
  String algorithm_;

  std::unique_ptr<MessageDigestSpi> digest_;
};

}  // namespace jaclks::javac_base
