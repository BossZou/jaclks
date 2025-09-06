#include "jaclks/javac-base/java/lang/string.h"

#ifdef _WIN32
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <utility>
#include <vector>

#include "jaclks/javac-base/java/lang/character.h"

namespace jaclks::javac_base {

String String::Trim(const String &str) {
  auto buf = str.buf_;
  auto len = str.len_;
  std::size_t st = 0;

  while ((st < len) && (buf[st] <= ' ')) {
    st++;
  }
  while ((st < len) && (buf[len - 1] <= ' ')) {
    len--;
  }
  return ((st > 0) || (len < str.len_)) ? String{buf + st, len - st} : String{};
}

String String::Strip(const String &str) {
  auto buf = str.buf_;
  auto len = str.len_;

  for (std::size_t i = 0UL, tlen = len; i < tlen; ++i) {
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

  for (std::size_t i = 0UL, tlen = len; i < tlen; ++i) {
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

std::int64_t String::IndexOf(const String &src,
                             const String &sub,
                             std::size_t from_index) {
  return src.IndexOf(sub, from_index);
}

std::int64_t String::LastIndexOf(const String &src,
                                 const String &sub,
                                 std::size_t from_index) {
  return src.LastIndexOf(sub, from_index);
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
  auto buf = buf_;
  auto len = static_cast<std::int64_t>(len_);
  std::size_t st = 0;

  while ((st < len_) && (buf[st] <= ' ')) {
    ++st;
    --len;
  }
  buf = buf_ + st;
  while ((len > 0) && (buf[len - 1] <= ' ')) {
    --len;
  }

  if (len == 0) {
    *this = {};
  } else {
    if (!own_) {
      if (st + len < len_) {
        *this = String{buf, static_cast<std::size_t>(len)};
      } else {
        buf_ = buf;
        len_ = len;
        info_.cap = len + 1;
      }
    } else {
      buf_ = buf;
      len_ = len;
      buf_[len] = '\0';
    }
  }
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

std::vector<String> String::Split(const String &sub, std::size_t offset) const {
  if (sub.len_ + offset <= len_) {
    return {*this};
  }
  // FIXME(John Doe): Implement it.
  return {};
}

std::int64_t String::IndexOf(char c, std::size_t from_index) const {
  auto max = len_;
  if (from_index >= max) {
    return -1;
  }
  for (auto i = from_index; i < max; i++) {
    if (buf_[i] == c) {
      return i;
    }
  }
  return -1;
}

std::int64_t String::IndexOf(const String &sub, std::size_t from_index) const {
  if (from_index >= len_) {
    return sub.len_ == 0 ? len_ : -1;
  }

  if (sub.len_ == 0) {
    return from_index;
  }

  if (sub.len_ > len_) {
    return -1;
  }

  auto pos = std::strstr(buf_ + from_index, sub.buf_);
  return pos != nullptr ? pos - buf_ : -1;
}

std::int64_t String::LastIndexOf(char c) const {
  return LastIndexOf(c, len_ - 1);
}

std::int64_t String::LastIndexOf(char c, std::size_t from_index) const {
  for (auto idx = std::min(from_index, len_ - 1); idx > 0; --idx) {
    if (buf_[idx] == c) {
      return idx;
    }
  }

  return len_ > 0 && buf_[0] == c ? 0 : -1;
}

std::int64_t String::LastIndexOf(const String &sub) const {
  return LastIndexOf(sub, len_);
}

std::int64_t String::LastIndexOf(const String &sub,
                                 std::size_t from_index) const {
  /*
   * Check arguments; return immediately where possible. For
   * consistency, don't check for null str.
   */
  auto idx = static_cast<std::int64_t>(from_index);
  if (auto right_idx =
          static_cast<std::int64_t>(len_) - static_cast<std::int64_t>(sub.len_);
      idx > right_idx) {
    idx = right_idx;
  }

  if (idx < 0) {
    return -1;
  }
  /* Empty string always matches. */
  if (sub.len_ == 0) {
    return idx;
  }

  auto min = static_cast<std::int64_t>(sub.len_ - 1);
  auto i = min + idx;
  auto str_last_index = static_cast<std::int64_t>(sub.len_ - 1);
  char str_last_char = sub.buf_[str_last_index];

start_search:
  while (true) {
    while (i >= min && buf_[i] != str_last_char) {
      i--;
    }
    if (i < min) {
      return -1;
    }
    auto j = i - 1;
    auto start = j - str_last_index;
    auto k = str_last_index - 1;
    while (j > start) {
      if (buf_[j--] != sub.buf_[k--]) {
        i--;
        goto start_search;
      }
    }
    return start + 1;
  }
}

String String::SubString(std::size_t begin_idx, std::size_t end_idx) const {
  // FIXME(John Doe): Implement it.
  return "";
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

}  // namespace jaclks::javac_base
