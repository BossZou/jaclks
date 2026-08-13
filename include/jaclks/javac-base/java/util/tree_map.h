#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <optional>
#include <type_traits>
#include <utility>

#include "jaclks/function/compare.h"
#include "jaclks/javac-base/java/util/navigable_map.h"
#include "jaclks/javac-base/java/util/no_such_element_exception.h"

namespace jaclks::javac_base {

template <typename K,
          typename V,
          typename Compare = jaclks::Comparator<K>>
class TreeMap : public NavigableMap<K, V, Compare> {
 public:
  using Entry = typename Map<K, V>::Entry;

  explicit TreeMap(Compare comp = Compare{})
      : compare_adaptor_{comp}, compare_(comp), map_(compare_adaptor_) {}
  ~TreeMap() override = default;

  // ==================== Map<K,V> methods ====================

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

  // ==================== SortedMap<K,V> methods ====================

  const Compare& comparator() const override { return compare_; }

  K FirstKey() const override {
    if (map_.empty()) {
      throw NoSuchElementException("Map is empty");
    }
    return map_.begin()->first;
  }

  K LastKey() const override {
    if (map_.empty()) {
      throw NoSuchElementException("Map is empty");
    }
    return map_.rbegin()->first;
  }

  // ==================== NavigableMap<K,V> methods ====================

  std::optional<Entry> LowerEntry(const K& key) const override {
    auto it = map_.lower_bound(key);
    if (it == map_.begin()) {
      return std::nullopt;
    }
    --it;
    return Entry(it->first, it->second);
  }

  std::optional<Entry> FloorEntry(const K& key) const override {
    auto it = map_.lower_bound(key);
    if (it != map_.end() && KeyEquals(it->first, key)) {
      return Entry(it->first, it->second);
    }
    if (it == map_.begin()) {
      return std::nullopt;
    }
    --it;
    return Entry(it->first, it->second);
  }

  std::optional<Entry> CeilingEntry(const K& key) const override {
    auto it = map_.lower_bound(key);
    if (it == map_.end()) {
      return std::nullopt;
    }
    return Entry(it->first, it->second);
  }

  std::optional<Entry> HigherEntry(const K& key) const override {
    auto it = map_.upper_bound(key);
    if (it == map_.end()) {
      return std::nullopt;
    }
    return Entry(it->first, it->second);
  }

  std::optional<Entry> FirstEntry() const override {
    if (map_.empty()) {
      return std::nullopt;
    }
    auto it = map_.begin();
    return Entry(it->first, it->second);
  }

  std::optional<Entry> LastEntry() const override {
    if (map_.empty()) {
      return std::nullopt;
    }
    auto it = map_.rbegin();
    return Entry(it->first, it->second);
  }

  std::optional<Entry> PollFirstEntry() override {
    if (map_.empty()) {
      return std::nullopt;
    }
    auto it = map_.begin();
    Entry entry(it->first, it->second);
    map_.erase(it);
    return entry;
  }

  std::optional<Entry> PollLastEntry() override {
    if (map_.empty()) {
      return std::nullopt;
    }
    auto it = std::prev(map_.end());
    Entry entry(it->first, it->second);
    map_.erase(it);
    return entry;
  }

  // ==================== View methods ====================

  TreeMap HeadMap(const K& toKey) const override {
    return SubMapImpl(map_.begin(), map_.lower_bound(toKey));
  }

  TreeMap HeadMap(const K& toKey, bool inclusive) const override {
    auto end = inclusive ? map_.upper_bound(toKey) : map_.lower_bound(toKey);
    return SubMapImpl(map_.begin(), end);
  }

  TreeMap TailMap(const K& fromKey) const override {
    return SubMapImpl(map_.lower_bound(fromKey), map_.end());
  }

  TreeMap TailMap(const K& fromKey, bool inclusive) const override {
    auto begin =
        inclusive ? map_.lower_bound(fromKey) : map_.upper_bound(fromKey);
    return SubMapImpl(begin, map_.end());
  }

  TreeMap SubMap(const K& fromKey, const K& toKey) const override {
    return SubMapImpl(map_.lower_bound(fromKey), map_.lower_bound(toKey));
  }

  TreeMap SubMap(const K& fromKey,
                 bool fromInclusive,
                 const K& toKey,
                 bool toInclusive) const override {
    auto begin = fromInclusive ? map_.lower_bound(fromKey)
                                : map_.upper_bound(fromKey);
    auto end =
        toInclusive ? map_.upper_bound(toKey) : map_.lower_bound(toKey);
    return SubMapImpl(begin, end);
  }

  // Non-virtual: removed from NavigableMap interface to avoid infinite
  // template recursion in vtable generation (DescendingMap returns a
  // TreeMap with a different comparator, which itself has DescendingMap).
  TreeMap<K, V, ReversedComparator<Compare>> DescendingMap() const {
    TreeMap<K, V, ReversedComparator<Compare>> result(
        ReversedComparator<Compare>{compare_});
    for (const auto& pair : map_) {
      result.Put(pair.first, pair.second);
    }
    return result;
  }

 protected:
  void ForEachEntry(
      std::function<void(const K&, const V&)> action) const override {
    for (const auto& pair : map_) {
      action(pair.first, pair.second);
    }
  }

 private:
  struct CompareAdaptor {
    bool operator()(const K& a, const K& b) const {
      if constexpr (std::is_same_v<decltype(compare(a, b)), bool>) {
        return compare(a, b);
      } else {
        return compare(a, b) < 0;
      }
    }
    Compare compare;
  };

  // Detects key equality for both three-way (int) and boolean comparators.
  bool KeyEquals(const K& a, const K& b) const {
    if constexpr (std::is_same_v<decltype(compare_(a, b)), bool>) {
      return !compare_(a, b) && !compare_(b, a);
    } else {
      return compare_(a, b) == 0;
    }
  }

  TreeMap SubMapImpl(
      typename std::map<K, V, CompareAdaptor>::const_iterator begin,
      typename std::map<K, V, CompareAdaptor>::const_iterator end) const {
    TreeMap result(compare_);
    for (auto it = begin; it != end; ++it) {
      result.map_.emplace(it->first, it->second);
    }
    return result;
  }

  CompareAdaptor compare_adaptor_;
  Compare compare_;
  std::map<K, V, CompareAdaptor> map_;
};

}  // namespace jaclks::javac_base
