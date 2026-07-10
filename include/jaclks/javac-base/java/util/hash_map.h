#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <unordered_map>

#include "jaclks/javac-base/java/util/map.h"

namespace jaclks::javac_base {

template <typename K,
          typename V,
          typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
class HashMap : public Map<K, V> {
 public:
  HashMap() = default;
  ~HashMap() override = default;

  std::size_t Size() const override { return map_.size(); }

  bool ContainsKey(const K& key) const override {
    return map_.find(key) != map_.end();
  }

  bool ContainsValue(const V& value) const override {
    for (const auto& pair : map_) {
      if (pair.second == value) {
        return true;
      }
    }
    return false;
  }

  std::optional<V> Get(const K& key) const override {
    auto it = map_.find(key);
    if (it != map_.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  std::optional<V> Put(const K& key, const V& value) override {
    auto it = map_.find(key);
    if (it != map_.end()) {
      V old = it->second;
      it->second = value;
      return std::optional<V>(old);
    }
    map_.emplace(key, value);
    return std::nullopt;
  }

  std::optional<V> Remove(const K& key) override {
    auto it = map_.find(key);
    if (it != map_.end()) {
      V removed = it->second;
      map_.erase(it);
      return std::optional<V>(removed);
    }
    return std::nullopt;
  }

  void Clear() override { map_.clear(); }

 protected:
  void ForEachEntry(
      std::function<void(const K&, const V&)> action) const override {
    for (const auto& pair : map_) {
      action(pair.first, pair.second);
    }
  }

 private:
  std::unordered_map<K, V, Hash, KeyEqual> map_;
};

}  // namespace jaclks::javac_base
