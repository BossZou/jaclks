#include "jaclks/javac-base/java/lang/character.h"

#include <cctype>

namespace jaclks::javac_base {

bool Character::IsWhitespace(int c) {
  return std::isspace(c);
}

}  // namespace jaclks
