#pragma once

#include <optional>

#include "jaclks/javac-base/java/util/sorted_map.h"

namespace jaclks::javac_base {

template <typename K,
          typename V,
          typename Compare = jaclks::Comparator<K>>
class NavigableMap : public SortedMap<K, V, Compare> {
 public:
  using Entry = typename Map<K, V>::Entry;

  ~NavigableMap() override = default;

  // Bring SortedMap view-method overloads into scope so the
  // inclusive-flag overloads below don't hide them.
  using SortedMap<K, V, Compare>::HeadMap;
  using SortedMap<K, V, Compare>::TailMap;
  using SortedMap<K, V, Compare>::SubMap;

  virtual std::optional<Entry> LowerEntry(const K& key) const = 0;

  virtual std::optional<Entry> FloorEntry(const K& key) const = 0;

  virtual std::optional<Entry> CeilingEntry(const K& key) const = 0;

  virtual std::optional<Entry> HigherEntry(const K& key) const = 0;

  virtual std::optional<Entry> FirstEntry() const = 0;

  virtual std::optional<Entry> LastEntry() const = 0;

  virtual std::optional<Entry> PollFirstEntry() = 0;

  virtual std::optional<Entry> PollLastEntry() = 0;

  std::optional<K> LowerKey(const K& key) const {
    auto e = LowerEntry(key);
    if (e.has_value()) {
      return e.value().Key();
    }
    return std::nullopt;
  }

  std::optional<K> FloorKey(const K& key) const {
    auto e = FloorEntry(key);
    if (e.has_value()) {
      return e.value().Key();
    }
    return std::nullopt;
  }

  std::optional<K> CeilingKey(const K& key) const {
    auto e = CeilingEntry(key);
    if (e.has_value()) {
      return e.value().Key();
    }
    return std::nullopt;
  }

  std::optional<K> HigherKey(const K& key) const {
    auto e = HigherEntry(key);
    if (e.has_value()) {
      return e.value().Key();
    }
    return std::nullopt;
  }

  virtual TreeMap<K, V, Compare> HeadMap(const K& toKey,
                                          bool inclusive) const = 0;

  virtual TreeMap<K, V, Compare> TailMap(const K& fromKey,
                                          bool inclusive) const = 0;

  virtual TreeMap<K, V, Compare> SubMap(const K& fromKey,
                                         bool fromInclusive,
                                         const K& toKey,
                                         bool toInclusive) const = 0;
};

}  // namespace jaclks::javac_base
