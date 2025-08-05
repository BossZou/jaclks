#include "string/string.h"

#include <cstdlib>
#include <cstring>

namespace jaclks {

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

String::~String() {
  if (!is_local_data() && own_) {
    free(buf_);
  }
}

bool String::StartWith(const String &sub, std::size_t offset) const {
  if (sub.len_ + offset <= len_) {
    return memcmp(buf_ + offset, sub.buf_, len_ - offset) == 0;
  }

  return false;
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
