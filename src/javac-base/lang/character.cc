#include "javac-base/lang/character.h"

#include <cctype>

namespace jaclks {

bool Character::IsWhitespace(int c) {
  return std::isspace(c);
}

}  // namespace jaclks
