#pragma once

#include "jaclks/javac-base/java/security/message_digest.h"

namespace jaclks::javac_base {

class MessageDigestSpi {
 public:
  virtual ~MessageDigestSpi() = default;

  virtual void EngineUpdate(const char *data, std::size_t num) = 0;

  virtual String EngineDigest() = 0;
};

}  // namespace jaclks::javac_base
