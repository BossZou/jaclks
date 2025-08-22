#include "javac-base/lang/character.h"

#include <gtest/gtest.h>

namespace jaclks {

TEST(CharacterTest, IsWhitespace) {
  std::set<int> white_chars{' ', '\t', '\n', '\r', '\f', '\v'};

  for (auto ch : white_chars) {
    ASSERT_TRUE(Character::IsWhitespace(ch));
  }

  for (auto i = 0; i < 128; ++i) {
    if (white_chars.find(i) == white_chars.end()) {
      ASSERT_FALSE(Character::IsWhitespace(i));
    }
  }
}

}  // namespace jaclks
