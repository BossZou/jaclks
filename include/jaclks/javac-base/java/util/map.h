#pragma once

#include <cstddef>
#include <functional>
#include <optional>

#include "jaclks/javac-base/java/util/map_entry.h"
#include "jaclks/javac-base/java/util/no_such_element_exception.h"

namespace jaclks::javac_base {

template <typename K, typename V>
class Map {
 public:
  using Entry = ::jaclks::javac_base::Entry<K, V>;

  virtual ~Map() = default;

  // === Query methods ===

  virtual std::size_t Size() const = 0;

  bool IsEmpty() const { return Size() == 0; }

  virtual bool ContainsKey(const K& key) const = 0;

  virtual bool ContainsValue(const V& value) const {
    bool found = false;
    ForEachEntry([&found, &value](const K& /*key*/, const V& v) {
      if (v == value) {
        found = true;
      }
    });
    return found;
  }

  virtual std::optional<V> Get(const K& key) const = 0;

  V GetOrThrow(const K& key) const {
    auto opt = Get(key);
    if (!opt.has_value()) {
      throw NoSuchElementException("Key not found in map");
    }
    return opt.value();
  }

  // === Modification methods ===

  virtual std::optional<V> Put(const K& key, const V& value) = 0;

  virtual std::optional<V> Remove(const K& key) = 0;

  void PutAll(const Map<K, V>& other) {
    other.ForEachEntry([this](const K& key, const V& value) {
      Put(key, value);
    });
  }

  virtual void Clear() = 0;

  // === Java 8/11 default methods ===

  V GetOrDefault(const K& key, const V& default_val) const {
    auto opt = Get(key);
    return opt.has_value() ? opt.value() : default_val;
  }

  std::optional<V> PutIfAbsent(const K& key, const V& value) {
    auto existing = Get(key);
    if (!existing.has_value()) {
      Put(key, value);
      return std::nullopt;
    }
    return existing;
  }

  std::optional<V> Replace(const K& key, const V& value) {
    if (ContainsKey(key)) {
      return Put(key, value);
    }
    return std::nullopt;
  }

  bool Replace(const K& key, const V& old_value, const V& new_value) {
    auto existing = Get(key);
    if (existing.has_value() && existing.value() == old_value) {
      Put(key, new_value);
      return true;
    }
    return false;
  }

  void ReplaceAll(std::function<V(const K&, const V&)> function) {
    ForEachEntry([this, &function](const K& key, const V& value) {
      Put(key, function(key, value));
    });
  }

  V ComputeIfAbsent(const K& key, std::function<V(const K&)> mapping_function) {
    auto existing = Get(key);
    if (existing.has_value()) {
      return existing.value();
    }
    V new_value = mapping_function(key);
    Put(key, new_value);
    return new_value;
  }

  std::optional<V> ComputeIfPresent(
      const K& key,
      std::function<V(const K&, const V&)> remapping_function) {
    auto existing = Get(key);
    if (existing.has_value()) {
      V new_value = remapping_function(key, existing.value());
      Put(key, new_value);
      return std::optional<V>(new_value);
    }
    return std::nullopt;
  }

  std::optional<V> Compute(
      const K& key,
      std::function<std::optional<V>(const K&, std::optional<V>)>
          remapping_function) {
    auto old_value = Get(key);
    auto new_value = remapping_function(key, old_value);
    if (new_value.has_value()) {
      Put(key, new_value.value());
    } else if (old_value.has_value()) {
      Remove(key);
    }
    return new_value;
  }

  std::optional<V> Merge(
      const K& key,
      const V& value,
      std::function<V(const V&, const V&)> remapping_function) {
    auto existing = Get(key);
    if (existing.has_value()) {
      V new_value = remapping_function(existing.value(), value);
      Put(key, new_value);
      return std::optional<V>(new_value);
    }
    Put(key, value);
    return std::optional<V>(value);
  }

  void ForEach(std::function<void(const K&, const V&)> action) const {
    ForEachEntry(std::move(action));
  }

 protected:
  virtual void ForEachEntry(
      std::function<void(const K&, const V&)> action) const = 0;
};

}  // namespace jaclks::javac_base
