#include "javac-base/lang/thread.h"

#if defined(__cplusplus)
extern "C" {
#endif
#if defined(JACLKS_OS_WINDOWS)
#include <process.h>  // for _beginthreadex
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif
#if defined(__cplusplus)
}
#endif

#include <atomic>
#include <cerrno>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <memory>
#include <thread>
#include <utility>

namespace jaclks::javac_base {

namespace {

#if defined(JACLKS_OS_WINDOWS)
static_assert(sizeof(HANDLE) <= sizeof(Thread::Id),
              "HANDLE size is smaller than Id");
#else
static_assert(sizeof(pthread_t) <= sizeof(Thread::Id),
              "pthread_t size is smaller than Id");
#endif

struct RunnableArg {
  RunnableArg(Runnable *_runnable, bool _owned)
      : runnable(_runnable), running(false), owned(_owned) {}

  Runnable *runnable;
  std::atomic<bool> running;
  bool owned;
};

struct RunnerDeleter {
  explicit RunnerDeleter(bool _owned) : owned(_owned) {}

  void operator()(Runnable *runnable) const {
    if (runnable != nullptr && owned) {
      delete runnable;
    }
  }

  bool owned;
};

#if defined(JACLKS_OS_WINDOWS)
unsigned __stdcall thread_call(void *arg) {
#else
void *thread_call(void *arg) {
#endif
  if (arg == nullptr) {
    std::terminate();
  }

  auto *runnable_arg = static_cast<RunnableArg *>(arg);
  bool owned = runnable_arg->owned;
  auto runner = runnable_arg->runnable;

  runnable_arg->running.store(true);

#if defined(__linux__)
#define THREAD_NAME_SIZE (16)
#else
#define THREAD_NAME_SIZE (256)
#endif
  char tname[THREAD_NAME_SIZE];
#if defined(JACLKS_OS_WINDOWS)
  std::uint64_t tid = GetCurrentThreadId();
  PWSTR name = nullptr;

  if (auto hr = GetThreadDescription(GetCurrentThread(), &name);
      SUCCEEDED(hr)) {
    WideCharToMultiByte(
        CP_UTF8, 0, name, -1, tname, THREAD_NAME_SIZE, nullptr, nullptr);
    LocalFree(name);
  } else {
    strncpy_s(tname, THREAD_NAME_SIZE, "unhnown", 7);
  }
#else
#if defined(JACLKS_OS_MACOS)
  auto ptid = pthread_self();
  std::uint64_t tid = 0;
  pthread_threadid_np(ptid, &tid);
  pthread_getname_np(ptid, tname, THREAD_NAME_SIZE);
#else
  auto tid = pthread_self();
  pthread_getname_np(tid, tname, THREAD_NAME_SIZE);
#endif
#endif

  auto defer =
      std::unique_ptr<Runnable, RunnerDeleter>(runner, RunnerDeleter{owned});
  try {
    runner->Run();
  } catch (const std::exception &e) {
    std::fprintf(stderr,
                 "[Thread-%" PRIu64 " %s] terminate with exception: %s\n",
                 tid,
                 tname,
                 e.what());
    // FIXME(BossZou): Consider if need rethrow exception here.
    // throw;
  } catch (...) {
    std::fprintf(stderr,
                 "[Thread-%" PRIu64 " %s] terminate with unknown exception\n",
                 tid,
                 tname);
    throw;
  }

#if defined(JACLKS_OS_WINDOWS)
  return 0;
#else
  return nullptr;
#endif
}

}  // namespace

Thread::Thread(const Runnable *runnable)
    : Thread(const_cast<Runnable *>(runnable), false) {}

Thread::Thread(Runnable *runnable) : Thread(runnable, true) {}

Thread::Thread(Thread &&other) noexcept
    : state_(other.state_),
      tid_(other.tid_),
      runner_(other.runner_),
      owned_(other.owned_) {
  other.state_ = State::kDone;
  other.tid_.id_ = 0;
  other.owned_ = false;
}

Thread &Thread::operator=(Thread &&other) noexcept {
  if (this != &other) {
    this->~Thread();
    new (this) Thread(std::move(other));
  }
  return *this;
}

Thread::~Thread() {
  Join();
}

int Thread::Start() {
#if defined(JACLKS_OS_WINDOWS)
  int ret = 0;
  HANDLE hThread =
      (HANDLE)_beginthreadex(nullptr, 0, thread_call, runner_, 0, nullptr);
  if (hThread == nullptr) {
    ret = errno;
    state_ = State::kFailed;
    delete runner_;
  } else {
    tid_.handle_ = hThread;
    state_ = State::kRunning;
  }

  runner_ = nullptr;
  return ret;
#else
  RunnableArg arg{runner_, owned_};
#if defined(JACLKS_OS_MACOS)
  auto ret = pthread_create(reinterpret_cast<pthread_t *>(&tid_.handle_),
                            nullptr,
                            thread_call,
                            runner_);
#else
  auto ret = pthread_create(
      reinterpret_cast<pthread_t *>(&tid_.id_), nullptr, thread_call, &arg);
#endif
  if (0 != ret) {
    state_ = State::kFailed;
    delete runner_;
  } else {
    while (!arg.running.load()) {
      Yield();
    }
    state_ = State::kRunning;
  }

  runner_ = nullptr;
  return ret;
#endif
}

int Thread::Cancel() {
  if (state_ != State::kRunning) {
    return ESRCH;
  }
#if defined(JACLKS_OS_WINDOWS)
  auto handle = static_cast<HANDLE>(tid_.handle_);
  if (TerminateThread(handle, 0)) {
    CloseHandle(handle);
    state_ = State::kCanceled;
    return 0;
  } else {
    state_ = State::kFailed;
    return errno;
  }
#else
#if defined(JACLKS_OS_MACOS)
  auto ptid = static_cast<pthread_t>(tid_.handle_);
#else
  auto ptid = static_cast<pthread_t>(tid_.id_);
#endif
  if (auto cancel_ret = pthread_cancel(ptid); cancel_ret != 0) {
    state_ = State::kFailed;
    return cancel_ret;
  } else {
    auto ret = pthread_join(ptid, nullptr);
    state_ = State::kCanceled;
    return ret;
  }
#endif
}

int Thread::Join() noexcept {
  if (state_ == State::kRunning) {
#if defined(JACLKS_OS_WINDOWS)
    int ret = 0;
    auto handle = static_cast<HANDLE>(tid_.handle_);
    WaitForSingleObject(handle, INFINITE);
#else
#if defined(JACLKS_OS_MACOS)
    auto ptid = static_cast<pthread_t>(tid_.handle_);
#else
    auto ptid = static_cast<pthread_t>(tid_.id_);
#endif
    auto ret = pthread_join(ptid, nullptr);
#endif
    state_ = State::kDone;
    return ret;
  }
  if (runner_) {
    delete runner_;
    runner_ = nullptr;
  }
  return 0;
}

Thread::Thread(Runnable *runnable, bool owned)
    : state_(State::kInit), tid_(0), runner_(runnable), owned_(owned) {}

#if defined(JACLKS_OS_WINDOWS)
#ifdef Yield  // ![Note]: Yield is a macro on windows, here must undef it first.
#undef Yield
#endif
void Thread::Yield() noexcept {
  SwitchToThread();
}

void Thread::Sleep(std::int64_t millis) {
  ::Sleep(static_cast<DWORD>(millis));
}
#else
void Thread::Yield() noexcept {
  sched_yield();
}

void Thread::Sleep(std::int64_t millis) {
  usleep(millis * 1000);
}
#endif

}  // namespace jaclks::javac_base
