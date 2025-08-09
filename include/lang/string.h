#pragma once

#include <cstddef>

namespace jaclks {

class String {
 public:
  static String StripTrailing(const String &str);

  String();

  explicit String(const char *str, bool ref = false);

  explicit String(const char *str, std::size_t len);

  ~String();

  bool StartsWith(const String &sub, std::size_t offset = 0) const;

  bool EndsWith(const String &suffix) const;

  void Strip();

  /**
   * Removes all trailing white space.
   */
  void StripTrailing();

  std::size_t Length() const;

  std::size_t Capacity() const;

  const char *CStr() const;

  void Reset();

  bool IsRef() const;

  // FIXME
  bool operator==(const String &other) const;

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
