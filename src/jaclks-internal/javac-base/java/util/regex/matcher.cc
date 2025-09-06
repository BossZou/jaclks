#include "jaclks/javac-base/java/util/regex/matcher.h"

#include "boost/regex/v5/regex.hpp"

namespace jaclks::javac_base {

class Matcher::MatcherInner {
 private:
  boost::smatch what_;
};

Matcher::Matcher(RegexImpl *regex, String input)
    : regex_(regex), text_(std::move(input)) {}

}  // namespace jaclks::javac_base
