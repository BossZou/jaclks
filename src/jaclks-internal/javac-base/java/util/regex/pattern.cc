#include "jaclks/javac-base/java/util/regex/pattern.h"

#include <utility>

#include <boost/regex.hpp>

#include "jaclks-internal/javac-base/java/util/regex/regex_impl.h"

namespace jaclks::javac_base {

Pattern Pattern::Compile(const String &pattern, int flags) {
  static_assert(kNormal == boost::regex::normal,
                "Normal option is not equal regex one");

  boost::regex_constants::syntax_option_type options = boost::regex::normal;

  if (flags & kCaseInsensitive) {
    options |= boost::regex::icase;
  }
  if (flags & kLiteral) {
    options |= boost::regex::literal;
  }
  if (flags & kMultiline) {
    options |= boost::regex::newline_alt;
  }
  if (flags & kDotall) {
    options |= boost::regex::mod_s;
  }

  auto regex = new RegexImpl(pattern.CStr(), options);

  return {pattern, flags, regex};
}

bool Pattern::Matches(const String &regex, const String &input) {
  auto pattern = Compile(regex);

  return pattern.Matcher(input).Matches();
}

Pattern::Pattern(String p, int f, RegexImpl *regex)
    : pattern_(std::move(p)), flags_(f), regex_(regex) {}

Pattern::~Pattern() {
  delete regex_;
  regex_ = nullptr;
}

::jaclks::javac_base::Matcher Pattern::Matcher(String input) {
  return {pattern_, regex_, std::move(input)};
}

std::vector<String> Pattern::Split(const String &input, int limit) const {
  std::string si{input.CStr()};
  std::vector<String> parts;
  boost::sregex_token_iterator it(si.begin(), si.end(), regex_->Regex(), -1);
  boost::sregex_token_iterator end;

  // FIXME(John Doe): Implement correct logic. reference to:
  // https://www.runoob.com/manual/jdk1.6/java.base/java/lang/String.html#split(java.lang.String,int)
  int count = 0;
  while (it != end) {
    if (limit > 0 && count >= limit) {
      break;
    }
    parts.emplace_back(it->str().c_str());
    ++it;
    ++count;
  }

  return parts;
}

int Pattern::Flags() const {
  return flags_;
}

}  // namespace jaclks::javac_base
