#include "jaclks/javac-base/java/util/regex/matcher.h"

#include <boost/regex.hpp>

#include "jaclks-internal/javac-base/java/util/regex/regex_impl.h"

namespace jaclks::javac_base {

class Matcher::MatcherInner {
 public:
  explicit MatcherInner(RegexImpl *regex, String input)
      : find_(false), input_(std::move(input)) {
    find_ = boost::regex_search(input_.CStr(), what_, regex->Regex());
  }

  bool find_;
  String input_;
  boost::cmatch what_;
};

Matcher::Matcher(RegexImpl *regex, String input)
    : regex_(regex), text_(std::move(input)), inner_(nullptr) {
  inner_ = new MatcherInner(regex, text_);
}

Matcher::~Matcher() {
  if (inner_) {
    delete inner_;
    inner_ = nullptr;
  }
}

bool Matcher::Matches() {
  return boost::regex_match(text_.CStr(), regex_->Regex());
}

bool Matcher::LookingAt() {
  return boost::regex_search(
      text_.CStr(), regex_->Regex(), boost::match_continuous);
}

bool Matcher::Find() {
  return inner_->find_;
}

String Matcher::Group(int idx) {
  if (static_cast<std::size_t>(idx) > inner_->what_.size()) {
    throw std::invalid_argument("Group index out of range");
  }

  const auto &group = inner_->what_[idx];
  return String{group.str().c_str(), false};
}

String Matcher::Group(const String &group) {
  if (Find()) {
    if (const auto &ref = inner_->what_[group.CStr()]; ref.matched) {
      return String{ref.str().c_str()};
    }
  }

  return {};
}

}  // namespace jaclks::javac_base
