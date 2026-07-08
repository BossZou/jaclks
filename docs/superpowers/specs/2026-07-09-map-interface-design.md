# Map Interface Design

**Date:** 2026-07-09
**Status:** Approved

## Overview

Design and implement a Java 11-style `Map<K,V>` interface with `HashMap<K,V>` concrete implementation in C++ for the jaclks library, following existing project conventions.

## Design Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Abstraction | Interface (`Map`) + Implementation (`HashMap`) | Simulate Java's interface-inheritance model |
| Method scope | Core + Java 8/11 default methods | Balance completeness with delivery speed |
| First implementation | HashMap only | Fast delivery, validate interface first |
| Entry design | Independent `class Entry<K,V>`, aliased via `using` in Map | Encapsulation + reuse |
| Hash / equality | Template parameters `Hash`, `KeyEqual` (default `std::hash`, `std::equal_to`) | Zero overhead, STL compatible |
| Namespace | `jaclks::javac_base` | Consistent with existing code |
| Error handling | Mixed: `Get()` returns `std::optional<V>`, `GetOrThrow()` throws `NoSuchElementException` | Safe + strict options |
| HashMap impl | Wraps `std::unordered_map` | Simple, correct, fast to deliver |

## File Structure

```
include/jaclks/javac-base/java/util/
├── map_entry.h                        # Entry<K,V> class template
├── map.h                              # Map<K,V> abstract template interface
├── hash_map.h                         # HashMap<K,V> concrete implementation
└── no_such_element_exception.h        # Exception class

tests/javac-base/java/util/
├── map_entry_test.cc
├── hash_map_test.cc
└── (CMakeLists.txt updated)
```

No `.cc` source files needed — all template implementations are header-only.

## API Design

### Entry<K,V>

Encapsulated key-value pair. `key_` is immutable after construction (matching Java semantics).

```cpp
template <typename K, typename V>
class Entry {
 public:
  Entry() = default;
  Entry(const K& k, const V& v);
  Entry(K&& k, V&& v);

  const K& Key() const;
  V& Value();
  const V& Value() const;
  void SetValue(const V& v);

  bool operator==(const Entry& other) const;

 private:
  K key_;
  V value_;
};
```

### Map<K,V> — Abstract Template Interface

Pure virtual methods define the contract. Default methods implement Java 8/11 convenience APIs on top of the primitives.

**Query methods (const):**

| Method | Returns | Virtual |
|--------|---------|---------|
| `Size()` | `std::size_t` | pure |
| `IsEmpty()` | `bool` | non-virtual (delegates to Size) |
| `ContainsKey(const K&)` | `bool` | pure |
| `ContainsValue(const V&)` | `bool` | virtual (default: O(n) traversal) |
| `Get(const K&)` | `std::optional<V>` | pure |
| `GetOrThrow(const K&)` | `V` | non-virtual (throws NoSuchElementException) |

**Modification methods:**

| Method | Returns | Virtual |
|--------|---------|---------|
| `Put(const K&, const V&)` | `std::optional<V>` (previous value) | pure |
| `Remove(const K&)` | `std::optional<V>` (removed value) | pure |
| `PutAll(const Map<K,V>&)` | `void` | non-virtual (iterates, calls Put) |
| `Clear()` | `void` | pure |

**Java 8/11 default methods (non-virtual):**

| Method | Returns |
|--------|---------|
| `GetOrDefault(const K&, const V& default_val)` | `V` |
| `PutIfAbsent(const K&, const V&)` | `std::optional<V>` |
| `Replace(const K&, const V&)` | `std::optional<V>` |
| `Replace(const K&, const V& old_val, const V& new_val)` | `bool` |
| `ReplaceAll(std::function<V(const K&, const V&)>)` | `void` |
| `ComputeIfAbsent(const K&, std::function<V(const K&)>)` | `V` |
| `ComputeIfPresent(const K&, std::function<V(const K&, const V&)>)` | `std::optional<V>` |
| `Compute(const K&, std::function<std::optional<V>(const K&, std::optional<V>)>)` | `std::optional<V>` |
| `Merge(const K&, const V&, std::function<V(const V&, const V&)>)` | `std::optional<V>` |
| `ForEach(std::function<void(const K&, const V&)>)` | `void` |

The `Compute` method uses `std::optional<V>` to represent whether the key is present or absent in the map, mirroring Java's `BiFunction<? super K,? super V,? extends V>` where the second argument can be null.

### HashMap<K,V,Hash,KeyEqual>

Wraps `std::unordered_map` for simplicity. Overrides all pure virtual methods from `Map<K,V>`.

```cpp
template <typename K,
          typename V,
          typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>>
class HashMap : public Map<K, V> {
 public:
  HashMap() = default;
  ~HashMap() override = default;

  std::size_t Size() const override;
  bool ContainsKey(const K& key) const override;
  bool ContainsValue(const V& value) const override;
  std::optional<V> Get(const K& key) const override;
  std::optional<V> Put(const K& key, const V& value) override;
  std::optional<V> Remove(const K& key) override;
  void Clear() override;

 private:
  std::unordered_map<K, V, Hash, KeyEqual> map_;
};
```

### NoSuchElementException

Follows existing exception pattern: inherits `std::runtime_error`, constructor accepts `const char*`.

```cpp
// include/jaclks/javac-base/java/util/no_such_element_exception.h
#pragma once

#include <stdexcept>

namespace jaclks::javac_base {

class NoSuchElementException : public std::runtime_error {
 public:
  explicit NoSuchElementException(const char* s) : std::runtime_error(s) {}
};

}  // namespace jaclks::javac_base
```

Placed under `java/util/` matching Java's `java.util.NoSuchElementException`.

## Design Principles

- **Java semantics, C++ idioms** — Interface mirrors Java's Map contract; implementation uses STL internally
- **Template-based** — No type erasure; compile-time type safety
- **Header-only templates** — No separate compilation needed for Map/Entry/HashMap
- **Consistent with existing code** — Same namespace, same exception pattern, same header structure as other jaclks components

## Future Work (Out of Scope)

- TreeMap<K,V> (ordered map implementation)
- LinkedHashMap<K,V> (insertion-ordered)
- `entrySet()` / `keySet()` / `values()` collection view methods
- Iterator support (Java's `Map.Entry` iteration pattern)
