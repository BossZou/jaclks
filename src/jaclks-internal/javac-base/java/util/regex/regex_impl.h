#pragma once

#include <boost/regex.hpp>

namespace jaclks::javac_base {

class RegexImpl {
 public:
  RegexImpl(const char *pattern,
            boost::regex_constants::syntax_option_type options)
      : regex_(pattern, options) {}

  boost::regex &Regex();

 private:
  boost::regex regex_;
};

}  // namespace jaclks::javac_base
