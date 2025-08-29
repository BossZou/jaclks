#include "jaclks/javac-base/java/security/message_digest.h"

#include "jaclks-internal/javac-base/java/security/message_digest_spi.h"

namespace jaclks::javac_base {

// class MessageDigest::MessageDigestSpi {
// public:
//   virtual ~MessageDigestSpi() = default;
//
//   virtual void EngineUpdate(char input) = 0;
// };
//
// class MD5Digest : public MessageDigest::MessageDigestSpi {
// public:
//   ~MD5Digest() override = default;
//
//   void EngineUpdate(char input) override {}
// };

const int MessageDigest::kInitial = 0;
const int MessageDigest::kInProgress = 1;

void MessageDigest::Update(char input) {
  digest_->EngineUpdate(input);
}

}  // namespace jaclks::javac_base
