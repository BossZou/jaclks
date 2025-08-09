#include "lang/character.h"

#include <gtest/gtest.h>

namespace jaclks {

TEST(CharacterTest, IsWhitespace) {
  ASSERT_TRUE(Character::IsWhitespace(' '));
  ASSERT_TRUE(Character::IsWhitespace('\t'));
  ASSERT_TRUE(Character::IsWhitespace('\n'));
  ASSERT_TRUE(Character::IsWhitespace('\r'));
  ASSERT_TRUE(Character::IsWhitespace('\f'));
  ASSERT_TRUE(Character::IsWhitespace('\v'));
  ASSERT_TRUE(Character::IsWhitespace('\0'));
}

}  // namespace jaclks
