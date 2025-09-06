#pragma once

#include "jaclks-internal/javac-base/java/util/regex/regex_impl.h"
#include "jaclks/javac-base/java/lang/string.h"

namespace jaclks::javac_base {

class Matcher {
 public:
  Matcher(RegexImpl *regex, String input);

 private:
  class MatcherInner;

  RegexImpl *regex_;

  String text_;
  MatcherInner *inner_;
};

}  // namespace jaclks::javac_base