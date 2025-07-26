#include "concurrent/thread.h"

namespace jaclks {

namespace {

void *thread_call(void *arg) {
  if (arg != nullptr) {
    auto runner = reinterpret_cast<Thread::Runner *>(arg);
    runner->Run();
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
  return pthread_join(tid_, nullptr);
}

}  // namespace jaclks
