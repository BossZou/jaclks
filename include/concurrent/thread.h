#pragma once

#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace jaclks {

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

  class Runner {
   public:
    virtual ~Runner() = default;

    virtual void Run() = 0;
  };

  template <typename Callable, typename... Args>
  class RunnerImpl : public Runner {
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

  template <typename Callable, typename... Args>
  explicit Thread(Callable &&f, Args &&... args)
      : state_(State::kInit),
        tid_(),
        runner_(
            new RunnerImpl<Callable, Args...>{std::forward<Callable>(f),
                                              std::forward<Args>(args)...}) {}

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

 private:
  volatile State state_;
  Id tid_;
  Runner *runner_;
};

}  // namespace jaclks
