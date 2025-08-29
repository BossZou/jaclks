#pragma once

#include "jaclks-internal/javac-base/java/security/message_digest_spi.h"

namespace jaclks::javac_base {

class MD5Digest : public MessageDigestSpi {
 public:
  ~MD5Digest() override = default;

  void EngineUpdate(char input) override;
};

}  // namespace jaclks::javac_base
