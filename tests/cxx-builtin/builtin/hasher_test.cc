#include "../../../include/jaclks/cxx-builtin/builtin/hasher.h"

#include <gtest/gtest.h>

namespace jaclks {

TEST(HasherTest, Integer) {
  ASSERT_EQ(std::hash<int>{}(100), Hasher<int>{}(100));
}

TEST(HasherTest, Pointer) {
  int val = 100;
  auto pval = new int(val);

  ASSERT_EQ(std::hash<int>{}(val), Hasher<decltype(pval)>{}(pval));

  delete pval;
  pval = nullptr;
  ASSERT_EQ(0UL, Hasher<decltype(pval)>{}(pval));
}

TEST(HasherTest, SmartPointer) {
  int val = 100;

  {
    auto pval = std::make_shared<int>(val);

    ASSERT_EQ(std::hash<int>{}(val), Hasher<decltype(pval)>{}(pval));

    pval = nullptr;
    ASSERT_EQ(0UL, Hasher<decltype(pval)>{}(pval));
  }

  {
    auto pval = std::make_unique<int>(val);

    ASSERT_EQ(std::hash<int>{}(val), Hasher<decltype(pval)>{}(pval));

    pval.reset();
    ASSERT_EQ(0UL, Hasher<decltype(pval)>{}(pval));
  }
}

}  // namespace jaclks
