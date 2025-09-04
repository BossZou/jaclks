#pragma once

#include <openssl/opensslv.h>  // used for openssl version
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#define USE_EVP_MD5  // OpenSSL 3.0+ Use EVP API
#include <openssl/evp.h>
#else
#include <openssl/md5.h>
#endif

#include "jaclks-internal/javac-base/java/security/message_digest_spi.h"

namespace jaclks::javac_base {

class MD5Digest : public MessageDigestSpi {
 public:
  MD5Digest();

  ~MD5Digest() override;

  void EngineUpdate(const std::uint8_t *data, std::size_t num) override;

  String EngineDigest() override;

  void EngineReset() override;

 private:
  void md5_init();

  void md5_update(const std::uint8_t *data, std::size_t len);

  String md5_final();

#ifdef USE_EVP_MD5
  EVP_MD_CTX *ctx_;
#else
  MD5_CTX ctx_;
#endif
};

}  // namespace jaclks::javac_base
