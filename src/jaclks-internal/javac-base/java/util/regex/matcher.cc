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
      : pattern_(pattern, pattern_len),
        input_(input, len),
        from_(input_.begin()), to_(input_.end()),
        first_(input_.end()), last_(input_.end()) {
    boost::regex named_group_pattern(kGroupPattern);
    boost::smatch matches;

    int group_count = 1;
    for (auto begin = pattern_.begin(), end = pattern_.end();
         boost::regex_search(begin, end, matches, named_group_pattern);) {
      String group_name{matches[1].str().c_str()};
      named_groups_.emplace(group_name, group_count);

      begin = matches[0].second;
      group_count++;
    }
  }

  void Reset() {
    from_ = input_.begin();
    to_ = input_.end();
    first_ = input_.end();
    last_ = input_.end();
    // named_groups_.clear();
  }

  bool Found() const {
    return first_ != input_.end();
  }

  auto RegexBegin() const {
    return first_ == input_.end() ? input_.begin() : last_;
  }

  static constexpr const char *kGroupPattern =
      "\\(\\?<([a-zA-Z][a-zA-Z0-9_]*)>";

  const std::string pattern_;
  std::string input_;
  boost::smatch what_;
  std::string::const_iterator from_;
  std::string::const_iterator to_;
  std::string::const_iterator first_;
  std::string::const_iterator last_;
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
  if (boost::regex_match(inner_->from_, inner_->to_, inner_->what_, regex_->Regex())) {
    inner_->first_ = inner_->what_[0].begin();
    inner_->last_ = inner_->what_[0].end();
    return true;
  } else {
    inner_->first_ = inner_->input_.end();
    return false;
  }
}

bool Matcher::LookingAt() {
  if (boost::regex_search(inner_->from_,
                          inner_->to_,
                          inner_->what_,
                          regex_->Regex(),
                          boost::match_continuous)) {
    inner_->first_ = inner_->what_[0].begin();
    inner_->last_ = inner_->what_[0].end();
    return true;
  } else {
    inner_->first_ = inner_->input_.end();
    return false;
  }
}

bool Matcher::Find() {
  if (const auto begin = inner_->RegexBegin(); boost::regex_search(
          begin, inner_->to_, inner_->what_, regex_->Regex())) {
    inner_->first_ = inner_->what_[0].begin();
    inner_->last_ = inner_->what_[0].end();
    return true;
  } else {
    inner_->first_ = inner_->input_.end();
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

Matcher& Matcher::Region(std::size_t start, std::size_t end) {
  if (start > inner_->input_.size()) {
    throw IndexOutOfBoundsException("Start index out of range");
  }
  if (end > inner_->input_.size()) {
    throw IndexOutOfBoundsException("End index out of range");
  }

  inner_->Reset();
  inner_->from_ = inner_->input_.begin() + start;
  inner_->to_ = inner_->input_.begin() + end;
  return *this;
}

}  // namespace jaclks::javac_base
