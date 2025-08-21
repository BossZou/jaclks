#include "javac-base/lang/system.h"

#include <gtest/gtest.h>

namespace jaclks::javac_base {

TEST(SystemTest, CurrentTimeMillis) {
  ASSERT_LT(55L * 365 * 24 * 60 * 60 * 1000L, System::CurrentTimeMillis());
}

}  // namespace jaclks::javac_base
