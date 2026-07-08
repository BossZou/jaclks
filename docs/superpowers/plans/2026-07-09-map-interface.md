# Map Interface Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement Java 11-style `Map<K,V>` abstract interface and `HashMap<K,V>` wrapper in C++ for the jaclks library.

**Architecture:** Header-only templates: `Entry<K,V>` (key-value pair), `Map<K,V>` (abstract template with Java 8/11 default methods), `HashMap<K,V>` (wraps `std::unordered_map`). All under `jaclks::javac_base` namespace. Iteration in base class via protected pure virtual `ForEachEntry`.

**Tech Stack:** C++17, GoogleTest, CMake, jaclks patterns (namespace `jaclks::javac_base`, `#pragma once`, `std::runtime_error` exceptions)

## Global Constraints

- Namespace: `jaclks::javac_base`
- Headers go in `include/jaclks/javac-base/java/util/`
- Tests go in `tests/javac-base/java/util/`
- Exceptions inherit `std::runtime_error`, accept `const char*`
- Use `#pragma once` for header guards
- No `.cc` source files — all templates are header-only
- Follow existing code style (Java-doc comments, 2-space indent)
- Map is on branch `AhooPao/map_dev`

---

## File Structure

| File | Responsibility |
|------|---------------|
| `include/jaclks/javac-base/java/util/no_such_element_exception.h` | Exception thrown by `GetOrThrow()` |
| `include/jaclks/javac-base/java/util/map_entry.h` | `Entry<K,V>` class template |
| `include/jaclks/javac-base/java/util/map.h` | `Map<K,V>` abstract template with default methods |
| `include/jaclks/javac-base/java/util/hash_map.h` | `HashMap<K,V>` wrapping `std::unordered_map` |
| `tests/javac-base/java/util/map_entry_test.cc` | Tests for `Entry<K,V>` |
| `tests/javac-base/java/util/hash_map_test.cc` | Tests for `HashMap<K,V>` + Map default methods |

---

### Task 1: NoSuchElementException header

**Files:**
- Create: `include/jaclks/javac-base/java/util/no_such_element_exception.h`

**Interfaces:**
- Produces: `jaclks::javac_base::NoSuchElementException` — inherits `std::runtime_error`, construct with `const char*`

- [ ] **Step 1: Write the header**

```cpp
#pragma once

#include <stdexcept>

namespace jaclks::javac_base {

class NoSuchElementException : public std::runtime_error {
 public:
  explicit NoSuchElementException(const char* s) : std::runtime_error(s) {}
};

}  // namespace jaclks::javac_base
```

- [ ] **Step 2: Verify compilation**

Run: `g++ -std=c++17 -c -I include include/jaclks/javac-base/java/util/no_such_element_exception.h -o /dev/null`
Expected: Exit code 0, no errors.

- [ ] **Step 3: Commit**

```bash
git add include/jaclks/javac-base/java/util/no_such_element_exception.h
git commit -m "feat: add NoSuchElementException class"
```

---

### Task 2: Entry<K,V> header

**Files:**
- Create: `include/jaclks/javac-base/java/util/map_entry.h`

**Interfaces:**
- Produces: `jaclks::javac_base::Entry<K,V>` — class with `Key()`, `Value()`, `SetValue()`, `operator==`

- [ ] **Step 1: Write the header**

```cpp
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
  K key_;
  V value_;
};

}  // namespace jaclks::javac_base
```

- [ ] **Step 2: Verify compilation of a simple usage**

Run:
```bash
cat > /tmp/test_entry.cc << 'EOF'
#include "jaclks/javac-base/java/util/map_entry.h"
using namespace jaclks::javac_base;
int main() {
  Entry<int, int> e1(1, 2);
  Entry<int, int> e2(1, 3);
  return e1.Key() == 1 && e1.Value() == 2 && !(e1 == e2) ? 0 : 1;
}
EOF
g++ -std=c++17 -I include /tmp/test_entry.cc -o /tmp/test_entry && /tmp/test_entry
```
Expected: Exit code 0.

- [ ] **Step 3: Commit**

```bash
git add include/jaclks/javac-base/java/util/map_entry.h
git commit -m "feat: add Entry<K,V> class template"
```

---

### Task 3: Entry unit tests

**Files:**
- Create: `tests/javac-base/java/util/map_entry_test.cc`

**Interfaces:**
- Consumes: `Entry<K,V>` from Task 2
- Produces: GoogleTest test cases covering construction, accessors, mutation, equality

- [ ] **Step 1: Write the test file**

```cpp
#include "jaclks/javac-base/java/util/map_entry.h"

#include <gtest/gtest.h>
#include <string>

namespace jaclks::javac_base {

namespace {

TEST(EntryTest, DefaultConstruction) {
  Entry<int, int> e;
  // Key and value are default-initialized
  EXPECT_EQ(e.Key(), 0);
  EXPECT_EQ(e.Value(), 0);
}

TEST(EntryTest, ConstructionWithKeyValue) {
  Entry<int, std::string> e(1, "hello");
  EXPECT_EQ(e.Key(), 1);
  EXPECT_EQ(e.Value(), "hello");
}

TEST(EntryTest, MoveConstruction) {
  std::string key = "key";
  std::string value = "value";
  Entry<std::string, std::string> e(std::move(key), std::move(value));
  EXPECT_EQ(e.Key(), "key");
  EXPECT_EQ(e.Value(), "value");
}

TEST(EntryTest, KeyReturnsConstRef) {
  Entry<int, int> e(1, 2);
  const auto& k = e.Key();
  static_assert(std::is_const_v<std::remove_reference_t<decltype(k)>>,
                "Key() must return const reference");
  EXPECT_EQ(k, 1);
}

TEST(EntryTest, ValueNonConstRefMutates) {
  Entry<int, int> e(1, 2);
  e.Value() = 42;
  EXPECT_EQ(e.Value(), 42);
}

TEST(EntryTest, SetValue) {
  Entry<int, std::string> e(1, "old");
  e.SetValue("new");
  EXPECT_EQ(e.Value(), "new");
}

TEST(EntryTest, ValueConstRefForConstObject) {
  const Entry<int, std::string> e(1, "hello");
  EXPECT_EQ(e.Value(), "hello");
}

TEST(EntryTest, OperatorEqualsTrue) {
  Entry<int, int> e1(1, 2);
  Entry<int, int> e2(1, 2);
  EXPECT_TRUE(e1 == e2);
}

TEST(EntryTest, OperatorEqualsFalseDifferentValue) {
  Entry<int, int> e1(1, 2);
  Entry<int, int> e2(1, 3);
  EXPECT_FALSE(e1 == e2);
}

TEST(EntryTest, OperatorEqualsFalseDifferentKey) {
  Entry<int, int> e1(1, 2);
  Entry<int, int> e2(2, 2);
  EXPECT_FALSE(e1 == e2);
}

}  // namespace

}  // namespace jaclks::javac_base
```

- [ ] **Step 2: Update test CMakeLists.txt to include the new test file**

Read `tests/javac-base/CMakeLists.txt` first, then add the new source directory:

The existing CMakeLists.txt has:
```cmake
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/lang JBASE_TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/security JBASE_TEST_SRC_FILES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/util/regex JBASE_TEST_SRC_FILES)
```

Add after the last `aux_source_directory` line:
```cmake
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/java/util JBASE_TEST_SRC_FILES)
```

- [ ] **Step 3: Build and run tests**

Run:
```bash
cd cmake-build-debug && cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_GTEST=ON && make jbase_gtest && ./tests/javac-base/jbase_gtest --gtest_filter="*Entry*"
```
Expected: All EntryTest cases pass.

- [ ] **Step 4: Commit**

```bash
git add tests/javac-base/java/util/map_entry_test.cc tests/javac-base/CMakeLists.txt
git commit -m "test: add Entry<K,V> unit tests"
```

---

### Task 4: Map<K,V> abstract template header

**Files:**
- Create: `include/jaclks/javac-base/java/util/map.h`

**Interfaces:**
- Consumes: `Entry<K,V>` from Task 2, `NoSuchElementException` from Task 1
- Produces: `jaclks::javac_base::Map<K,V>` — abstract class with pure virtual methods and default implementations

- [ ] **Step 1: Write the header**

```cpp
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
```

- [ ] **Step 2: Verify compilation**

Run:
```bash
g++ -std=c++17 -c -I include \
  include/jaclks/javac-base/java/util/map.h \
  -o /dev/null
```
Expected: Exit code 0, no errors.

- [ ] **Step 3: Commit**

```bash
git add include/jaclks/javac-base/java/util/map.h
git commit -m "feat: add Map<K,V> abstract template interface"
```

---

### Task 5: HashMap<K,V> header

**Files:**
- Create: `include/jaclks/javac-base/java/util/hash_map.h`

**Interfaces:**
- Consumes: `Map<K,V>` from Task 4
- Produces: `jaclks::javac_base::HashMap<K,V,Hash,KeyEqual>` — concrete Map implementation

- [ ] **Step 1: Write the header**

```cpp
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
```

- [ ] **Step 2: Verify compilation with a smoke test**

Run:
```bash
cat > /tmp/test_hashmap.cc << 'EOF'
#include "jaclks/javac-base/java/util/hash_map.h"
#include <cassert>
using namespace jaclks::javac_base;
int main() {
  HashMap<int, int> m;
  assert(m.IsEmpty());
  m.Put(1, 100);
  assert(m.Size() == 1);
  assert(m.Get(1).value() == 100);
  assert(!m.Get(2).has_value());
  m.Remove(1);
  assert(m.IsEmpty());
  return 0;
}
EOF
g++ -std=c++17 -I include /tmp/test_hashmap.cc -o /tmp/test_hashmap && /tmp/test_hashmap
```
Expected: Exit code 0.

- [ ] **Step 3: Commit**

```bash
git add include/jaclks/javac-base/java/util/hash_map.h
git commit -m "feat: add HashMap<K,V> wrapping std::unordered_map"
```

---

### Task 6: HashMap unit tests

**Files:**
- Create: `tests/javac-base/java/util/hash_map_test.cc`

**Interfaces:**
- Consumes: `HashMap<K,V>` from Task 5, `Map<K,V>` from Task 4
- Produces: GoogleTest test cases covering all Map methods via HashMap

- [ ] **Step 1: Write the test file**

```cpp
#include "jaclks/javac-base/java/util/hash_map.h"

#include <gtest/gtest.h>
#include <string>

namespace jaclks::javac_base {

namespace {

// ==================== Core Query Tests ====================

TEST(HashMapTest, EmptyMap) {
  HashMap<int, int> m;
  EXPECT_EQ(m.Size(), 0UL);
  EXPECT_TRUE(m.IsEmpty());
}

TEST(HashMapTest, PutIncreasesSize) {
  HashMap<int, int> m;
  m.Put(1, 100);
  EXPECT_EQ(m.Size(), 1UL);
  EXPECT_FALSE(m.IsEmpty());
}

TEST(HashMapTest, PutSameKeyReturnsOldValue) {
  HashMap<int, int> m;
  m.Put(1, 100);
  auto old = m.Put(1, 200);
  ASSERT_TRUE(old.has_value());
  EXPECT_EQ(old.value(), 100);
  EXPECT_EQ(m.Size(), 1UL);
}

TEST(HashMapTest, PutNewKeyReturnsNullopt) {
  HashMap<int, int> m;
  auto old = m.Put(1, 100);
  EXPECT_FALSE(old.has_value());
}

TEST(HashMapTest, GetExistingKey) {
  HashMap<int, int> m;
  m.Put(1, 100);
  auto val = m.Get(1);
  ASSERT_TRUE(val.has_value());
  EXPECT_EQ(val.value(), 100);
}

TEST(HashMapTest, GetMissingKeyReturnsNullopt) {
  HashMap<int, int> m;
  auto val = m.Get(99);
  EXPECT_FALSE(val.has_value());
}

TEST(HashMapTest, GetOrThrowExisting) {
  HashMap<int, int> m;
  m.Put(1, 100);
  EXPECT_EQ(m.GetOrThrow(1), 100);
}

TEST(HashMapTest, GetOrThrowMissingThrows) {
  HashMap<int, int> m;
  EXPECT_THROW(m.GetOrThrow(99), NoSuchElementException);
}

TEST(HashMapTest, ContainsKeyTrue) {
  HashMap<int, int> m;
  m.Put(1, 100);
  EXPECT_TRUE(m.ContainsKey(1));
}

TEST(HashMapTest, ContainsKeyFalse) {
  HashMap<int, int> m;
  EXPECT_FALSE(m.ContainsKey(99));
}

TEST(HashMapTest, ContainsValueTrue) {
  HashMap<int, int> m;
  m.Put(1, 100);
  EXPECT_TRUE(m.ContainsValue(100));
}

TEST(HashMapTest, ContainsValueFalse) {
  HashMap<int, int> m;
  m.Put(1, 100);
  EXPECT_FALSE(m.ContainsValue(999));
}

TEST(HashMapTest, RemoveExistingReturnsValue) {
  HashMap<int, int> m;
  m.Put(1, 100);
  auto removed = m.Remove(1);
  ASSERT_TRUE(removed.has_value());
  EXPECT_EQ(removed.value(), 100);
  EXPECT_EQ(m.Size(), 0UL);
}

TEST(HashMapTest, RemoveMissingReturnsNullopt) {
  HashMap<int, int> m;
  auto removed = m.Remove(99);
  EXPECT_FALSE(removed.has_value());
}

TEST(HashMapTest, ClearEmptiesMap) {
  HashMap<int, int> m;
  m.Put(1, 100);
  m.Put(2, 200);
  m.Clear();
  EXPECT_EQ(m.Size(), 0UL);
  EXPECT_TRUE(m.IsEmpty());
  EXPECT_FALSE(m.ContainsKey(1));
}

TEST(HashMapTest, MultipleKeys) {
  HashMap<int, int> m;
  m.Put(1, 100);
  m.Put(2, 200);
  m.Put(3, 300);
  EXPECT_EQ(m.Size(), 3UL);
  EXPECT_EQ(m.Get(1).value(), 100);
  EXPECT_EQ(m.Get(2).value(), 200);
  EXPECT_EQ(m.Get(3).value(), 300);
}

TEST(HashMapTest, StringKeys) {
  HashMap<std::string, int> m;
  m.Put("hello", 42);
  EXPECT_TRUE(m.ContainsKey("hello"));
  EXPECT_EQ(m.Get("hello").value(), 42);
}

// ==================== Default Method Tests ====================

TEST(HashMapTest, PutAll) {
  HashMap<int, int> src;
  src.Put(1, 10);
  src.Put(2, 20);

  HashMap<int, int> dst;
  dst.PutAll(src);

  EXPECT_EQ(dst.Size(), 2UL);
  EXPECT_EQ(dst.Get(1).value(), 10);
  EXPECT_EQ(dst.Get(2).value(), 20);
}

TEST(HashMapTest, GetOrDefaultExisting) {
  HashMap<int, int> m;
  m.Put(1, 100);
  EXPECT_EQ(m.GetOrDefault(1, -1), 100);
}

TEST(HashMapTest, GetOrDefaultMissing) {
  HashMap<int, int> m;
  EXPECT_EQ(m.GetOrDefault(99, -1), -1);
}

TEST(HashMapTest, PutIfAbsentWhenAbsent) {
  HashMap<int, int> m;
  auto result = m.PutIfAbsent(1, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(m.Get(1).value(), 100);
}

TEST(HashMapTest, PutIfAbsentWhenPresent) {
  HashMap<int, int> m;
  m.Put(1, 100);
  auto result = m.PutIfAbsent(1, 200);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 100);
  EXPECT_EQ(m.Get(1).value(), 100);  // unchanged
}

TEST(HashMapTest, ReplaceByKeyWhenPresent) {
  HashMap<int, int> m;
  m.Put(1, 100);
  auto old = m.Replace(1, 200);
  ASSERT_TRUE(old.has_value());
  EXPECT_EQ(old.value(), 100);
  EXPECT_EQ(m.Get(1).value(), 200);
}

TEST(HashMapTest, ReplaceByKeyWhenAbsent) {
  HashMap<int, int> m;
  auto old = m.Replace(99, 200);
  EXPECT_FALSE(old.has_value());
  EXPECT_FALSE(m.ContainsKey(99));
}

TEST(HashMapTest, ReplaceByKeyValueWhenMatches) {
  HashMap<int, int> m;
  m.Put(1, 100);
  EXPECT_TRUE(m.Replace(1, 100, 200));
  EXPECT_EQ(m.Get(1).value(), 200);
}

TEST(HashMapTest, ReplaceByKeyValueWhenOldValueMismatch) {
  HashMap<int, int> m;
  m.Put(1, 100);
  EXPECT_FALSE(m.Replace(1, 999, 200));
  EXPECT_EQ(m.Get(1).value(), 100);  // unchanged
}

TEST(HashMapTest, ReplaceByKeyValueWhenKeyMissing) {
  HashMap<int, int> m;
  EXPECT_FALSE(m.Replace(99, 100, 200));
}

TEST(HashMapTest, ReplaceAll) {
  HashMap<int, int> m;
  m.Put(1, 10);
  m.Put(2, 20);
  m.ReplaceAll([](const int& key, const int& value) {
    return value * 10;
  });
  EXPECT_EQ(m.Get(1).value(), 100);
  EXPECT_EQ(m.Get(2).value(), 200);
}

TEST(HashMapTest, ComputeIfAbsentWhenAbsent) {
  HashMap<int, int> m;
  V val = m.ComputeIfAbsent(1, [](const int& key) { return key * 10; });
  EXPECT_EQ(val, 10);
  EXPECT_EQ(m.Get(1).value(), 10);
}

TEST(HashMapTest, ComputeIfAbsentWhenPresent) {
  HashMap<int, int> m;
  m.Put(1, 100);
  V val = m.ComputeIfAbsent(1, [](const int& key) { return key * 10; });
  EXPECT_EQ(val, 100);  // function not called, existing value returned
}

TEST(HashMapTest, ComputeIfPresentWhenPresent) {
  HashMap<int, int> m;
  m.Put(1, 100);
  auto result = m.ComputeIfPresent(
      1, [](const int& key, const int& val) { return val * 2; });
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 200);
  EXPECT_EQ(m.Get(1).value(), 200);
}

TEST(HashMapTest, ComputeIfPresentWhenAbsent) {
  HashMap<int, int> m;
  auto result = m.ComputeIfPresent(
      99, [](const int& key, const int& val) { return val * 2; });
  EXPECT_FALSE(result.has_value());
}

TEST(HashMapTest, ComputeInsertsNewValue) {
  HashMap<int, int> m;
  auto result = m.Compute(1,
      [](const int& key, std::optional<int> old) {
        return std::optional<int>(key * 10);
      });
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 10);
  EXPECT_EQ(m.Get(1).value(), 10);
}

TEST(HashMapTest, ComputeUpdatesExistingValue) {
  HashMap<int, int> m;
  m.Put(1, 100);
  auto result = m.Compute(1,
      [](const int& key, std::optional<int> old) {
        return std::optional<int>(old.value() * 2);
      });
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 200);
  EXPECT_EQ(m.Get(1).value(), 200);
}

TEST(HashMapTest, ComputeRemovesWhenNullopt) {
  HashMap<int, int> m;
  m.Put(1, 100);
  auto result = m.Compute(1,
      [](const int& key, std::optional<int> old) {
        return std::optional<int>();  // nullopt -> remove
      });
  EXPECT_FALSE(result.has_value());
  EXPECT_FALSE(m.ContainsKey(1));
}

TEST(HashMapTest, MergeWhenKeyAbsent) {
  HashMap<int, int> m;
  auto result = m.Merge(1, 100,
      [](const int& old_val, const int& new_val) { return old_val + new_val; });
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 100);
  EXPECT_EQ(m.Get(1).value(), 100);
}

TEST(HashMapTest, MergeWhenKeyPresent) {
  HashMap<int, int> m;
  m.Put(1, 100);
  auto result = m.Merge(1, 200,
      [](const int& old_val, const int& new_val) { return old_val + new_val; });
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 300);
  EXPECT_EQ(m.Get(1).value(), 300);
}

TEST(HashMapTest, ForEach) {
  HashMap<int, int> m;
  m.Put(1, 10);
  m.Put(2, 20);
  int sum_keys = 0;
  int sum_values = 0;
  m.ForEach([&](const int& key, const int& value) {
    sum_keys += key;
    sum_values += value;
  });
  EXPECT_EQ(sum_keys, 3);
  EXPECT_EQ(sum_values, 30);
}

TEST(HashMapTest, CustomHashAndKeyEqual) {
  // Use a case-insensitive hash map with string keys
  struct CaseInsensitiveHash {
    std::size_t operator()(const std::string& s) const {
      std::string lower;
      for (char c : s) lower += static_cast<char>(std::tolower(c));
      return std::hash<std::string>{}(lower);
    }
  };
  struct CaseInsensitiveEqual {
    bool operator()(const std::string& a, const std::string& b) const {
      return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                        [](char ca, char cb) {
                          return std::tolower(ca) == std::tolower(cb);
                        });
    }
  };

  HashMap<std::string, int, CaseInsensitiveHash, CaseInsensitiveEqual> m;
  m.Put("Hello", 42);
  EXPECT_TRUE(m.ContainsKey("HELLO"));
  EXPECT_TRUE(m.ContainsKey("hello"));
  EXPECT_EQ(m.Get("HeLLo").value(), 42);
}

}  // namespace

}  // namespace jaclks::javac_base
```

- [ ] **Step 2: Build and run all tests**

Run:
```bash
cd cmake-build-debug && cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_GTEST=ON && make jbase_gtest && ./tests/javac-base/jbase_gtest
```
Expected: All tests pass — both existing tests and new EntryTest + HashMapTest cases.

- [ ] **Step 3: Run existing tests to verify no regressions**

Run:
```bash
cd cmake-build-debug && ctest --verbose --output-on-failure --build-config Debug
```
Expected: All tests pass, no regressions.

- [ ] **Step 4: Commit**

```bash
git add tests/javac-base/java/util/hash_map_test.cc
git commit -m "test: add HashMap<K,V> unit tests covering all Map methods"
```

---

### Task 7: Final verification and commit

- [ ] **Step 1: Full build from clean**

```bash
cd cmake-build-debug && cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_GTEST=ON && make -j$(nproc)
```
Expected: Clean build, no warnings.

- [ ] **Step 2: Run all tests**

```bash
cd cmake-build-debug && ctest --verbose --output-on-failure --build-config Debug
```
Expected: All tests pass.

- [ ] **Step 3: Verify header-only self-sufficiency**

Each header should compile independently:

```bash
g++ -std=c++17 -fsyntax-only -I include include/jaclks/javac-base/java/util/no_such_element_exception.h
g++ -std=c++17 -fsyntax-only -I include include/jaclks/javac-base/java/util/map_entry.h
g++ -std=c++17 -fsyntax-only -I include include/jaclks/javac-base/java/util/map.h
g++ -std=c++17 -fsyntax-only -I include include/jaclks/javac-base/java/util/hash_map.h
```
Expected: All exit code 0.
