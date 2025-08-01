#include "hash/hasher.h"

#include <gtest/gtest.h>

namespace jaclks {

TEST(HasherTest, Integer) {
  ASSERT_EQ(100UL, Hasher<int>{}(100));
}

TEST(HasherTest, Pointer) {
  int val = 100;
  auto pval = new int(val);

  ASSERT_EQ(static_cast<std::size_t>(val), Hasher<decltype(pval)>{}(pval));

  delete pval;
  pval = nullptr;
  ASSERT_EQ(0UL, Hasher<decltype(pval)>{}(pval));
}

TEST(HasherTest, SmartPointer) {
  int val = 100;

  {
    auto pval = std::make_shared<int>(val);

    ASSERT_EQ(static_cast<std::size_t>(val), Hasher<decltype(pval)>{}(pval));

    pval = nullptr;
    ASSERT_EQ(0UL, Hasher<decltype(pval)>{}(pval));
  }

  {
    auto pval = std::make_unique<int>(val);

    ASSERT_EQ(static_cast<std::size_t>(val), Hasher<decltype(pval)>{}(pval));

    pval = nullptr;
    ASSERT_EQ(0UL, Hasher<decltype(pval)>{}(pval));
  }
}

}  // namespace jaclks
