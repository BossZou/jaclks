#pragma once

#include "jaclks/function/compare.h"
#include "jaclks/javac-base/java/util/map.h"

namespace jaclks::javac_base {

template <typename K, typename V, typename Compare>
class TreeMap;

// Wraps a three-way comparator and negates its result, so that
// DescendingMap can return a TreeMap with reversed sort order.
template <typename Compare>
struct ReversedComparator {
  Compare original;

  int operator()(const auto& a, const auto& b) const {
    return -original(a, b);
  }
};

template <typename K,
          typename V,
          typename Compare = jaclks::Comparator<K>>
class SortedMap : public Map<K, V> {
 public:
  using Entry = typename Map<K, V>::Entry;

  ~SortedMap() override = default;

  virtual const Compare& comparator() const = 0;

  virtual K FirstKey() const = 0;

  virtual K LastKey() const = 0;

  virtual TreeMap<K, V, Compare> HeadMap(const K& toKey) const = 0;

  virtual TreeMap<K, V, Compare> TailMap(const K& fromKey) const = 0;

  virtual TreeMap<K, V, Compare> SubMap(const K& fromKey,
                                         const K& toKey) const = 0;
};

}  // namespace jaclks::javac_base
