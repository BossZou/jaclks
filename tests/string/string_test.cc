#include "string/string.h"

#include <gtest/gtest.h>

namespace jaclks {

TEST(String, EmptyString) {
  String str{};

  ASSERT_EQ(str.Capacity(), 16UL);
  ASSERT_EQ(str.Length(), 0UL);
}

TEST(String, FromCharStr) {
  auto cstr = "1234567890";
  String str{cstr};

  ASSERT_EQ(str.Capacity(), 16UL);
  ASSERT_EQ(str.Length(), strlen(cstr));
  ASSERT_NE(reinterpret_cast<std::size_t>(str.CStr()),
            reinterpret_cast<std::size_t>(cstr));
}

TEST(String, FromLongCharStr) {
  auto cstr = "123456789098765432101234567890987654321";
  String str{cstr};

  ASSERT_EQ(str.Capacity(), strlen(cstr) + 1);
  ASSERT_EQ(str.Length(), strlen(cstr));
  ASSERT_NE(reinterpret_cast<std::size_t>(str.CStr()),
            reinterpret_cast<std::size_t>(cstr));
}

TEST(String, RefCharStr) {
  auto cstr = "1234567890";
  String str{cstr, true};

  ASSERT_EQ(str.Capacity(), strlen(cstr) + 1);
  ASSERT_EQ(str.Length(), strlen(cstr));
  ASSERT_EQ(reinterpret_cast<std::size_t>(str.CStr()),
            reinterpret_cast<std::size_t>(cstr));
}

TEST(String, StartWith) {
  {
    String str{"1234", true};
    ASSERT_TRUE(str.StartsWith(String{"123", true}));
  }
  {
    String str{"1234", true};
    ASSERT_FALSE(str.StartsWith(String{"234", true}));
  }
  {
    String str{"1234", true};
    ASSERT_TRUE(str.StartsWith(String{"1234", true}));
  }
  {
    String str{"1234", true};
    ASSERT_FALSE(str.StartsWith(String{"12345", true}));
  }
  {
    String str{"1234", true};
    ASSERT_TRUE(str.StartsWith(String{"", true}));
  }
}

TEST(String, StartWithOffset) {
  {
    String str{"1234", true};
    ASSERT_TRUE(str.StartsWith(String{"123", true}, 0));
  }
  {
    String str{"1234", true};
    ASSERT_TRUE(str.StartsWith(String{"234", true}, 1));
  }
  {
    String str{"1234", true};
    ASSERT_TRUE(str.StartsWith(String{"4", true}, 3));
  }
  {
    String str{"1234", true};
    ASSERT_TRUE(str.StartsWith(String{"", true}, 4));
  }
  {
    String str{"1234", true};
    ASSERT_FALSE(str.StartsWith(String{"", true}, 5));
  }
}

}  // namespace jaclks
