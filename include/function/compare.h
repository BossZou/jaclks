#pragma once

#include <string>
#include <string_view>
#include <type_traits>

#include "compile/type_traits.h"

namespace jaclks {

template <typename T, typename = void>
struct Comparator {
  int operator()(const T &a, const T &b) const noexcept {
    if (a < b) {
      return -1;
    }

    return a == b ? 0 : 1;
  }
};

template <>
struct Comparator<std::string> {
  int operator()(const std::string &a, const std::string &b) const noexcept {
    return a.compare(b);
  }
};

template <>
struct Comparator<std::string_view> {
  int operator()(const std::string_view &a, const std::string_view &b) const
      noexcept {
    return a.compare(b);
  }
};

template <typename T>
struct Comparator<T *> {
  int operator()(const T *a, const T *b) const noexcept {
    if (a == b) {
      return 0;
    }

    if (a == nullptr || b == nullptr) {
      return a == nullptr ? -1 : 1;
    }

    return Comparator<T>{}(*a, *b);
  }
};

template <typename T>
struct Comparator<T, std::enable_if_t<is_smart_pointer_v<T>>> {
  using Type = std::decay_t<typename T::element_type>;

  std::size_t operator()(const T &a, const T &b) const noexcept {
    if (a.get() == b.get()) {
      return 0;
    }

    if (a == nullptr || b == nullptr) {
      return a == nullptr ? -1 : 1;
    }

    return Comparator<Type>{}(*a, *b);
  }
};

template <typename T, typename = void>
struct Lesser {
  bool operator()(const T &a, const T &b) const noexcept {
    return std::less<T>(a, b);
  }
};

}  // namespace jaclks
