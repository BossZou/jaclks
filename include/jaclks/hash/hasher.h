#pragma once

#include <functional>
#include <unordered_map>

#include "jaclks/compile/type_traits.h"
#include "hash/types.h"

namespace jaclks {

template <typename T, typename = void>
struct Hasher {
  Hash_t operator()(const T &x) const noexcept {
    return std::hash<T>{}(x);
  }
};

template <typename T>
struct Hasher<T *> {
  Hash_t operator()(const T *x) const noexcept {
    if (x == nullptr) {
      return 0;
    }
    return Hasher<T>{}(*x);
  }
};

template <typename T>
struct Hasher<T, std::enable_if_t<is_smart_pointer_v<T>>> {
  using Type = std::decay_t<typename T::element_type>;

  Hash_t operator()(const T &x) const noexcept {
    if (x == nullptr) {
      return 0;
    }
    return Hasher<decltype(x.get())>{}(x.get());
  }
};

}  // namespace jaclks
