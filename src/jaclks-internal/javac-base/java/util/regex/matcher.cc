#include "jaclks/javac-base/java/util/regex/matcher.h"

#include <string>

#include <boost/regex.hpp>

#include "jaclks-internal/javac-base/java/util/regex/regex_impl.h"
#include "jaclks/javac-base/java/lang/illegal_argument_exception.h"
#include "jaclks/javac-base/java/lang/illegal_state_exception.h"
#include "jaclks/javac-base/java/lang/index_out_of_bounds_exception.h"

namespace jaclks::javac_base {

struct Matcher::MatcherInner {
  MatcherInner(const char *pattern,
               std::size_t pattern_len,
               const char *input,
               std::size_t len)
      : pattern_(pattern),
        pattern_len_(pattern_len),
        input_(input),
        len_(len),
        what_(),
        begin_(nullptr),
        end_(input_ + len) {
    boost::regex named_group_pattern(kGroupPattern);
    boost::cmatch matches;

    int group_count = 1;
    for (auto begin = pattern_, end = pattern_ + pattern_len_;
         boost::regex_search(begin, end, matches, named_group_pattern);) {
      String group_name{matches[1].str().c_str()};
      named_groups_.emplace(group_name, group_count);

      begin = matches[0].second;
      group_count++;
    }
  }

  void Reset() {
    begin_ = nullptr;
    end_ = input_ + len_;
    named_groups_.clear();
  }

  bool Found() const {
    return begin_ != nullptr;
  }

  [[nodiscard]] const char *RegexBegin() const {
    return begin_ == nullptr ? input_ : begin_;
  }

  static constexpr const char *kGroupPattern =
      "\\(\\?<([a-zA-Z][a-zA-Z0-9_]*)>";

  const char *pattern_;
  std::size_t pattern_len_;
  const char *input_;
  const std::size_t len_;
  boost::cmatch what_;
  const char *begin_;
  const char *end_;
  std::map<String, int> named_groups_;
};

Matcher::Matcher(String pattern, RegexImpl *regex, String input)
    : pattern_(std::move(pattern)),
      regex_(regex),
      text_(std::move(input)),
      inner_(nullptr) {
  inner_ = new MatcherInner(
      pattern_.CStr(), pattern_.Length(), text_.CStr(), text_.Length());
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
  if (!inner_->Found()) {
    throw IllegalStateException("No match found");
  }

  if (idx > GroupCount()) {
    // TODO(John Doe): Add idx information to exception error
    throw IndexOutOfBoundsException("Group index out of range");
  }

  const auto &group = inner_->what_[idx];
  return {group.str().c_str()};
}

String Matcher::Group(const String &group) {
  if (!inner_->Found()) {
    throw IllegalStateException("No match found");
  }

  if (const auto it = inner_->named_groups_.find(group);
      it == inner_->named_groups_.end()) {
    // TODO(John Doe): Add group name
    throw IllegalArgumentException("No group with name <group>");
  } else {
    auto idx = it->second;
    return {inner_->what_[idx].str().c_str()};
  }
}

}  // namespace jaclks::javac_base
