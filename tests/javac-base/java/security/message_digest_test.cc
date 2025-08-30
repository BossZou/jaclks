#include "jaclks/javac-base/java/security/message_digest.h"

#include <gtest/gtest.h>

namespace jaclks::javac_base {

TEST(MessageDigestTest, MD5) {
  auto md5 = MessageDigest::GetInstance("MD5");

  md5.Update(reinterpret_cast<const std::uint8_t *>("1234567890"), 10);
  auto result = md5.Digest();
  ASSERT_EQ(16UL, result.Length());
}

}  // namespace jaclks::javac_base
