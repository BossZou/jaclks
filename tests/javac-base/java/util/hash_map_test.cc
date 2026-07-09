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
  int val = m.ComputeIfAbsent(1, [](const int& key) { return key * 10; });
  EXPECT_EQ(val, 10);
  EXPECT_EQ(m.Get(1).value(), 10);
}

TEST(HashMapTest, ComputeIfAbsentWhenPresent) {
  HashMap<int, int> m;
  m.Put(1, 100);
  int val = m.ComputeIfAbsent(1, [](const int& key) { return key * 10; });
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
