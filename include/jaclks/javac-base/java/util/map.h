#pragma once

#include <optional>
#include <utility>

namespace jaclks::javac_base {

template <typename K, typename V>
class Map {
 public:
  virtual ~Map() = default;

  virtual std::size_t Size() const = 0;

  virtual bool IsEmpty() const = 0;

  virtual bool ContainsKey(const K &key) const = 0;

  virtual bool ContainsValue(const V &value) const = 0;

  virtual std::pair<V, bool> Get(const K &key) const = 0;

  virtual std::pair<V, bool> Put(const K &key, const V &value) = 0;

  virtual std::pair<V, bool> Remove(const K &key) = 0;

  virtual void PutAll(const Map<K, V> &m) = 0;

  virtual void Clear() = 0;

  // FIXME(John Doe): Implement it.
  // virtual Set<K> KeySet() const = 0;

  // FIXME(John Doe): Implement it.
  // virtual Collection<V> Values() const = 0;

  // FIXME(John Doe): Implement it.
  // Set<Map<K, V>::Entry> EntrySet();

  // class Entry {};

  virtual V GetOrDefault(const K &key, V value) {
    if (auto ret = Get(key); ret.second) {
      return ret.first;
    }

    return std::move(value);
  }

  // virtual void ForEach();

  // virtual void ReplaceAll();

  // virtual std::pair<V, bool> PutIfAbsent(const K& key, const V& value) {}

  // virtual bool Remove(const K& key, const V& value) {}

  // virtual bool Replace(const K& key, const V& old_value, const V& new_value)
  // {}

  // virtual bool Replace(const K& key, const V& value) {}

  // virtual std::pair<V, bool> ComputeIfAbsent(const K& key, Function<>) {}

  // virtual std::pair<V, bool> ComputeIfPersent(const K& key, Function<>) {}

  // virtual std::pair<V, bool> Compute(const K& key, Function<>) {}

  // virtual std::pair<V, bool> Merge(const K& key, const V& value, Function<>)
  // {}
};

}  // namespace jaclks::javac_base
