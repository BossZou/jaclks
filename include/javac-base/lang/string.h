#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace jaclks {

class String {
 public:
  static String Trim(const String &str);

  /**
   * Returns a string whose value is this string, with all leading
   * and trailing {@link Character#isWhitespace(int) white space}
   * removed.
   * <p>
   * If this {@code String} object represents an empty string,
   * or if all code points in this string are
   * {@link Character#isWhitespace(int) white space}, then an empty string
   * is returned.
   * <p>
   * Otherwise, returns a substring of this string beginning with the first
   * code point that is not a {@link Character#isWhitespace(int) white space}
   * up to and including the last code point that is not a
   * {@link Character#isWhitespace(int) white space}.
   * <p>
   * This method may be used to strip
   * {@link Character#isWhitespace(int) white space} from
   * the beginning and end of a string.
   *
   * @return  a string whose value is this string, with all leading
   *          and trailing white space removed
   *
   * @see Character#isWhitespace(int)
   *
   * @since 11
   */
  static String Strip(const String &str);

  static String StripLeading(const String &str);

  static String StripTrailing(const String &str);

  String();

  explicit String(const char *str, bool ref = false);

  explicit String(const char *str, std::size_t len);

  String(const String &other);

  String(String &&other) noexcept;

  ~String();

  String &operator=(const String &other);

  String &operator=(String &&other) noexcept;

  bool StartsWith(const String &sub, std::size_t offset = 0) const;

  bool EndsWith(const String &suffix) const;

  void Trim();

  void Strip();

  void StripLeading();

  /**
   * Removes all trailing white space.
   */
  void StripTrailing();

  std::vector<String> Split(const String &sub, std::size_t offset = 0) const;

  std::int64_t IndexOf(char c, std::size_t from_index = 0) const;

  std::int64_t LastIndexOf(char c) const;

  std::int64_t LastIndexOf(char c, std::size_t from_index) const;

  std::size_t Length() const;

  std::size_t Capacity() const;

  const char *CStr() const;

  void Reset();

  bool IsRef() const;

  // FIXME
  bool operator==(const String &other) const;

 private:
  struct InternalInfo {
    InternalInfo() : cap(0), head(nullptr) {}

    std::size_t cap;
    char *head;
  };

  static constexpr auto kLocalCapacity = 16UL;

  static_assert(kLocalCapacity >= sizeof(InternalInfo),
                "Local capacity must be larger than Info");

  void construct(const char *str, std::size_t len);

  bool is_local_data() const;

  union {
    char local_buf_[kLocalCapacity];
    InternalInfo info_;
  };

  bool own_;
  std::size_t len_;
  char *buf_;
};

}  // namespace jaclks
