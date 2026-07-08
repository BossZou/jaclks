#pragma once

#include <utility>

namespace jaclks::javac_base {

template <typename K, typename V>
class Entry {
 public:
  Entry() = default;

  Entry(const K& k, const V& v) : key_(k), value_(v) {}

  Entry(K&& k, V&& v) : key_(std::move(k)), value_(std::move(v)) {}

  const K& Key() const { return key_; }

  V& Value() { return value_; }
  const V& Value() const { return value_; }

  void SetValue(const V& v) { value_ = v; }

  bool operator==(const Entry& other) const {
    return key_ == other.key_ && value_ == other.value_;
  }

 private:
  K key_{};
  V value_{};
};

}  // namespace jaclks::javac_base
