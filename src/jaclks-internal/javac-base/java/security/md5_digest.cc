#include "jaclks-internal/javac-base/java/security/md5_digest.h"

namespace jaclks::javac_base {

MD5Digest::MD5Digest() : ctx_{} {
  md5_init();
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
  MD5_Init(&ctx_);
}

void MD5Digest::md5_update(const std::uint8_t *data, std::size_t len) {
  MD5_Update(&ctx_, data, len);
}

String MD5Digest::md5_final() {
  unsigned char digest[MD5_DIGEST_LENGTH];

  MD5_Final(digest, &ctx_);
  return String{reinterpret_cast<const char *>(digest),
                static_cast<std::size_t>(kMD5DigestLength)};
}

}  // namespace jaclks::javac_base
