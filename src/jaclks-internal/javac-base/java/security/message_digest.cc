#include "jaclks/javac-base/java/security/message_digest.h"

#include <memory>
#include <utility>

#include "jaclks-internal/javac-base/java/security/md5_digest.h"
#include "jaclks-internal/javac-base/java/security/message_digest_spi.h"

namespace jaclks::javac_base {

const int MessageDigest::kInitial = 0;
const int MessageDigest::kInProgress = 1;

MessageDigest MessageDigest::GetInstance(String algorithm) {
  std::unique_ptr<MessageDigestSpi> digest;

  if (algorithm == String{"MD5", true}) {
    digest = std::make_unique<MD5Digest>();
  }

  return {std::move(algorithm), std::move(digest)};
}

MessageDigest::MessageDigest(String algorithm,
                             std::unique_ptr<MessageDigestSpi> digest)
    : state_(kInitial),
      algorithm_(std::move(algorithm)),
      digest_(std::move(digest)) {}  // NOLINT(whitespace/indent_namespace)

MessageDigest::~MessageDigest() {
  digest_.reset();
}

void MessageDigest::Update(const std::uint8_t *data, std::size_t num) {
  digest_->EngineUpdate(data, num);
  state_ = kInProgress;
}

String MessageDigest::Digest() {
  auto digest = digest_->EngineDigest();
  state_ = kInitial;
  return digest;
}

void MessageDigest::Reset() {
  digest_->EngineReset();
  state_ = kInitial;
}

}  // namespace jaclks::javac_base
