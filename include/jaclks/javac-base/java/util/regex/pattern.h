#pragma once

#include <vector>

#include "jaclks/javac-base/java/lang/string.h"
#include "jaclks/javac-base/java/util/regex/matcher.h"

namespace jaclks::javac_base {

class RegexImpl;

class Pattern {
 public:
  static constexpr int kUnixLines = 0x01;
  static constexpr int kCaseInsensitive = 0x02;
  static constexpr int kComments = 0x04;
  static constexpr int kMultiline = 0x08;
  static constexpr int kLiteral = 0x10;
  static constexpr int kDotall = 0x20;
  static constexpr int kUnicodeCase = 0x40;
  static constexpr int kCanonEq = 0x80;
  static constexpr int kUnicodeCharacterClass = 0x100;

  static constexpr int kAllFlags =
      kUnixLines | kCaseInsensitive | kComments | kMultiline | kLiteral |
      kDotall | kUnicodeCase | kCanonEq | kUnicodeCharacterClass;

  static Pattern Compile(const String &regex, int flags = 0);

  static bool Matches(const String &regex, const String &input);

  ~Pattern();

  ::jaclks::javac_base::Matcher Matcher(const String &input);

  std::vector<String> Split(const String &input, int limit = 0) const;

  int Flags() const;

 private:
  Pattern(String p, int f, RegexImpl *regex);

  String pattern_;
  int flags_;

  RegexImpl *regex_;
};

}  // namespace jaclks::javac_base
