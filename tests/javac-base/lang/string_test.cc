#include "javac-base/lang/string.h"

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

TEST_P(StringTest, CopyConstruct) {
  String str{"\n 1234567890\t\r", GetParam()};

  ASSERT_EQ(str, String{str});

  str.Strip();
  ASSERT_EQ(str, String{str});
}

TEST_P(StringTest, MoveConstruct) {
  String str{"\n 1234567890\t\r", GetParam()};

  {
    auto move_str = str;
    ASSERT_EQ(str, String{std::move(move_str)});
  }
  {
    str.Strip();
    auto move_str = str;
    ASSERT_EQ(str, String{std::move(move_str)});
  }
}

TEST_P(StringTest, CopyAssign) {
  String str{"\n 1234567890\t\r", GetParam()};

  {
    String str2{"123", GetParam()};
    ASSERT_EQ(3UL, str2.Length());

    str2 = str;
    ASSERT_EQ(str.Length(), str2.Length());
    ASSERT_EQ(str, str2);
  }
  {
    String str2{"0123456789012345678901234567890123456789", GetParam()};
    ASSERT_EQ(40UL, str2.Length());

    str2 = str;
    ASSERT_EQ(str.Length(), str2.Length());
    ASSERT_EQ(str, str2);
  }

  {
    str.Strip();

    String str2{"123", GetParam()};
    ASSERT_EQ(3UL, str2.Length());

    str2 = str;
    ASSERT_EQ(str.Length(), str2.Length());
    ASSERT_EQ(str, str2);
  }
}

TEST_P(StringTest, MoveAssignment) {
  String str{"\n 1234567890\t\r", GetParam()};

  {
    String to_move = str;

    String str2{"123", GetParam()};
    ASSERT_EQ(3UL, str2.Length());

    str2 = std::move(to_move);
    ASSERT_EQ(str.Length(), str2.Length());
    ASSERT_EQ(str, str2);
  }
  {
    String to_move = str;

    String str2{"0123456789012345678901234567890123456789", GetParam()};
    ASSERT_EQ(40UL, str2.Length());

    str2 = std::move(to_move);
    ASSERT_EQ(str.Length(), str2.Length());
    ASSERT_EQ(str, str2);
  }

  {
    str.Strip();
    String to_move = str;

    String str2{"123", GetParam()};
    ASSERT_EQ(3UL, str2.Length());

    str2 = std::move(to_move);
    ASSERT_EQ(str.Length(), str2.Length());
    ASSERT_EQ(str, str2);
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

TEST_P(StringTest, StaticTrim) {
  {
    char bytes[]{'\0', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};
    for (char c = 0; c < ' '; ++c) {
      bytes[0] = c;
      String str{bytes, sizeof(bytes) - 1};

      auto trim_str = String::Trim(str);
      ASSERT_EQ(sizeof(bytes) - 2, trim_str.Length());
      ASSERT_EQ((String{"0123456789", true}), trim_str);
    }
  }
  {
    char bytes[]{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0', '\0'};
    for (char c = 0; c < ' '; ++c) {
      bytes[sizeof(bytes) - 2] = c;
      String str{bytes, sizeof(bytes) - 1};

      auto trim_str = String::Trim(str);
      ASSERT_EQ(sizeof(bytes) - 2, trim_str.Length());
      ASSERT_EQ((String{"0123456789", true}), trim_str);
    }
  }
  {
    auto cstr = " \t\r\n\f\v0123456789987654321 \v\f\r\n\t";
    String str{cstr, GetParam()};

    auto trim_str = String::Trim(str);
    ASSERT_EQ(trim_str, (String{"0123456789987654321", GetParam()}));
    ASSERT_EQ(str.Length() - 12, trim_str.Length());
  }
}

TEST_P(StringTest, StaticStrip) {
  {
    auto cstr = " \t\r\n\f\v0123456789";
    String str{cstr, GetParam()};

    auto strip_str = String::Strip(str);
    ASSERT_EQ(strip_str, (String{"0123456789", GetParam()}));
    ASSERT_EQ(str.Length() - 6, strip_str.Length());
  }
  {
    auto cstr = "0123456789 \t\r\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::Strip(str);
    ASSERT_EQ(strip_str, (String{"0123456789", GetParam()}));
    ASSERT_EQ(str.Length() - 6, strip_str.Length());
  }
  {
    auto cstr = " \t\raaa\n\f\v0123456789";
    String str{cstr, GetParam()};

    auto strip_str = String::Strip(str);
    ASSERT_EQ(strip_str, (String{"aaa\n\f\v0123456789", GetParam()}));
    ASSERT_EQ(str.Length() - 3, strip_str.Length());
  }
  {
    auto cstr = "0123456789 \t\raaa\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::Strip(str);
    ASSERT_EQ(strip_str, (String{"0123456789 \t\raaa", GetParam()}));
    ASSERT_EQ(str.Length() - 3, strip_str.Length());
  }
  {
    auto cstr = " \t\raaa0123456789\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::Strip(str);
    ASSERT_EQ(strip_str, (String{"aaa0123456789", GetParam()}));
    ASSERT_EQ(str.Length() - 6, strip_str.Length());
  }
  {
    auto cstr = " \t\r012345678909876543210123456789\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::Strip(str);
    ASSERT_EQ(strip_str,
              (String{"012345678909876543210123456789", GetParam()}));
    ASSERT_EQ(str.Length() - 6, strip_str.Length());
  }
  {  // Case all char are white space
    auto cstr = "\t\ra\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::Strip(str);
    ASSERT_EQ(strip_str, (String{"a", GetParam()}));
    ASSERT_EQ(1UL, strip_str.Length());
  }
  {  // Case all char are white space
    auto cstr = "\t\r\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::Strip(str);
    ASSERT_EQ(strip_str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, strip_str.Length());
  }
  {
    String str{"", GetParam()};

    auto strip_str = String::Strip(str);
    ASSERT_EQ(strip_str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, strip_str.Length());
  }
}

TEST_P(StringTest, Strip) {
  {
    auto cstr = " \t\r\n\f\v0123456789";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.Strip();
    ASSERT_EQ(str, (String{"0123456789", GetParam()}));
    ASSERT_EQ(len - 6, str.Length());
  }
  {
    auto cstr = "0123456789 \t\r\n\f\v";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.Strip();
    ASSERT_EQ(str, (String{"0123456789", GetParam()}));
    ASSERT_EQ(len - 6, str.Length());
  }
  {
    auto cstr = " \t\raaa\n\f\v0123456789";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.Strip();
    ASSERT_EQ(str, (String{"aaa\n\f\v0123456789", GetParam()}));
    ASSERT_EQ(len - 3, str.Length());
  }
  {
    auto cstr = "0123456789 \t\raaa\n\f\v";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.Strip();
    ASSERT_EQ(str, (String{"0123456789 \t\raaa", GetParam()}));
    ASSERT_EQ(len - 3, str.Length());
  }
  {
    auto cstr = " \t\raaa0123456789\n\f\v";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.Strip();
    ASSERT_EQ(str, (String{"aaa0123456789", GetParam()}));
    ASSERT_EQ(len - 6, str.Length());
  }
  {
    auto cstr = " \t\r012345678909876543210123456789\n\f\v";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.Strip();
    ASSERT_EQ(str, (String{"012345678909876543210123456789", GetParam()}));
    ASSERT_EQ(len - 6, str.Length());
  }
  {
    auto cstr = "\t\ra\n\f\v";
    String str{cstr, GetParam()};

    str.Strip();
    ASSERT_EQ(str, (String{"a", GetParam()}));
    ASSERT_EQ(1UL, str.Length());
  }
  {  // Case all char are white space
    auto cstr = "\t\r\n\f\v";
    String str{cstr, GetParam()};

    str.Strip();
    ASSERT_EQ(str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, str.Length());
  }
  {
    String str{"", GetParam()};

    str.Strip();
    ASSERT_EQ(str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, str.Length());
  }
}

TEST_P(StringTest, StaticStripLeading) {
  {
    auto cstr = " \t\r\n\f\v0123456789";
    String str{cstr, GetParam()};

    auto strip_str = String::StripLeading(str);
    ASSERT_EQ(strip_str, (String{"0123456789", GetParam()}));
    ASSERT_EQ(str.Length() - 6, strip_str.Length());
  }
  {
    auto cstr = " \t\raaa\n\f\v0123456789";
    String str{cstr, GetParam()};

    auto strip_str = String::StripLeading(str);
    ASSERT_EQ(strip_str, (String{"aaa\n\f\v0123456789", GetParam()}));
    ASSERT_EQ(str.Length() - 3, strip_str.Length());
  }
  {
    auto cstr = " \t\r012345678909876543210123456789\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::StripLeading(str);
    ASSERT_EQ(strip_str,
              (String{"012345678909876543210123456789\n\f\v", GetParam()}));
    ASSERT_EQ(str.Length() - 3, strip_str.Length());
  }
  {
    auto cstr = "\t\r\n\f\va";
    String str{cstr, GetParam()};

    auto strip_str = String::StripLeading(str);
    ASSERT_EQ(strip_str, (String{"a", GetParam()}));
    ASSERT_EQ(1UL, strip_str.Length());
  }
  {  // Case all char are white space
    auto cstr = "\t\r\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::StripLeading(str);
    ASSERT_EQ(strip_str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, strip_str.Length());
  }
  {
    String str{"", GetParam()};

    auto strip_str = String::StripLeading(str);
    ASSERT_EQ(strip_str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, strip_str.Length());
  }
}

TEST_P(StringTest, StripLeading) {
  {
    auto cstr = " \t\r\n\f\v0123456789";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.StripLeading();
    ASSERT_EQ(str, (String{"0123456789", GetParam()}));
    ASSERT_EQ(len - 6, str.Length());
  }
  {
    auto cstr = " \t\raaa\n\f\v0123456789";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.StripLeading();
    ASSERT_EQ(str, (String{"aaa\n\f\v0123456789", GetParam()}));
    ASSERT_EQ(len - 3, str.Length());
  }
  {
    auto cstr = " \t\r012345678909876543210123456789\n\f\v";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.StripLeading();
    ASSERT_EQ(str,
              (String{"012345678909876543210123456789\n\f\v", GetParam()}));
    ASSERT_EQ(len - 3, str.Length());
  }
  {
    auto cstr = "\t\r\n\f\va";
    String str{cstr, GetParam()};

    str.StripLeading();
    ASSERT_EQ(str, (String{"a", GetParam()}));
    ASSERT_EQ(1UL, str.Length());
  }
  {  // Case all char are white space
    auto cstr = "\t\r\n\f\v";
    String str{cstr, GetParam()};

    str.StripLeading();
    ASSERT_EQ(str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, str.Length());
  }
  {
    String str{"", GetParam()};

    str.StripLeading();
    ASSERT_EQ(str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, str.Length());
  }
}

TEST_P(StringTest, StaticStripTrailing) {
  {
    auto cstr = "0123456789 \t\r\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::StripTrailing(str);
    ASSERT_EQ(strip_str, (String{"0123456789", GetParam()}));
    ASSERT_EQ(str.Length() - 6, strip_str.Length());
  }
  {
    auto cstr = "0123456789 \t\raaa\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::StripTrailing(str);
    ASSERT_EQ(strip_str, (String{"0123456789 \t\raaa", GetParam()}));
    ASSERT_EQ(str.Length() - 3, strip_str.Length());
  }
  {
    auto cstr = " \t\r012345678909876543210123456789\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::StripTrailing(str);
    ASSERT_EQ(strip_str,
              (String{" \t\r012345678909876543210123456789", GetParam()}));
    ASSERT_EQ(str.Length() - 3, strip_str.Length());
  }
  {
    auto cstr = "a\t\r\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::StripTrailing(str);
    ASSERT_EQ(strip_str, (String{"a", GetParam()}));
    ASSERT_EQ(1UL, strip_str.Length());
  }
  {  // Case all char are white space
    auto cstr = "\t\r\n\f\v";
    String str{cstr, GetParam()};

    auto strip_str = String::StripTrailing(str);
    ASSERT_EQ(strip_str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, strip_str.Length());
  }
  {
    String str{"", GetParam()};

    auto strip_str = String::StripTrailing(str);
    ASSERT_EQ(strip_str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, strip_str.Length());
  }
}

TEST_P(StringTest, StripTrailing) {
  {
    auto cstr = "0123456789 \t\r\n\f\v";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.StripTrailing();
    ASSERT_EQ(str, (String{"0123456789", GetParam()}));
    ASSERT_EQ(len - 6, str.Length());
  }
  {
    auto cstr = "0123456789 \t\raaa\n\f\v";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.StripTrailing();
    ASSERT_EQ(str, (String{"0123456789 \t\raaa", GetParam()}));
    ASSERT_EQ(len - 3, str.Length());
  }
  {
    auto cstr = " \t\r012345678909876543210123456789\n\f\v";
    String str{cstr, GetParam()};

    auto len = str.Length();
    str.StripTrailing();
    ASSERT_EQ(str, (String{" \t\r012345678909876543210123456789", GetParam()}));
    ASSERT_EQ(len - 3, str.Length());
  }
  {
    auto cstr = "a\t\r\n\f\v";
    String str{cstr, GetParam()};

    str.StripTrailing();
    ASSERT_EQ(str, (String{"a", GetParam()}));
    ASSERT_EQ(1UL, str.Length());
  }
  {  // Case all char are white space
    auto cstr = "\t\r\n\f\v";
    String str{cstr, GetParam()};

    str.StripTrailing();
    ASSERT_EQ(str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, str.Length());
  }
  {
    String str{"", GetParam()};

    str.StripTrailing();
    ASSERT_EQ(str, (String{"", GetParam()}));
    ASSERT_EQ(0UL, str.Length());
  }
}

TEST_P(StringTest, Reset) {
  {
    auto cstr = "123456789098765432101234567890987654321";
    String str{cstr, GetParam()};

    str.Reset();
    ASSERT_EQ(str.Length(), 0UL);
    ASSERT_EQ(str.Capacity(), 16UL);
  }
  {
    auto cstr = "1234567890";
    String str{cstr, GetParam()};

    str.Reset();
    ASSERT_EQ(str.Length(), 0UL);
    ASSERT_EQ(str.Capacity(), 16UL);
  }
}

}  // namespace jaclks
