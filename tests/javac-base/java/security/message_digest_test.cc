#include "jaclks/javac-base/java/security/message_digest.h"

#include <gtest/gtest.h>

namespace jaclks::javac_base {

TEST(MessageDigestTest, MD5) {
  auto md5 = MessageDigest::GetInstance("MD5");

  md5.Update("1234567890", 10);
}

}
