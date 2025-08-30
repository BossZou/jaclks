#include "jaclks/javac-base/java/lang/system.h"

#include <chrono>

namespace jaclks::javac_base {

std::int64_t System::CurrentTimeMillis() {
  using namespace std::chrono;
  auto now = system_clock::now();
  return duration_cast<milliseconds>(now.time_since_epoch()).count();
}

}  // namespace jaclks::javac_base
