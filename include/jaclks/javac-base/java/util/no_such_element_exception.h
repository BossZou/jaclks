#pragma once

#include <stdexcept>

namespace jaclks::javac_base {

class NoSuchElementException : public std::runtime_error {
 public:
  explicit NoSuchElementException(const char* s) : std::runtime_error(s) {}
};

}  // namespace jaclks::javac_base
