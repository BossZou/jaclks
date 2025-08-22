#include "javac-base/lang/system.h"

#include <chrono>

namespace jaclks::javac_base {

std::int64_t System::CurrentTimeMillis() {
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             now.time_since_epoch())
      .count();
}

}  // namespace jaclks::javac_base
