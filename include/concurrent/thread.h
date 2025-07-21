#pragma once

#include <pthread.h>

#include <tuple>
#include <type_traits>

namespace jaclks {

class Thread {
 public:
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
          tuple_(std::forward<Args>(args)...) {
    }

    ~RunnerImpl() override = default;

    void Run() override {
      constexpr auto kArgSize = std::tuple_size<std::decay_t<Tuple>>::value;
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
                                              std::forward<Args>(args)...}) {
  }

  ~Thread();

  int Start();

  int Join();

 private:
  pthread_t tid_;
  Runner *runner_;
};

}  // namespace jaclks
