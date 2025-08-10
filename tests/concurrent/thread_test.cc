#include "concurrent/thread.h"

#include <gtest/gtest.h>

#include <memory>
#include <mutex>

namespace jaclks {

TEST(Thread, Normal) {
  auto func = [](int *a) { *a += 1; };

  int val = 10;
  auto t = Thread{func, &val};

  ASSERT_EQ(t.Start(), 0);
  ASSERT_EQ(t.Join(), 0);

  ASSERT_EQ(val, 11);
}

TEST(Thread, NoParam) {
  auto int_a = 10;
  auto func = [&int_a]() { ++int_a; };

  auto t = Thread{func};

  ASSERT_EQ(t.Start(), 0);
  ASSERT_EQ(t.Join(), 0);

  ASSERT_EQ(int_a, 11);
}

TEST(Thread, UinqPtrArg) {
  auto func = [](std::unique_ptr<int> a_p, int *r) {
    *a_p += 1;
    *r = *a_p;
  };

  auto val = std::make_unique<int>(10);
  int res;
  auto t = Thread{func, std::move(val), &res};

  ASSERT_EQ(t.Start(), 0);
  ASSERT_EQ(t.Join(), 0);

  ASSERT_EQ(res, 11);
}

TEST(Thread, Cancel) {
  bool called = false;

  auto func = [](bool *is_called) {
    sleep(3);
    *is_called = true;
  };

  auto t = Thread{func, &called};

  ASSERT_EQ(t.Start(), 0);
  sleep(1);
  ASSERT_EQ(t.Cancel(), 0);

  ASSERT_FALSE(called);
}

}  // namespace jaclks
