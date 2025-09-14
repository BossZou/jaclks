#pragma once
#include <functional>

namespace jaclks {

template <typename T, typename = void>
struct Equaler {
  std::size_t operator()(const T &x) const noexcept {
    return std::equal_to<T>{}(x);
  }
};

}
