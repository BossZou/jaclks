#include "concurrent/thread.h"

#include <cstdio>
#include <memory>

namespace jaclks {

namespace {

void *thread_call(void *arg) {
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

  return nullptr;
}

}  // namespace

Thread::~Thread() {
  Join();
}

int Thread::Start() {
  auto ret = pthread_create(&tid_, nullptr, thread_call, runner_);
  if (0 != ret) {
    delete runner_;
  }

  runner_ = nullptr;
  return ret;
}

int Thread::Cancel() {
  if (auto cancel_ret = pthread_cancel(tid_); cancel_ret != 0) {
    return cancel_ret;
  } else {
    return pthread_join(tid_, nullptr);
  }
}

int Thread::Join() {
  // TODO(BossZou): Here if runner_ is not null mean thread not start.
  // Try use
  if (runner_) {
    delete runner_;
    return 0;
  } else {
    return pthread_join(tid_, nullptr);
  }
}

}  // namespace jaclks
