#pragma once

#include <functional>

namespace jaclks {

template <typename T, typename = void>
struct Hasher {
std::size_t operator()(const T& x) const noexcept {
return std::hash<T>{}(x);
}
};

template <typename T>
struct Hasher<T*> {
        std::size_t operator()(const T* x) const noexcept {
            if (x == nullptr) {
                return 0;
            }
            return std::hash<T>{}(*x);
        }
    };

}
