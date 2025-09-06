#include "jaclks/javac-base/java/util/regex/pattern.h"

#include <utility>

#include <boost/regex.hpp>

#include "jaclks-internal/javac-base/java/util/regex/regex_impl.h"

namespace jaclks::javac_base {

Pattern Pattern::Compile(String pattern, int flags) {
  boost::regex_constants::syntax_option_type options = boost::regex::normal;

  if (flags | kCaseInsensitive) {
    options |= boost::regex::icase;
  }
  if (flags | kLiteral) {
    options |= boost::regex::literal;
  }
  if (flags | kMultiline) {
    options |= boost::regex::newline_alt;
  }
  if (flags & kDotall) {
    options |= boost::regex::mod_s;
  }

  auto boost_regex = boost::regex(pattern.CStr(), options);
  auto regex = new RegexImpl(boost_regex);

  std::string str = "input";
  boost::smatch matches;
  boost::regex_search(str, matches, boost_regex);

  return {std::move(pattern), flags, regex};
}

bool Pattern::Matches(String regex, String input) {
  auto pattern = Compile(std::move(regex));
  return false;
}

Pattern::Pattern(String p, int f, RegexImpl *regex)
    : pattern_(std::move(p)), flags_(f), regex_(regex) {}

Pattern::~Pattern() {
  if (regex_) {
    delete regex_;
    regex_ = nullptr;
  }
}

::jaclks::javac_base::Matcher Pattern::Matcher(String input) {
  return {regex_, std::move(input)};
}

}  // namespace jaclks::javac_base
