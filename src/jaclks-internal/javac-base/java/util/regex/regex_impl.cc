#include "jaclks-internal/javac-base/java/util/regex/regex_impl.h"

namespace jaclks::javac_base {

boost::regex &RegexImpl::Regex() {
  return regex_;
}

}  // namespace jaclks::javac_base
