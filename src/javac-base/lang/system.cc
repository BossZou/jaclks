#include "javac-base/lang/system.h"

#include <chrono>

namespace jaclks::javac_base {

std::int64_t System::CurrentTimeMillis() {
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
}

}  // namespace jaclks::javac_base
