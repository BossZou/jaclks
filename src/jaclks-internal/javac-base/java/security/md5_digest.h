#pragma once

#include "jaclks-internal/javac-base/java/security/message_digest_spi.h"

namespace jaclks::javac_base {

class MD5Digest : public MessageDigestSpi {
 public:
  MD5Digest();

  ~MD5Digest() override = default;

  void EngineUpdate(const char *data, std::size_t num) override;

  String EngineDigest() override;

  void EngineReset() override;

 private:
  static constexpr int kMD5StateNum = 4;
  static constexpr int kMD5BlockSize = 64;
  static constexpr int kMD5DigestLength = 16;

  void md5_init();

  void md5_update(const std::uint8_t *data, std::size_t len);

  void md5_transform(const std::uint8_t *data, std::size_t len);

  String md5_final();

  std::uint32_t h_[kMD5StateNum];
  std::uint32_t Nl_, Nh_;
  std::uint8_t data_[kMD5BlockSize];
  unsigned num_;
};

}  // namespace jaclks::javac_base
