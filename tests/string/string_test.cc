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

}  // namespace jaclks
