#include "concurrent/thread.h"

#if defined(__cplusplus)
extern "C" {
#endif
#if defined(JACLKS_OS_WINDOWS)
#include <process.h>  // for _beginthreadex
#include <windows.h>
#else
#include <pthread.h>
#endif
#if defined(__cplusplus)
}
#endif

#include <cerrno>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <memory>
#include <utility>

namespace jaclks {

namespace {

#if defined(JACLKS_OS_WINDOWS)
static_assert(sizeof(HANDLE) <= sizeof(Thread::Id),
              "HANDLE size is smaller than Id");
#else
static_assert(sizeof(pthread_t) <= sizeof(Thread::Id),
              "pthread_t size is smaller than Id");
#endif

#if defined(JACLKS_OS_WINDOWS)
unsigned __stdcall thread_call(void *arg) {
#else
void *thread_call(void *arg) {
#endif
  if (arg != nullptr) {
#if defined(__linux__)
#define THREAD_NAME_SIZE (16)
#else
#define THREAD_NAME_SIZE (256)
#endif
    char tname[THREAD_NAME_SIZE];
#if defined(JACLKS_OS_WINDOWS)
    auto tid = GetCurrentThreadId();
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

    auto runner = static_cast<Thread::Runner *>(arg);
    auto defer = std::unique_ptr<Thread::Runner>(runner);
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
  }

#if defined(JACLKS_OS_WINDOWS)
  return 0;
#else
  return nullptr;
#endif
}

}  // namespace

Thread::Thread(Thread &&other) noexcept
    : state_(other.state_), tid_(other.tid_), runner_(other.runner_) {
  other.state_ = State::kDone;
  other.tid_.id_ = 0;
  other.runner_ = nullptr;
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
#if defined(JACLKS_OS_MACOS)
  auto ret = pthread_create(reinterpret_cast<pthread_t *>(&tid_.handle_),
                            nullptr,
                            thread_call,
                            runner_);
#else
  auto ret = pthread_create(
      reinterpret_cast<pthread_t *>(&tid_.id_), nullptr, thread_call, runner_);
#endif
  if (0 != ret) {
    state_ = State::kFailed;
    delete runner_;
  } else {
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

}  // namespace jaclks
