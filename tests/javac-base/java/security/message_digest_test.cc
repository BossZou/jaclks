#include "jaclks/javac-base/java/security/message_digest.h"

#include <gtest/gtest.h>

#include "jaclks/javac-base/java/lang/system.h"

namespace jaclks::javac_base {

namespace {

class MessageDigestTest : public ::testing::Test {
 protected:
  static void RandomBuffer(char *buf, std::size_t len) {
    for (auto i = 0UL; i < len - 8; i += 8) {
      *reinterpret_cast<std::int64_t *>(buf + i) = System::CurrentTimeMillis();
    }
  }
};

}  // namespace

TEST_F(MessageDigestTest, MD5) {
  auto md5 = MessageDigest::GetInstance("MD5");

  md5.Update(reinterpret_cast<const std::uint8_t *>("1234567890"), 10);
  auto result = md5.Digest();
  ASSERT_EQ(16UL, result.Length());
}

TEST_F(MessageDigestTest, MD5LargeBuffer) {
  std::vector<std::size_t> sizes{
      4096, 99999, 128 * 1024, 1024 * 1024, 99999999};
  auto md5 = MessageDigest::GetInstance("MD5");

  for (const auto size : sizes) {
    auto buffer = std::make_unique<char[]>(size);
    RandomBuffer(buffer.get(), size);

    md5.Update(reinterpret_cast<const std::uint8_t *>(buffer.get()), size);
    auto result = md5.Digest();
    ASSERT_EQ(16UL, result.Length());

    md5.Reset();
  }
}

TEST_F(MessageDigestTest, MD5UpdatePartial) {
  auto size = 99999999UL;
  auto buffer = std::make_unique<char[]>(size);
  RandomBuffer(buffer.get(), size);

  String res1;
  {
    auto md5 = MessageDigest::GetInstance("MD5");
    md5.Update(reinterpret_cast<const std::uint8_t *>(buffer.get()), size);

    res1 = md5.Digest();
    ASSERT_EQ(16UL, res1.Length());
  }
  String res2;
  {
    auto md5 = MessageDigest::GetInstance("MD5");
    for (auto i = 0UL; i < size; i += 9999) {
      md5.Update(reinterpret_cast<const std::uint8_t *>(buffer.get() + i),
                 std::min(9999UL, size - i));
    }

    res2 = md5.Digest();
    ASSERT_EQ(16UL, res2.Length());
  }

  ASSERT_EQ(res1, res2) << "r1:" << res1.CStr() << ", r2:" << res2.CStr();
}

}  // namespace jaclks::javac_base
