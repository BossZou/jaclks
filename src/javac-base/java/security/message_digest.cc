#include "jaclks/javac-base/java/security/message_digest.h"

namespace javac::javac_base {

const int MessageDigest::kInitial = 0;
const int MessageDigest::kInProgress = 1;

class MessageDigest::MessageDigestSpi {};

void MessageDigest::Update(char input) {}

}  // namespace javac::javac_base
