#include "lang/string.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <utility>

#include "lang/character.h"

namespace jaclks {

String String::Trim(const String &str) {
  auto buf = str.buf_;
  auto len = str.len_;
  std::size_t st = 0;

  while ((st < len) && ((buf[st] & 0xff) <= ' ')) {
    st++;
  }
  while ((st < len) && ((buf[len - 1] & 0xff) <= ' ')) {
    len--;
  }
  return ((st > 0) || (len < str.len_)) ? String{buf + st, len - st} : String{};
}

String String::Strip(const String &str) {
  auto buf = str.buf_;
  auto len = str.len_;

  for (auto i = 0UL, tlen = len; i < tlen; ++i) {
    if (!Character::IsWhitespace(str.buf_[i])) {
      break;
    }
    ++buf;
    --len;
  }

  for (auto i = static_cast<ssize_t>(len - 1); i >= 0; --i) {
    if (!Character::IsWhitespace(buf[i])) {
      break;
    }
    --len;
  }

  return String{buf, len};
}

String String::StripLeading(const String &str) {
  auto buf = str.buf_;
  auto len = str.len_;

  for (auto i = 0UL, tlen = len; i < tlen; ++i) {
    if (!Character::IsWhitespace(str.buf_[i])) {
      break;
    }
    ++buf;
    --len;
  }

  return String{buf, len};
}

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

String::String() : info_(), own_(true), len_(0), buf_(local_buf_) {}

String::String(const char *str, bool ref) : String() {
  auto len = strlen(str);
  if (ref) {
    own_ = false;
    len_ = len;
    info_.cap = len + 1;
    buf_ = const_cast<char *>(str);
  } else {
    construct(str, len);
  }
}

String::String(const char *str, std::size_t len) : String() {
  construct(str, len);
}

String::String(const String &other) : String() {
  if (!other.own_) {
    own_ = other.own_;
    len_ = other.len_;
    info_.cap = other.info_.cap;
    buf_ = other.buf_;
  } else {
    construct(other.buf_, other.len_);
  }
}

String::String(String &&other) noexcept
    : info_(), own_(other.own_), len_(other.len_), buf_(other.buf_) {
  if (other.is_local_data()) {
    std::memcpy(local_buf_, other.local_buf_, kLocalCapacity);
    buf_ = local_buf_ + (other.buf_ - other.local_buf_);
  }
  other.own_ = false;
  other.len_ = 0;
  other.buf_ = nullptr;
  other.info_ = {};
}

String::~String() {
  if (!is_local_data() && own_) {
    free(info_.head);
  }
}

String &String::operator=(const String &other) {
  if (this != &other) {
    this->~String();

    // TODO(John Doe): Optimize here to reduce strlen() call.
    new (this) String(other.buf_, !other.own_);
  }
  return *this;
}

String &String::operator=(String &&other) noexcept {
  if (this != &other) {
    this->~String();
    new (this) String(std::move(other));
  }
  return *this;
}

bool String::StartsWith(const String &sub, std::size_t offset) const {
  if (sub.len_ + offset <= len_) {
    return std::memcmp(buf_ + offset, sub.buf_, sub.len_) == 0;
  }

  return false;
}

bool String::EndsWith(const String &suffix) const {
  if (suffix.len_ <= len_) {
    return StartsWith(suffix, len_ - suffix.len_);
  }
  return false;
}

void String::Trim() {
  // FIXME(John Doe): Implement it. Consider difference from Strip().
}

void String::Strip() {
  StripLeading();
  StripTrailing();
}

void String::StripLeading() {
  while (Character::IsWhitespace(*buf_)) {
    ++buf_;
    --len_;
  }
}

void String::StripTrailing() {
  std::size_t space_len = 0UL;

  for (auto i = static_cast<ssize_t>(len_ - 1); i >= 0; --i) {
    if (!Character::IsWhitespace(buf_[i])) {
      break;
    }
    ++space_len;
  }

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
  return is_local_data() ? kLocalCapacity : info_.cap;
}

const char *String::CStr() const {
  return buf_;
}

void String::Reset() {
  if (!is_local_data() && own_) {
    free(buf_);
  }

  len_ = 0UL;
  own_ = false;
  buf_ = local_buf_;
  buf_[0] = '\0';
}

bool String::IsRef() const {
  return !own_;
}

bool String::operator==(const String &other) const {
  return std::strncmp(buf_, other.buf_, Length()) == 0;
}

void String::construct(const char *str, std::size_t len) {
  own_ = true;
  len_ = len;

  if (len < kLocalCapacity) {
    memcpy(local_buf_, str, len);
    local_buf_[len] = '\0';
    buf_ = local_buf_;
  } else {
    buf_ = static_cast<char *>(malloc(len + 1));
    memcpy(buf_, str, len);
    buf_[len] = '\0';
    info_.cap = len + 1;
    info_.head = buf_;
  }
}

bool String::is_local_data() const {
  auto buf_ptr = reinterpret_cast<uintptr_t>(buf_);
  auto local_ptr = reinterpret_cast<uintptr_t>(local_buf_);

  return buf_ptr >= local_ptr && buf_ptr < local_ptr + kLocalCapacity;
}

}  // namespace jaclks
