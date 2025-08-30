#pragma once

#include <cstdint>

namespace jaclks::javac_base {

class System final {
 public:
  static std::int64_t CurrentTimeMillis();
};

}  // namespace jaclks::javac_base
