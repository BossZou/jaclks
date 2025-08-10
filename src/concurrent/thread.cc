#include "concurrent/thread.h"

#if defined(__cplusplus)
extern "C" {
#endif
#if defined(_WIN32)
#include <process.h>  // for _beginthreadex
#include <windows.h>
#else
#include <pthread.h>
#endif
#if defined(__cplusplus)
}
#endif

#include <cstdio>
#include <memory>

namespace jaclks {

namespace {

#if defined(_WIN32)
static_assert(sizeof(HANDLE) <= sizeof(Thread::Id),
              "HANDLE size is smaller than Id");
#else
static_assert(sizeof(pthread_t) <= sizeof(Thread::Id),
              "pthread_t size is smaller than Id");
#endif

#if defined(_WIN32)
unsigned __stdcall thread_call(void *arg) {
#else
void *thread_call(void *arg) {
#endif
  if (arg != nullptr) {
#if defined(__linux__)
#define THREAD_NAME_SIZE (16)
#else
#warning "Other platform which name size limit may confuse"
#define THREAD_NAME_SIZE (64)
#endif
    char tname[THREAD_NAME_SIZE];
    auto tid = pthread_self();

    pthread_getname_np(tid, tname, THREAD_NAME_SIZE);

    auto runner = static_cast<Thread::Runner *>(arg);
    auto defer = std::unique_ptr<Thread::Runner>(runner);
    try {
      runner->Run();
    } catch (const std::exception &e) {
      std::fprintf(stderr,
                   "[Thread-%lu %s] terminate with exception: %s\n",
                   tid,
                   tname,
                   e.what());
      // FIXME(BossZou): Consider if need rethrow exception here.
      // throw;
    } catch (...) {
      std::fprintf(stderr,
                   "[Thread-%lu %s] terminate with unknown exception\n",
                   tid,
                   tname);
      throw;
    }
  }

#if defined(_WIN32)
  return 0;
#else
  return nullptr;
#endif
}

}  // namespace

Thread::~Thread() {
  Join();
}

int Thread::Start() {
#if defined(_WIN32)
  int ret = 0;
  HANDLE hThread =
      (HANDLE)_beginthreadex(nullptr, 0, thread_call, runner_, 0, nullptr);
  if (hThread == nullptr) {
    ret = errno;
    delete runner_;
  } else {
    tid_.handle_ = hThread;
  }

  runner_ = nullptr;
  return ret;
#else
  auto ret = pthread_create(
      reinterpret_cast<pthread_t *>(&tid_.id_), nullptr, thread_call, runner_);
  if (0 != ret) {
    delete runner_;
  }

  runner_ = nullptr;
  return ret;
#endif
}

int Thread::Cancel() {
#if defined(_WIN32)
  auto handle = static_cast<HANDLE>(tid_.handle_);
  if (TerminateThread(handle, 0)) {
    CloseHandle(handle);
    return 0;
  } else {
    return errno;
  }
#else
  auto ptid = static_cast<pthread_t>(tid_.id_);
  if (auto cancel_ret = pthread_cancel(ptid); cancel_ret != 0) {
    return cancel_ret;
  } else {
    return pthread_join(ptid, nullptr);
  }
#endif
}

int Thread::Join() {
  // TODO(BossZou): Here if runner_ is not null mean thread not start.
  // Try use
  if (runner_) {
    delete runner_;
    return 0;
  } else {
#if defined(_WIN32)
    auto handle = static_cast<HANDLE>(tid_.handle_);
    WaitForSingleObject(handle, INFINITE);
    return 0;
#else
    auto ptid = static_cast<pthread_t>(tid_.id_);
    return pthread_join(ptid, nullptr);
#endif
  }
}

}  // namespace jaclks
