#pragma once

#include <stdexcept>

namespace jaclks::javac_base {

class IllegalStateException : public std::runtime_error {
 public:
  explicit IllegalStateException(const char *s) : std::runtime_error(s) {}
};

}  // namespace jaclks::javac_base
