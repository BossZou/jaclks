#include "lang/string.h"

#include <cstdlib>
#include <cstring>
#include <new>

#include "lang/character.h"

namespace jaclks {

String String::StripTrailing(const String &str) {
  auto len = str.Length();

  for (auto i = static_cast<ssize_t>(len - 1); i >= 0; --i) {
    if (!Character::IsWhitespace(str.buf_[i])) {
      break;
    }
    --len;
  }

  return String{str.buf_, len};
}

String::String() : cap_(0), own_(true), len_(0), buf_(local_buf_) {}

String::String(const char *str, bool ref) : String() {
  auto len = strlen(str);
  if (ref) {
    own_ = false;
    len_ = len;
    cap_ = len + 1;
    buf_ = const_cast<char *>(str);
  } else {
    construct(str, len);
  }
}

String::String(const char *str, std::size_t len) : String() {
  construct(str, len);
}

String::~String() {
  if (!is_local_data() && own_) {
    free(buf_);
  }
}

bool String::StartsWith(const String &sub, std::size_t offset) const {
  if (sub.len_ + offset <= len_) {
    return memcmp(buf_ + offset, sub.buf_, sub.len_) == 0;
  }

  return false;
}

bool String::EndsWith(const String &suffix) const {
  if (suffix.len_ <= len_) {
    return StartsWith(suffix, len_ - suffix.len_);
  }
  return false;
}

void String::Strip() {
  // TODO(BossZou): Optimize to reduce data copy by move header ptr
  // FIXME(BossZou): Use Character
}

void String::StripTrailing() {
  std::size_t space_len = 0UL;

  for (auto i = static_cast<ssize_t>(len_ - 1); i >= 0; --i) {
    if (!Character::IsWhitespace(buf_[i])) {
      break;
    }
    ++space_len;
  }

  // FIXME(guanfeng): Tell if is ref mode.
  if (own_) {
    len_ -= space_len;
    buf_[len_] = '\0';
  } else {
    auto len = len_;
    auto buf = buf_;

    this->~String();

    new (this) String(static_cast<const char *>(buf), len - space_len);
  }
}

std::size_t String::Length() const {
  return len_;
}

std::size_t String::Capacity() const {
  return is_local_data() ? kLocalCapacity : cap_;
}

const char *String::CStr() const {
  return buf_;
}

void String::Reset() {
  if (!is_local_data() && own_) {
    free(buf_);
  }

  buf_ = nullptr;
  len_ = 0UL;
  cap_ = 0UL;
  own_ = false;
}

bool String::IsRef() const {
  return !own_;
}

bool String::operator==(const String &other) const {
  return std::strncmp(buf_, other.buf_, Length()) == 0;
}

void String::construct(const char *str, std::size_t len) {
  if (len < kLocalCapacity) {
    memcpy(local_buf_, str, len);
    local_buf_[len] = '\0';
    own_ = true;
    len_ = len;
    buf_ = local_buf_;
  } else {
    buf_ = static_cast<char *>(malloc(len + 1));
    memcpy(buf_, str, len);
    buf_[len] = '\0';
    own_ = true;
    len_ = len;
    cap_ = len + 1;
  }
}

bool String::is_local_data() const {
  return buf_ == local_buf_;
}

}  // namespace jaclks
