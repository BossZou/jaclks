#pragma once

#include "jaclks/javac-base/java/lang/string.h"

namespace jaclks::javac_base {

class RegexImpl;

class Matcher {
 public:
  Matcher(String pattern, RegexImpl *regex, String input);

  ~Matcher();

  bool Matches();

  bool LookingAt();

  bool Find();

  std::size_t GroupCount() const;

  String Group(std::size_t idx = 0);

  String Group(const String &group);

  Matcher &Region(std::size_t start, std::size_t end);

 private:
  String pattern_;
  RegexImpl *regex_;
  String text_;

  struct MatcherInner;
  MatcherInner *inner_;
};

}  // namespace jaclks::javac_base
