#pragma once

#include <string>

#include "jaclks/javac-base/java/lang/string.h"

namespace jaclks::cxx_detail {

class StringUtils {
 public:
  static std::string ToStdString(javac_base::String str);
};

}  // namespace jaclks::cxx_detail
