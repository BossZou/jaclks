#pragma once

#include <stdexcept>

namespace jaclks::javac_base {

class IllegalArgumentException : public std::runtime_error {
public:
  explicit IllegalArgumentException(const char* s) : std::runtime_error(s) {}
};

}
