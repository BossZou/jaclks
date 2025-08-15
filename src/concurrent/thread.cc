#include "concurrent/thread.h"

#include <thread>

#if defined(__cplusplus)
extern "C" {
#endif
#if defined(__MSVC__)
#include <process.h>  // for _beginthreadex
#include <windows.h>
#else
#include <pthread.h>
#endif
#if defined(__cplusplus)
}
#endif

namespace jaclks {

namespace {

#if defined(__MSVC__)
static_assert(sizeof(HANDLE) <= sizeof(Thread::Id),
              "HANDLE size is smaller than Id");
#else
static_assert(sizeof(pthread_t) <= sizeof(Thread::Id),
              "pthread_t size is smaller than Id");
#endif

#if defined(__MSVC__)
unsigned __stdcall thread_call(void *arg) {
#else
void *thread_call(void *arg) {
#endif
  if (arg != nullptr) {
    auto runner = reinterpret_cast<Thread::Runner *>(arg);
    runner->Run();
  }

#if defined(__MSVC__)
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
#if defined(__MSVC__)
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
}
#else
  auto ret = pthread_create(
      reinterpret_cast<pthread_t *>(&tid_.id_), nullptr, thread_call, runner_);
  if (0 != ret) {
    delete runner_;
  }

  runner_ = nullptr;
  return ret;
#endif
}  // namespace jaclks

int Thread::Cancel() {
#if defined(__MSVC__)
  auto handle = static_cast<pthread_t>(tid_.handle_);
  if (TerminateThread(handle, 0)) {
    CloseHandle(handle);
    return 0;
  } else {
    return errno;
  }
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
#if defined(__MSVC__)
  WaitForSingleObject(hThread2, INFINITE);
  return 0;
#else
  auto ptid = static_cast<pthread_t>(tid_.id_);
  return pthread_join(ptid, nullptr);
#endif
}

}  // namespace jaclks
