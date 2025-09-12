#pragma once

#include "jaclks/javac-base/java/lang/string.h"

namespace jaclks::javac_base {

class RegexImpl;

class Matcher {
 public:
  Matcher(RegexImpl *regex, String input);

  ~Matcher();

  bool Matches();

  bool LookingAt();

  bool Find();

  String Group(int idx);

  String Group(const String &group);

 private:
  RegexImpl *regex_;
  String text_;

  class MatcherInner;
  MatcherInner *inner_;
};

}  // namespace jaclks::javac_base
