#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace jaclks {

class Thread {
 public:
  class Id final {
   public:
    explicit Id(long int id = 0) : id_(id) {}

   private:
    friend class Thread;

    union {
      long int id_;
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
    template <size_t... I>
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
      : tid_(),
        runner_(
            new RunnerImpl<Callable, Args...>{std::forward<Callable>(f),
                                              std::forward<Args>(args)...}) {}

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

  int Join();

 private:
  Id tid_;
  Runner *runner_;
};

}  // namespace jaclks
