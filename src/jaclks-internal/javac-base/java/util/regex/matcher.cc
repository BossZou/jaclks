#include "jaclks/javac-base/java/util/regex/matcher.h"

#include <boost/regex.hpp>

#include "jaclks-internal/javac-base/java/util/regex/regex_impl.h"

namespace jaclks::javac_base {

class Matcher::MatcherInner {
 public:
  MatcherInner(boost::smatch what) : what_(what) {}

  boost::smatch what_;
};

Matcher::Matcher(RegexImpl *regex, String input)
    : regex_(regex), text_(std::move(input)), inner_(nullptr) {
  boost::smatch what;

  boost::regex_search(std::string{text_.CStr()}, what, regex_->regex_);
  inner_ = new MatcherInner(std::move(what));
}

Matcher::~Matcher() {
  if (inner_) {
    delete inner_;
    inner_ = nullptr;
  }
}

bool Matcher::Matches() {
  return boost::regex_match(text_.CStr(), regex_->regex_);
}

String Matcher::Group(int idx) {
  if (static_cast<std::size_t>(idx) > inner_->what_.size()) {
    throw std::invalid_argument("Group index out of range");
  }

  const auto &group = inner_->what_[idx];
  return String{group.str().c_str(), false};
}

}  // namespace jaclks::javac_base
