#include "concurrent/thread.h"

namespace jaclks {

namespace {
void* thread_call(void *arg) {
    if (arg != nullptr) {
        auto runner = reinterpret_cast<Thread::Runner *>(arg);
        runner->Run();

        delete runner;
    }
}
}

Thread::~Thread() {
    if (runner_ != nullptr) {
        delete runner_;
        runner_ = nullptr;
    }
}

int Thread::Start() {
    auto ret = pthread_create(&tid_, nullptr, thread_call, runner_);
    if (0 == ret) {
        runner_ = nullptr;
    }

    return ret;
}

int Thread::Join() {
    return pthread_join(tid_, nullptr);
}

}
