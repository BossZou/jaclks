#include "jaclks/javac-base/lang/system.h"

#include <gtest/gtest.h>

namespace jaclks::javac_base {

TEST(SystemTest, CurrentTimeMillis) {
  constexpr std::int64_t kYearMillis = 55LL * 365 * 24 * 60 * 60 * 1000;
  ASSERT_LT(kYearMillis, System::CurrentTimeMillis());
}

}  // namespace jaclks::javac_base
