#include "lang/string.h"

#include <gtest/gtest.h>

namespace jaclks {

namespace {

class StringTest : public ::testing::TestWithParam<bool> {
 protected:
  // Sets up the test fixture.
  void SetUp() override {}

  // Tears down the test fixture.
  void TearDown() override {}
};

INSTANTIATE_TEST_SUITE_P(Reference, StringTest, ::testing::Values(false, true));

}  // namespace

TEST_P(StringTest, EmptyString) {
  String str{};

  ASSERT_EQ(str.Capacity(), 16UL);
  ASSERT_EQ(str.Length(), 0UL);
}

TEST_P(StringTest, FromCharStr) {
  auto cstr = "1234567890";
  String str{cstr, GetParam()};

  ASSERT_EQ(str.Length(), strlen(cstr));
  if (str.IsRef()) {
    ASSERT_EQ(str.Capacity(), 11UL);
    ASSERT_EQ(reinterpret_cast<std::size_t>(str.CStr()),
              reinterpret_cast<std::size_t>(cstr));
  } else {
    ASSERT_EQ(str.Capacity(), 16UL);
    ASSERT_NE(reinterpret_cast<std::size_t>(str.CStr()),
              reinterpret_cast<std::size_t>(cstr));
  }
}

TEST_P(StringTest, FromLongCharStr) {
  auto cstr = "123456789098765432101234567890987654321";
  String str{cstr, GetParam()};

  ASSERT_EQ(str.Capacity(), strlen(cstr) + 1);
  ASSERT_EQ(str.Length(), strlen(cstr));
  if (str.IsRef()) {
    ASSERT_EQ(reinterpret_cast<std::size_t>(str.CStr()),
              reinterpret_cast<std::size_t>(cstr));
  } else {
    ASSERT_NE(reinterpret_cast<std::size_t>(str.CStr()),
              reinterpret_cast<std::size_t>(cstr));
  }
}

TEST_P(StringTest, StartsWith) {
  {
    String str{"1234", GetParam()};
    ASSERT_TRUE(str.StartsWith(String{"123", GetParam()}));
  }
  {
    String str{"1234", GetParam()};
    ASSERT_FALSE(str.StartsWith(String{"234", GetParam()}));
  }
  {
    String str{"1234", GetParam()};
    ASSERT_TRUE(str.StartsWith(String{"1234", GetParam()}));
  }
  {
    String str{"1234", GetParam()};
    ASSERT_FALSE(str.StartsWith(String{"12345", GetParam()}));
  }
  {
    String str{"1234", GetParam()};
    ASSERT_TRUE(str.StartsWith(String{"", GetParam()}));
  }
}

TEST_P(StringTest, StartsWithOffset) {
  {
    String str{"1234", GetParam()};
    ASSERT_TRUE(str.StartsWith(String{"123", GetParam()}, 0));
  }
  {
    String str{"1234", GetParam()};
    ASSERT_TRUE(str.StartsWith(String{"234", GetParam()}, 1));
  }
  {
    String str{"1234", GetParam()};
    ASSERT_TRUE(str.StartsWith(String{"4", GetParam()}, 3));
  }
  {
    String str{"1234", GetParam()};
    ASSERT_TRUE(str.StartsWith(String{"", GetParam()}, 4));
  }
  {
    String str{"1234", GetParam()};
    ASSERT_FALSE(str.StartsWith(String{"", GetParam()}, 5));
  }
}

TEST_P(StringTest, EndsWith) {
  auto cstr = "123456789098765432101234567890987654321";
  String str{cstr, GetParam()};

  ASSERT_TRUE(str.EndsWith(String{"54321", GetParam()}));
  ASSERT_FALSE(str.EndsWith(String{"554321", GetParam()}));
  ASSERT_TRUE(str.EndsWith({}));
  ASSERT_TRUE(str.EndsWith(String{cstr, GetParam()}));
  ASSERT_FALSE((String{"12345678909", GetParam()}).EndsWith(str));
}

TEST_P(StringTest, StaticStripTrailing) {
  {
    auto cstr = "0123456789 \t\r\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::StripTrailing(str);
    ASSERT_EQ(strip_str, (String{"0123456789", GetParam()}));
  }
  {
    auto cstr = "0123456789 \t\raaa\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::StripTrailing(str);
    ASSERT_EQ(strip_str, (String{"0123456789 \t\raaa", GetParam()}));
  }
}

TEST_P(StringTest, StripTrailing) {
  {
    auto cstr = "0123456789 \t\r\n\f\v";
    String str{cstr, GetParam()};

    str.StripTrailing();
    ASSERT_EQ(str, (String{"0123456789", GetParam()}));
  }
  {
    auto cstr = "0123456789 \t\raaa\n\f\v";
    String str{cstr, GetParam()};

    str.StripTrailing();
    ASSERT_EQ(str, (String{"0123456789 \t\raaa", GetParam()}));
  }
}

TEST_P(StringTest, Reset) {
  auto cstr = "123456789098765432101234567890987654321";
  String str{cstr, GetParam()};

  str.Reset();
  ASSERT_EQ(str.Length(), 0UL);
  ASSERT_EQ(str.Capacity(), 16UL);
}

}  // namespace jaclks
