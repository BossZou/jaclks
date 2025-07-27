#pragma once

#include <cstddef>

namespace jaclks {

class String {
 public:
  String();

  explicit String(const char *str, bool ref = false);

  ~String();

  std::size_t Length() const;

  std::size_t Capacity() const;

  const char *CStr() const;

 private:
  static constexpr auto kLocalCapacity = 16UL;

  void construct(const char *str, std::size_t len);

  bool is_local_data() const;

  union {
    char local_buf_[kLocalCapacity];
    std::size_t cap_;
  };

  bool own_;
  std::size_t len_;
  char *buf_;
};

}  // namespace jaclks
