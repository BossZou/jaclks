#pragma once

#include "jaclks-internal/javac-base/java/security/message_digest_spi.h"

namespace jaclks::javac_base {

struct MD5Ctx {
  static constexpr int kMD5StateNum = 4;
  static constexpr int kMD5BlockSize = 64;

  std::uint32_t h[kMD5StateNum];
  std::uint32_t Nl, Nh;
  std::uint8_t data[kMD5BlockSize];
  unsigned num;
};

class MD5Digest : public MessageDigestSpi {
 public:
  MD5Digest();

  ~MD5Digest() override = default;

  void EngineUpdate(char input) override;

 private:
  static constexpr int kMD5DigestLength = 16;

  int init();

  int update(const std::uint8_t *data, std::size_t len);

  void transform(const std::uint8_t *data, std::size_t len);

  void final();

  MD5Ctx md5_;
};

}  // namespace jaclks::javac_base
