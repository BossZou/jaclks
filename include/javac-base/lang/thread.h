#pragma once

#include <cstdint>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#include "javac-base/lang/runnable.h"

namespace jaclks::javac_base {

class Thread {
 public:
  enum class State {
    kInit = 0,
    kRunning,
    kFailed,
    kCanceled,
    kDone,
  };

  class Id final {
   public:
    explicit Id(std::int64_t id = 0) : id_(id) {}

   private:
    friend class Thread;

    union {
      std::int64_t id_;
      void *handle_;
    };
  };

  template <typename Callable, typename... Args>
  class RunnerImpl : public Runnable {
   public:
    using Tuple = std::tuple<Args...>;

    explicit RunnerImpl(Callable &&f, Args &&... args)
        : call_(std::forward<Callable>(f)),
          tuple_(std::forward<Args>(args)...) {}

    ~RunnerImpl() override = default;

    void Run() override {
      constexpr auto kArgSize = std::tuple_size_v<std::decay_t<Tuple>>;
      call_impl(std::forward<Callable>(call_),
                std::forward<Tuple>(tuple_),
                std::make_index_sequence<kArgSize>{});
    }

   private:
    template <std::size_t... I>
    decltype(auto) call_impl(Callable &&f,
                             Tuple &&t,
                             std::index_sequence<I...>) {
      return std::forward<Callable>(f)(std::get<I>(std::forward<Tuple>(t))...);
    }

    Callable call_;
    Tuple tuple_;
  };

  template <typename Callable,
            typename... Args,
            typename = std::void_t<
                decltype(std::declval<Callable>()(std::declval<Args>()...))>>
  explicit Thread(Callable &&f, Args &&... args)
      : Thread(new RunnerImpl<Callable, Args...>{std::forward<Callable>(f),
                                                 std::forward<Args>(args)...},
               true) {}

  explicit Thread(const Runnable *runnable);

  explicit Thread(Runnable *runnable);

  Thread(Thread &) = delete;
  Thread(const Thread &) = delete;

  Thread(const Thread &&) = delete;

  Thread(Thread &&other) noexcept;

  Thread &operator=(Thread &) = delete;
  Thread &operator=(const Thread &) = delete;

  Thread &operator=(Thread &&other) noexcept;

  ~Thread();

  int Start();

  /**
   * Cancels this thread immediately or at the next possibility,
   * and join wait for termination of the thread.
   *
   * @return status code
   *    - 0 for success
   * @note The method must only be invoked while process is dying,
   * cause the thread resource may be not deconstructed.
   */
  int Cancel();

  int Join() noexcept;

  static void Yield() noexcept;

  static void Sleep(std::int64_t millis);

 private:
  Thread(Runnable *runnable, bool owned);

  volatile State state_;
  Id tid_;
  Runnable *runner_;
  bool owned_;
};

}  // namespace jaclks::javac_base
