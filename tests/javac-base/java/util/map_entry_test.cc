#include "jaclks/javac-base/java/util/map_entry.h"

#include <string>

#include <gtest/gtest.h>

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
