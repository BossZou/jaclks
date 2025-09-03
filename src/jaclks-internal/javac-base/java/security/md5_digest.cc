#include "jaclks-internal/javac-base/java/security/md5_digest.h"

namespace jaclks::javac_base {

MD5Digest::MD5Digest() : ctx_{} {
  md5_init();
}

MD5Digest::~MD5Digest() {
#ifdef USE_EVP_MD5
  EVP_MD_CTX_free(ctx_);
  ctx_ = nullptr;
#endif
}

void MD5Digest::EngineUpdate(const std::uint8_t *data, std::size_t num) {
  md5_update(data, num);
}

String MD5Digest::EngineDigest() {
  return md5_final();
}

void MD5Digest::EngineReset() {
  this->~MD5Digest();
  new (this) MD5Digest();
}

void MD5Digest::md5_init() {
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
  ctx_evp_ = EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx_evp_, EVP_md5(), nullptr);
#else
  MD5_Init(&ctx_);
#endif
}

void MD5Digest::md5_update(const std::uint8_t *data, std::size_t len) {
#ifdef USE_EVP_MD5
  EVP_DigestUpdate(ctx_, data, len);
#else
  MD5_Update(&ctx_, data, len);
#endif
}

String MD5Digest::md5_final() {
  unsigned char digest[MD5_DIGEST_LENGTH];

#ifdef USE_EVP_MD5
  unsigned int len;
  EVP_DigestFinal_ex(ctx__, digest, &len);
#else
  MD5_Final(digest, &ctx_);
#endif
  return String{reinterpret_cast<const char *>(digest),
                static_cast<std::size_t>(MD5_DIGEST_LENGTH)};
}

}  // namespace jaclks::javac_base
