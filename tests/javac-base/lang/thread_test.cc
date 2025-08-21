#include "javac-base/lang/thread.h"

#ifdef JACLKS_OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <gtest/gtest.h>

#include <cerrno>
#include <cmath>
#include <memory>
#include <mutex>

#include "javac-base/lang/system.h"

namespace jaclks::javac_base {

TEST(ThreadTest, Normal) {
  auto func = [](int *a) { *a += 1; };

  int val = 10;
  auto t = Thread{func, &val};

  ASSERT_EQ(t.Start(), 0);
  ASSERT_EQ(t.Join(), 0);

  ASSERT_EQ(val, 11);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
#if defined(JACLKS_OS_WINDOWS)
    Sleep(3000);
#else
    sleep(3);
#endif
    *is_called = true;
  };

  auto t = Thread{func, &called, std::make_unique<CancelArg>(&exited)};

  ASSERT_EQ(t.Start(), 0);
#if defined(JACLKS_OS_WINDOWS)
  Sleep(1000);
#else
  sleep(1);
#endif
  ASSERT_EQ(t.Cancel(), 0);

  ASSERT_FALSE(called);
#if !defined(JACLKS_OS_WINDOWS) && !defined(JACLKS_OS_MACOS)
  ASSERT_TRUE(exited);
#endif
}

TEST(ThreadTest, CancelBeforeStart) {
  auto func = [](int *a) { *a += 1; };

  int val = 10;
  auto t = Thread{func, &val};

  ASSERT_EQ(ESRCH, t.Cancel());
}

TEST(ThreadTest, MoveConstruct) {
  auto func = [](int *a) {
#if defined(JACLKS_OS_WINDOWS)
    Sleep(3000);
#else
    sleep(3);
#endif
    *a += 1;
  };

  int val = 10;
  auto t = Thread{func, &val};
  t.Start();

  auto other = std::move(t);
  ASSERT_EQ(0, other.Join());
  ASSERT_EQ(11, val);
}

TEST(ThreadTest, MoveAssignment) {
  auto func = [](int *a) {
#if defined(JACLKS_OS_WINDOWS)
    Sleep(3000);
#else
    sleep(3);
#endif
    *a += 1;
  };

  int val = 10;
  auto t = Thread{func, &val};
  t.Start();

  auto other = Thread{func, &val};
  t = std::move(other);
  t.Start();

  ASSERT_EQ(0, t.Join());
  ASSERT_EQ(12, val);
}

TEST(ThreadTest, Sleep) {
  {
    auto t0 = System::CurrentTimeMillis();
    Thread::Sleep(0);
    auto t1 = System::CurrentTimeMillis();

    ASSERT_LT(t1 - t0, 20);
  }
  {
    auto t0 = System::CurrentTimeMillis();
    Thread::Sleep(100);
    auto t1 = System::CurrentTimeMillis();
#if defined(JACLKS_OS_MACOS)
    ASSERT_LE(std::abs(t1 - t0 - 100), 100);
#else
    ASSERT_LE(std::abs(t1 - t0 - 100), 30);
#endif
  }
  {
    auto t0 = System::CurrentTimeMillis();
    Thread::Sleep(10000);
    auto t1 = System::CurrentTimeMillis();

    ASSERT_LT(std::abs(t1 - t0 - 10000), 50);
  }
}

TEST(ThreadTest, Yield) {
  auto func = []() {
    auto t0 = System::CurrentTimeMillis();

    while (System::CurrentTimeMillis() - t0 < 1000) {
#if defined(JACLKS_OS_WINDOWS)
#undef Yield
#endif
      Thread::Yield();
    }
  };

  auto t = Thread{func};
  ASSERT_EQ(0, t.Start());
  ASSERT_EQ(0, t.Join());
}

}  // namespace jaclks::javac_base
