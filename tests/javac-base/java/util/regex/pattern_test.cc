#include "jaclks/javac-base/java/util/regex/pattern.h"

#include <gtest/gtest.h>

namespace jaclks::javac_base {

TEST(PatternTest, Normal) {
  auto regex = Pattern::Compile("123");
}

}  // namespace jaclks::javac_base
