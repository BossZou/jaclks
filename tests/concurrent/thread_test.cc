#include "concurrent/thread.h"

#include <gtest/gtest.h>

namespace jaclks {

TEST(Thread, Normal) {
  auto func = [](int *a) { *a += 1; };

  int val = 10;
  auto t = Thread{func, &val};

  ASSERT_EQ(t.Start(), 0);
  ASSERT_EQ(t.Join(), 0);

  ASSERT_EQ(val, 11);
}

}  // namespace jaclks
