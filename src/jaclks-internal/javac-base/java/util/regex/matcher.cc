#include "jaclks/javac-base/java/util/regex/matcher.h"

#include <boost/regex.hpp>

#include "jaclks-internal/javac-base/java/util/regex/regex_impl.h"
#include "jaclks/javac-base/java/lang/index_out_of_bounds_exception.h"

namespace jaclks::javac_base {

struct Matcher::MatcherInner {
  MatcherInner(const char *input, std::size_t len)
      : input_(input),
        len_(len),
        what_(),
        begin_(nullptr),
        end_(input_ + len) {}

  void Reset() {
    begin_ = nullptr;
    end_ = input_ + len_;
  }

  bool Found() const {
    return begin_ != nullptr;
  }

  [[nodiscard]] const char *RegexBegin() const {
    return begin_ == nullptr ? input_ : begin_;
  }

  const char *input_;
  const std::size_t len_;
  boost::cmatch what_;
  const char *begin_;
  const char *end_;
};

Matcher::Matcher(RegexImpl *regex, String input)
    : regex_(regex), text_(std::move(input)), inner_(nullptr) {
  inner_ = new MatcherInner(text_.CStr(), text_.Length());
}

Matcher::~Matcher() {
  delete inner_;
  inner_ = nullptr;
}

bool Matcher::Matches() {
  if (const auto begin = inner_->RegexBegin();
      boost::regex_match(begin, inner_->end_, inner_->what_, regex_->Regex())) {
    inner_->begin_ = begin;
    return true;
  } else {
    inner_->begin_ = nullptr;
    return false;
  }
}

bool Matcher::LookingAt() {
  if (const auto begin = inner_->RegexBegin();
      boost::regex_search(begin,
                          inner_->end_,
                          inner_->what_,
                          regex_->Regex(),
                          boost::match_continuous)) {
    inner_->begin_ = begin;
    return true;
  } else {
    inner_->begin_ = nullptr;
    return false;
  }
}

bool Matcher::Find() {
  if (const auto begin = inner_->RegexBegin(); boost::regex_search(
          begin, inner_->end_, inner_->what_, regex_->Regex())) {
    inner_->begin_ = begin;
    return true;
  } else {
    inner_->begin_ = nullptr;
    return false;
  }
}

std::size_t Matcher::GroupCount() const {
  return inner_->Found() ? inner_->what_.size() - 1 : 0;
}

String Matcher::Group(std::size_t idx) {
  if (idx >= GroupCount()) {
    throw IndexOutOfBoundsException("Group index out of range");
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
