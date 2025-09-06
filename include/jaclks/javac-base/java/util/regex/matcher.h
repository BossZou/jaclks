#pragma once

#include "jaclks/javac-base/java/lang/string.h"

namespace jaclks::javac_base {

class RegexImpl;

class Matcher {
 public:
  Matcher(RegexImpl *regex, String input);

  ~Matcher();

  bool Matches();

  String Group(int idx);

 private:
  RegexImpl *regex_;
  String text_;

  class MatcherInner;
  MatcherInner *inner_;
};

}  // namespace jaclks::javac_base
