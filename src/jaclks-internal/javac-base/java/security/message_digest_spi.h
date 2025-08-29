#pragma once

#include "jaclks/javac-base/java/security/message_digest.h"

namespace jaclks::javac_base {

class MessageDigestSpi {
 public:
  virtual ~MessageDigestSpi() = default;

  virtual void EngineUpdate(char input) = 0;
};

}  // namespace jaclks::javac_base
