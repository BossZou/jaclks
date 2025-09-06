#include "jaclks/javac-base/java/util/regex/pattern.h"

#include <boost/regex.hpp>
#include <regex>

namespace jaclks::javac_base {

class Pattern::Regex {
 public:
  explicit Regex(boost::regex regex) : regex_(std::move(regex)) {}

 private:
  boost::regex regex_;
};

Pattern Pattern::Compile(String pattern, int flags) {
  auto regex = new Regex(boost::regex(pattern));

  return {std::move(pattern), flags, regex};
}

Pattern::Pattern(String p, int f, Regex *regex)
    : pattern_(std::move(p)), flags_(f), regex_(regex) {}

}  // namespace jaclks::javac_base
