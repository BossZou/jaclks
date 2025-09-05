#include "jaclks/javac-base/java/lang/thread.h"

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

#include "jaclks/javac-base/java/lang/system.h"

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

TEST(ThreadTest, MoveAssignmentSelf) {
  struct ThreadRef {
    explicit ThreadRef(Thread &_ref) : ref(_ref) {}

    Thread &ref;
  };

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

  ThreadRef ref{t};
  t = std::move(ref.ref);

  ASSERT_EQ(0, t.Join());
  ASSERT_EQ(11, val);
}

TEST(ThreadTest, OwnRunnable) {
  class SingleRunner : public Runnable {
   public:
    explicit SingleRunner(std::shared_ptr<int> val, std::atomic<bool> &flag)
        : val_(std::move(val)), flag_(flag) {}

    ~SingleRunner() override {
      flag_.store(true);
    }

    void Run() override {
      *val_ += 5;
    }

   private:
    std::shared_ptr<int> val_;
    std::atomic<bool> &flag_;
  };

  auto val = std::make_shared<int>(5);
  std::atomic<bool> flag{false};

  Thread t{new SingleRunner(val, flag)};

  t.Start();
  t.Join();

  ASSERT_EQ(10, *val);
  ASSERT_TRUE(flag.load());
}

TEST(ThreadTest, UnownRunnable) {
  class SingleRunner : public Runnable {
   public:
    explicit SingleRunner(std::atomic<int> &counter) : counter_(counter) {}

    ~SingleRunner() override {
      counter_.fetch_add(1);
    }

    void Run() override {}

   private:
    std::atomic<int> &counter_;
  };

  std::atomic<int> counter{0};

  int thread_num = 50;
  std::vector<Thread> threads;

  SingleRunner runner{counter};
  for (auto i = 0; i < thread_num; ++i) {
    threads.emplace_back(static_cast<const Runnable *>(&runner));
  }

  for (auto &t : threads) {
    ASSERT_EQ(0, t.Start());
  }
  for (auto &t : threads) {
    ASSERT_EQ(0, t.Join());
  }

  ASSERT_EQ(0, counter.load());
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
    ASSERT_LE(std::abs(t1 - t0 - 100), 150);
#else
    ASSERT_LE(std::abs(t1 - t0 - 100), 30);
#endif
  }
  {
    auto t0 = System::CurrentTimeMillis();
    Thread::Sleep(10000);
    auto t1 = System::CurrentTimeMillis();

#if defined(JACLKS_OS_MACOS)
    ASSERT_LT(std::abs(t1 - t0 - 10000), 250);
#else
    ASSERT_LT(std::abs(t1 - t0 - 10000), 50);
#endif
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
