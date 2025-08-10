#include "concurrent/thread.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <gtest/gtest.h>

#include <cerrno>
#include <memory>
#include <mutex>

namespace jaclks {

TEST(ThreadTest, Normal) {
  auto func = [](int *a) { *a += 1; };

  int val = 10;
  auto t = Thread{func, &val};

  ASSERT_EQ(t.Start(), 0);
  ASSERT_EQ(t.Join(), 0);

  ASSERT_EQ(val, 11);
}

TEST(ThreadTest, NoParam) {
  auto int_a = 10;
  auto func = [&int_a]() { ++int_a; };

  auto t = Thread{func};

  ASSERT_EQ(t.Start(), 0);
  ASSERT_EQ(t.Join(), 0);

  ASSERT_EQ(int_a, 11);
}

TEST(ThreadTest, UniqPtrArg) {
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

TEST(ThreadTest, ThrowRuntimeError) {
  auto func = [](int *a) {
    if (nullptr == a) {
      throw std::runtime_error("null pointer");
    }
    *a += 1;
  };

  auto t = Thread{func, nullptr};

  ASSERT_EQ(t.Start(), 0);
  ASSERT_EQ(t.Join(), 0);
}

TEST(ThreadTest, ThrowStdException) {
  auto func = [](int *a) {
    if (nullptr == a) {
      throw std::logic_error("null pointer");
    }
    *a += 1;
  };

  auto t = Thread{func, nullptr};

  ASSERT_EQ(t.Start(), 0);
  ASSERT_EQ(t.Join(), 0);
}

TEST(ThreadTest, Cancel) {
  struct CancelArg {
    explicit CancelArg(bool *_stat) : stat(_stat) {}

    ~CancelArg() {
      *stat = true;
    }

    bool *stat;
  };

  bool called = false;
  bool exited = false;

  auto func = [](bool *is_called, std::unique_ptr<CancelArg> arg) {
    (void)(arg.get());
#if defined(_WIN32)
    Sleep(3000);
#else
    sleep(3);
#endif
    *is_called = true;
  };

  auto t = Thread{func, &called, std::make_unique<CancelArg>(&exited)};

  ASSERT_EQ(t.Start(), 0);
#if defined(_WIN32)
  Sleep(1000);
#else
  sleep(1);
#endif
  ASSERT_EQ(t.Cancel(), 0);

  ASSERT_FALSE(called);
  ASSERT_TRUE(exited);
}

TEST(ThreadTest, CancelBeforeStart) {
  auto func = [](int *a) { *a += 1; };

  int val = 10;
  auto t = Thread{func, &val};

  ASSERT_EQ(ESRCH, t.Cancel());
}

}  // namespace jaclks
