#pragma once

#include <stdexcept>

#include "jaclks/javac-base/java/lang/string.h"

namespace jaclks::javac_base {

class IndexOutOfBoundsException : public std::runtime_error {
public:
  explicit IndexOutOfBoundsException(const String& s) : std::runtime_error(s.CStr()) {}
};

}