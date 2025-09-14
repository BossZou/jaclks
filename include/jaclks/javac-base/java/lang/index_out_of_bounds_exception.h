#pragma once

#include <stdexcept>

namespace jaclks::javac_base {

class IndexOutOfBoundsException : public std::runtime_error {
 public:
  explicit IndexOutOfBoundsException(const char *s) : std::runtime_error(s) {}
};

}  // namespace jaclks::javac_base
