#include "jaclks/javac-base/java/util/tree_map.h"

#include <string>

#include <gtest/gtest.h>

namespace jaclks::javac_base {

namespace {

// ==================== Core Map Operation Tests ====================

TEST(TreeMapTest, EmptyMap) {
  TreeMap<int, int> m;
  EXPECT_EQ(m.Size(), 0UL);
  EXPECT_TRUE(m.IsEmpty());
}

TEST(TreeMapTest, PutIncreasesSize) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  EXPECT_EQ(m.Size(), 1UL);
  EXPECT_FALSE(m.IsEmpty());
}

TEST(TreeMapTest, PutSameKeyReturnsOldValue) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  auto old = m.Put(1, 200);
  ASSERT_TRUE(old.has_value());
  EXPECT_EQ(old.value(), 100);
  EXPECT_EQ(m.Size(), 1UL);
}

TEST(TreeMapTest, PutNewKeyReturnsNullopt) {
  TreeMap<int, int> m;
  auto old = m.Put(1, 100);
  EXPECT_FALSE(old.has_value());
}

TEST(TreeMapTest, GetExistingKey) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  auto val = m.Get(1);
  ASSERT_TRUE(val.has_value());
  EXPECT_EQ(val.value(), 100);
}

TEST(TreeMapTest, GetMissingKeyReturnsNullopt) {
  TreeMap<int, int> m;
  EXPECT_FALSE(m.Get(99).has_value());
}

TEST(TreeMapTest, GetOrThrowExisting) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  EXPECT_EQ(m.GetOrThrow(1), 100);
}

TEST(TreeMapTest, GetOrThrowMissingThrows) {
  TreeMap<int, int> m;
  EXPECT_THROW(m.GetOrThrow(99), NoSuchElementException);
}

TEST(TreeMapTest, ContainsKeyTrue) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  EXPECT_TRUE(m.ContainsKey(1));
}

TEST(TreeMapTest, ContainsKeyFalse) {
  TreeMap<int, int> m;
  EXPECT_FALSE(m.ContainsKey(99));
}

TEST(TreeMapTest, ContainsValueTrue) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  EXPECT_TRUE(m.ContainsValue(100));
}

TEST(TreeMapTest, ContainsValueFalse) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  EXPECT_FALSE(m.ContainsValue(999));
}

TEST(TreeMapTest, RemoveExistingReturnsValue) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  auto removed = m.Remove(1);
  ASSERT_TRUE(removed.has_value());
  EXPECT_EQ(removed.value(), 100);
  EXPECT_EQ(m.Size(), 0UL);
}

TEST(TreeMapTest, RemoveMissingReturnsNullopt) {
  TreeMap<int, int> m;
  EXPECT_FALSE(m.Remove(99).has_value());
}

TEST(TreeMapTest, ClearEmptiesMap) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  m.Put(2, 200);
  m.Clear();
  EXPECT_EQ(m.Size(), 0UL);
  EXPECT_TRUE(m.IsEmpty());
  EXPECT_FALSE(m.ContainsKey(1));
}

TEST(TreeMapTest, MultipleKeys) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  m.Put(2, 200);
  m.Put(3, 300);
  EXPECT_EQ(m.Size(), 3UL);
  EXPECT_EQ(m.Get(1).value(), 100);
  EXPECT_EQ(m.Get(2).value(), 200);
  EXPECT_EQ(m.Get(3).value(), 300);
}

TEST(TreeMapTest, StringKeys) {
  TreeMap<std::string, int> m;
  m.Put("hello", 42);
  EXPECT_TRUE(m.ContainsKey("hello"));
  EXPECT_EQ(m.Get("hello").value(), 42);
}

// ==================== Ordering Verification ====================

TEST(TreeMapTest, ForEachIteratesInSortedKeyOrder) {
  TreeMap<int, std::string> m;
  m.Put(3, "three");
  m.Put(1, "one");
  m.Put(2, "two");

  std::vector<int> keys;
  m.ForEach([&](const int& k, const std::string&) { keys.push_back(k); });

  ASSERT_EQ(keys.size(), 3UL);
  EXPECT_EQ(keys[0], 1);
  EXPECT_EQ(keys[1], 2);
  EXPECT_EQ(keys[2], 3);
}

TEST(TreeMapTest, StringKeysAreSortedLexicographically) {
  TreeMap<std::string, int> m;
  m.Put("banana", 1);
  m.Put("apple", 2);
  m.Put("cherry", 3);

  std::vector<std::string> keys;
  m.ForEach(
      [&](const std::string& k, const int&) { keys.push_back(k); });

  ASSERT_EQ(keys.size(), 3UL);
  EXPECT_EQ(keys[0], "apple");
  EXPECT_EQ(keys[1], "banana");
  EXPECT_EQ(keys[2], "cherry");
}

// ==================== SortedMap Tests ====================

TEST(TreeMapTest, FirstKeyReturnsSmallest) {
  TreeMap<int, int> m;
  m.Put(3, 300);
  m.Put(1, 100);
  m.Put(2, 200);
  EXPECT_EQ(m.FirstKey(), 1);
}

TEST(TreeMapTest, LastKeyReturnsLargest) {
  TreeMap<int, int> m;
  m.Put(3, 300);
  m.Put(1, 100);
  m.Put(2, 200);
  EXPECT_EQ(m.LastKey(), 3);
}

TEST(TreeMapTest, FirstKeyThrowsWhenEmpty) {
  TreeMap<int, int> m;
  EXPECT_THROW(m.FirstKey(), NoSuchElementException);
}

TEST(TreeMapTest, LastKeyThrowsWhenEmpty) {
  TreeMap<int, int> m;
  EXPECT_THROW(m.LastKey(), NoSuchElementException);
}

TEST(TreeMapTest, ComparatorReturnsCompare) {
  TreeMap<int, int> m;
  const auto& cmp = m.comparator();
  EXPECT_LT(cmp(1, 2), 0);
  EXPECT_GT(cmp(2, 1), 0);
  EXPECT_EQ(cmp(1, 1), 0);
}

// ==================== NavigableMap Tests ====================

TEST(TreeMapTest, LowerEntryReturnsStrictlyLess) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");
  m.Put(20, "twenty");
  m.Put(30, "thirty");

  auto e = m.LowerEntry(25);
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 20);
  EXPECT_EQ(e.value().Value(), "twenty");
}

TEST(TreeMapTest, LowerEntryReturnsNulloptWhenNoSmallerKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");
  m.Put(20, "twenty");

  auto e = m.LowerEntry(10);
  EXPECT_FALSE(e.has_value());
}

TEST(TreeMapTest, LowerEntryReturnsNulloptWhenEmpty) {
  TreeMap<int, std::string> m;
  EXPECT_FALSE(m.LowerEntry(5).has_value());
}

TEST(TreeMapTest, FloorEntryReturnsEqualKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");
  m.Put(20, "twenty");

  auto e = m.FloorEntry(10);
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 10);
}

TEST(TreeMapTest, FloorEntryReturnsSmallerWhenNoExactMatch) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");
  m.Put(20, "twenty");

  auto e = m.FloorEntry(15);
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 10);
}

TEST(TreeMapTest, FloorEntryReturnsNulloptWhenAllLarger) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");

  EXPECT_FALSE(m.FloorEntry(5).has_value());
}

TEST(TreeMapTest, CeilingEntryReturnsExactMatch) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");
  m.Put(20, "twenty");

  auto e = m.CeilingEntry(10);
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 10);
}

TEST(TreeMapTest, CeilingEntryReturnsNextLarger) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");
  m.Put(20, "twenty");

  auto e = m.CeilingEntry(15);
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 20);
}

TEST(TreeMapTest, CeilingEntryReturnsNulloptWhenAllSmaller) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");

  EXPECT_FALSE(m.CeilingEntry(20).has_value());
}

TEST(TreeMapTest, HigherEntryReturnsStrictlyGreater) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");
  m.Put(20, "twenty");

  auto e = m.HigherEntry(10);
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 20);
}

TEST(TreeMapTest, HigherEntryReturnsNulloptWhenNoGreaterKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");

  EXPECT_FALSE(m.HigherEntry(10).has_value());
}

TEST(TreeMapTest, HigherEntryReturnsNulloptWhenEmpty) {
  TreeMap<int, std::string> m;
  EXPECT_FALSE(m.HigherEntry(5).has_value());
}

TEST(TreeMapTest, LowerKeyReturnsKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");
  m.Put(20, "twenty");

  auto k = m.LowerKey(20);
  ASSERT_TRUE(k.has_value());
  EXPECT_EQ(k.value(), 10);
}

TEST(TreeMapTest, FloorKeyReturnsKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");

  auto k = m.FloorKey(15);
  ASSERT_TRUE(k.has_value());
  EXPECT_EQ(k.value(), 10);
}

TEST(TreeMapTest, CeilingKeyReturnsKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");

  auto k = m.CeilingKey(5);
  ASSERT_TRUE(k.has_value());
  EXPECT_EQ(k.value(), 10);
}

TEST(TreeMapTest, HigherKeyReturnsKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "ten");
  m.Put(20, "twenty");

  auto k = m.HigherKey(10);
  ASSERT_TRUE(k.has_value());
  EXPECT_EQ(k.value(), 20);
}

TEST(TreeMapTest, FirstEntryReturnsSmallest) {
  TreeMap<int, std::string> m;
  m.Put(3, "c");
  m.Put(1, "a");
  m.Put(2, "b");

  auto e = m.FirstEntry();
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 1);
  EXPECT_EQ(e.value().Value(), "a");
}

TEST(TreeMapTest, FirstEntryReturnsNulloptWhenEmpty) {
  TreeMap<int, std::string> m;
  EXPECT_FALSE(m.FirstEntry().has_value());
}

TEST(TreeMapTest, LastEntryReturnsLargest) {
  TreeMap<int, std::string> m;
  m.Put(3, "c");
  m.Put(1, "a");
  m.Put(2, "b");

  auto e = m.LastEntry();
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 3);
  EXPECT_EQ(e.value().Value(), "c");
}

TEST(TreeMapTest, LastEntryReturnsNulloptWhenEmpty) {
  TreeMap<int, std::string> m;
  EXPECT_FALSE(m.LastEntry().has_value());
}

TEST(TreeMapTest, PollFirstEntryRemovesAndReturns) {
  TreeMap<int, std::string> m;
  m.Put(3, "c");
  m.Put(1, "a");
  m.Put(2, "b");

  auto e = m.PollFirstEntry();
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 1);
  EXPECT_EQ(e.value().Value(), "a");
  EXPECT_EQ(m.Size(), 2UL);
  EXPECT_FALSE(m.ContainsKey(1));
}

TEST(TreeMapTest, PollFirstEntryEmptyReturnsNullopt) {
  TreeMap<int, std::string> m;
  EXPECT_FALSE(m.PollFirstEntry().has_value());
}

TEST(TreeMapTest, PollLastEntryRemovesAndReturns) {
  TreeMap<int, std::string> m;
  m.Put(3, "c");
  m.Put(1, "a");
  m.Put(2, "b");

  auto e = m.PollLastEntry();
  ASSERT_TRUE(e.has_value());
  EXPECT_EQ(e.value().Key(), 3);
  EXPECT_EQ(e.value().Value(), "c");
  EXPECT_EQ(m.Size(), 2UL);
  EXPECT_FALSE(m.ContainsKey(3));
}

TEST(TreeMapTest, PollLastEntryEmptyReturnsNullopt) {
  TreeMap<int, std::string> m;
  EXPECT_FALSE(m.PollLastEntry().has_value());
}

// ==================== View Method Tests ====================

TEST(TreeMapTest, HeadMapReturnsEntriesLessThanToKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");
  m.Put(30, "c");

  auto sub = m.HeadMap(25);
  EXPECT_EQ(sub.Size(), 2UL);
  EXPECT_TRUE(sub.ContainsKey(10));
  EXPECT_TRUE(sub.ContainsKey(20));
  EXPECT_FALSE(sub.ContainsKey(30));
}

TEST(TreeMapTest, HeadMapEmptyRange) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");

  auto sub = m.HeadMap(5);
  EXPECT_TRUE(sub.IsEmpty());
}

TEST(TreeMapTest, HeadMapInclusiveIncludesToKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");

  auto sub = m.HeadMap(20, true);
  EXPECT_EQ(sub.Size(), 2UL);
  EXPECT_TRUE(sub.ContainsKey(20));
}

TEST(TreeMapTest, HeadMapExclusiveExcludesToKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");

  auto sub = m.HeadMap(20, false);
  EXPECT_EQ(sub.Size(), 1UL);
  EXPECT_FALSE(sub.ContainsKey(20));
}

TEST(TreeMapTest, TailMapReturnsEntriesGreaterOrEqualToFromKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");
  m.Put(30, "c");

  auto sub = m.TailMap(20);
  EXPECT_EQ(sub.Size(), 2UL);
  EXPECT_TRUE(sub.ContainsKey(20));
  EXPECT_TRUE(sub.ContainsKey(30));
  EXPECT_FALSE(sub.ContainsKey(10));
}

TEST(TreeMapTest, TailMapEmptyRange) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");

  auto sub = m.TailMap(50);
  EXPECT_TRUE(sub.IsEmpty());
}

TEST(TreeMapTest, TailMapInclusiveIncludesFromKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");

  auto sub = m.TailMap(20, true);
  EXPECT_EQ(sub.Size(), 1UL);
  EXPECT_TRUE(sub.ContainsKey(20));
}

TEST(TreeMapTest, TailMapExclusiveExcludesFromKey) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");

  auto sub = m.TailMap(20, false);
  EXPECT_TRUE(sub.IsEmpty());
}

TEST(TreeMapTest, SubMapReturnsRange) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");
  m.Put(30, "c");
  m.Put(40, "d");

  auto sub = m.SubMap(15, 35);
  EXPECT_EQ(sub.Size(), 2UL);
  EXPECT_TRUE(sub.ContainsKey(20));
  EXPECT_TRUE(sub.ContainsKey(30));
  EXPECT_FALSE(sub.ContainsKey(10));
  EXPECT_FALSE(sub.ContainsKey(40));
}

TEST(TreeMapTest, SubMapEmptyRange) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");

  auto sub = m.SubMap(20, 30);
  EXPECT_TRUE(sub.IsEmpty());
}

TEST(TreeMapTest, SubMapWithInclusiveFlags) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");
  m.Put(30, "c");

  auto sub = m.SubMap(10, true, 30, true);
  EXPECT_EQ(sub.Size(), 3UL);
  EXPECT_TRUE(sub.ContainsKey(10));
  EXPECT_TRUE(sub.ContainsKey(20));
  EXPECT_TRUE(sub.ContainsKey(30));
}

TEST(TreeMapTest, SubMapWithExclusiveFlags) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");
  m.Put(30, "c");

  auto sub = m.SubMap(10, false, 30, false);
  EXPECT_EQ(sub.Size(), 1UL);
  EXPECT_TRUE(sub.ContainsKey(20));
}

TEST(TreeMapTest, DescendingMapReversesOrder) {
  TreeMap<int, std::string> m;
  m.Put(1, "a");
  m.Put(2, "b");
  m.Put(3, "c");

  auto desc = m.DescendingMap();
  EXPECT_EQ(desc.Size(), 3UL);

  std::vector<int> keys;
  desc.ForEach([&](const int& k, const std::string&) { keys.push_back(k); });

  ASSERT_EQ(keys.size(), 3UL);
  EXPECT_EQ(keys[0], 3);
  EXPECT_EQ(keys[1], 2);
  EXPECT_EQ(keys[2], 1);
}

TEST(TreeMapTest, DescendingMapOfEmptyMap) {
  TreeMap<int, std::string> m;
  auto desc = m.DescendingMap();
  EXPECT_TRUE(desc.IsEmpty());
}

TEST(TreeMapTest, ViewCopiesAreIndependent) {
  TreeMap<int, std::string> m;
  m.Put(10, "a");
  m.Put(20, "b");

  auto sub = m.HeadMap(30);
  sub.Put(30, "c");

  EXPECT_FALSE(m.ContainsKey(30));
  EXPECT_EQ(m.Size(), 2UL);
}

// ==================== Default Method Tests (inherited from Map) ====================

TEST(TreeMapTest, PutAll) {
  TreeMap<int, int> src;
  src.Put(1, 10);
  src.Put(2, 20);

  TreeMap<int, int> dst;
  dst.PutAll(src);

  EXPECT_EQ(dst.Size(), 2UL);
  EXPECT_EQ(dst.Get(1).value(), 10);
  EXPECT_EQ(dst.Get(2).value(), 20);
}

TEST(TreeMapTest, GetOrDefaultExisting) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  EXPECT_EQ(m.GetOrDefault(1, -1), 100);
}

TEST(TreeMapTest, GetOrDefaultMissing) {
  TreeMap<int, int> m;
  EXPECT_EQ(m.GetOrDefault(99, -1), -1);
}

TEST(TreeMapTest, PutIfAbsentWhenAbsent) {
  TreeMap<int, int> m;
  auto result = m.PutIfAbsent(1, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(m.Get(1).value(), 100);
}

TEST(TreeMapTest, PutIfAbsentWhenPresent) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  auto result = m.PutIfAbsent(1, 200);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 100);
  EXPECT_EQ(m.Get(1).value(), 100);
}

TEST(TreeMapTest, ReplaceByKeyWhenPresent) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  auto old = m.Replace(1, 200);
  ASSERT_TRUE(old.has_value());
  EXPECT_EQ(old.value(), 100);
  EXPECT_EQ(m.Get(1).value(), 200);
}

TEST(TreeMapTest, ReplaceByKeyWhenAbsent) {
  TreeMap<int, int> m;
  auto old = m.Replace(99, 200);
  EXPECT_FALSE(old.has_value());
  EXPECT_FALSE(m.ContainsKey(99));
}

TEST(TreeMapTest, ReplaceByKeyValueWhenMatches) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  EXPECT_TRUE(m.Replace(1, 100, 200));
  EXPECT_EQ(m.Get(1).value(), 200);
}

TEST(TreeMapTest, ReplaceAll) {
  TreeMap<int, int> m;
  m.Put(1, 10);
  m.Put(2, 20);
  m.ReplaceAll([](const int& key, const int& value) {
    return value * 10;
  });
  EXPECT_EQ(m.Get(1).value(), 100);
  EXPECT_EQ(m.Get(2).value(), 200);
}

TEST(TreeMapTest, ComputeIfAbsentWhenAbsent) {
  TreeMap<int, int> m;
  int val = m.ComputeIfAbsent(1, [](const int& key) { return key * 10; });
  EXPECT_EQ(val, 10);
  EXPECT_EQ(m.Get(1).value(), 10);
}

TEST(TreeMapTest, ComputeIfPresentWhenPresent) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  auto result = m.ComputeIfPresent(
      1, [](const int& key, const int& val) { return val * 2; });
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 200);
  EXPECT_EQ(m.Get(1).value(), 200);
}

TEST(TreeMapTest, ComputeInsertsNewValue) {
  TreeMap<int, int> m;
  auto result = m.Compute(1,
      [](const int& key, std::optional<int> old) {
        return std::optional<int>(key * 10);
      });
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 10);
  EXPECT_EQ(m.Get(1).value(), 10);
}

TEST(TreeMapTest, ComputeRemovesWhenNullopt) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  auto result = m.Compute(1,
      [](const int& key, std::optional<int> old) {
        return std::optional<int>();
      });
  EXPECT_FALSE(result.has_value());
  EXPECT_FALSE(m.ContainsKey(1));
}

TEST(TreeMapTest, MergeWhenKeyAbsent) {
  TreeMap<int, int> m;
  auto result = m.Merge(1, 100,
      [](const int& old_val, const int& new_val) { return old_val + new_val; });
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 100);
}

TEST(TreeMapTest, MergeWhenKeyPresent) {
  TreeMap<int, int> m;
  m.Put(1, 100);
  auto result = m.Merge(1, 200,
      [](const int& old_val, const int& new_val) { return old_val + new_val; });
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 300);
  EXPECT_EQ(m.Get(1).value(), 300);
}

TEST(TreeMapTest, ForEach) {
  TreeMap<int, int> m;
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

// ==================== Custom Comparator Tests ====================

TEST(TreeMapTest, ReverseOrderComparator) {
  TreeMap<int, std::string, std::greater<int>> m;
  m.Put(10, "a");
  m.Put(20, "b");
  m.Put(30, "c");

  std::vector<int> keys;
  m.ForEach([&](const int& k, const std::string&) { keys.push_back(k); });

  ASSERT_EQ(keys.size(), 3UL);
  EXPECT_EQ(keys[0], 30);
  EXPECT_EQ(keys[1], 20);
  EXPECT_EQ(keys[2], 10);
}

TEST(TreeMapTest, CustomComparatorCaseInsensitive) {
  struct CaseInsensitiveCompare {
    int operator()(const std::string& a, const std::string& b) const {
      std::string la, lb;
      for (char c : a) la += static_cast<char>(std::tolower(c));
      for (char c : b) lb += static_cast<char>(std::tolower(c));
      if (la < lb) return -1;
      if (la > lb) return 1;
      return 0;
    }
  };

  TreeMap<std::string, int, CaseInsensitiveCompare> m;
  m.Put("Apple", 1);
  m.Put("banana", 2);
  m.Put("CHERRY", 3);

  std::vector<std::string> keys;
  m.ForEach(
      [&](const std::string& k, const int&) { keys.push_back(k); });

  ASSERT_EQ(keys.size(), 3UL);
  EXPECT_EQ(keys[0], "Apple");
  EXPECT_EQ(keys[1], "banana");
  EXPECT_EQ(keys[2], "CHERRY");
}

}  // namespace

}  // namespace jaclks::javac_base
