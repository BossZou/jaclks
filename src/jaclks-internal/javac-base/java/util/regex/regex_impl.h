#pragma once

#include <boost/regex.hpp>

namespace jaclks::javac_base {

class RegexImpl {
 public:
  explicit RegexImpl(boost::regex regex) : regex_(std::move(regex)) {}

  // private:
  friend class Matcher;

  boost::regex regex_;
};

}  // namespace jaclks::javac_base
