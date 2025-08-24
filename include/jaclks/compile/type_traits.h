#pragma once

#include <memory>
#include <type_traits>

namespace jaclks {

template <typename T>
struct IsSmartPointer : std::false_type {};

template <typename T>
struct IsSmartPointer<std::unique_ptr<T>> : std::true_type {};

template <typename T>
struct IsSmartPointer<std::shared_ptr<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_smart_pointer_v = IsSmartPointer<T>::value;

}  // namespace jaclks
