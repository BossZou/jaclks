#include "function/compare.h"

#include <gtest/gtest.h>

namespace jaclks {

TEST(Compare, IntegerComparator) {
  ASSERT_EQ(0, Comparator<int>{}(100, 100));
  ASSERT_EQ(-1, Comparator<int>{}(100, 200));
  ASSERT_EQ(1, Comparator<int>{}(100, -10000));
}

TEST(Compare, StringComparator) {
  {
    std::string s1 = "abcdefg";
    std::string s2 = "zxcvbnm";

    ASSERT_EQ(0, Comparator<std::string>{}(s1, s1));
    ASSERT_EQ(s1.compare(s2), Comparator<std::string>{}(s1, s2));
    ASSERT_EQ(s2.compare(s1), Comparator<std::string>{}(s2, s1));
  }
  {
    std::string s1 = "abcdefg";
    std::string s2 = "abcdefghij";
    ASSERT_EQ(0, Comparator<std::string>{}(s1, s1));
    ASSERT_EQ(s1.compare(s2), Comparator<std::string>{}(s1, s2));
    ASSERT_EQ(s2.compare(s1), Comparator<std::string>{}(s2, s1));
  }
}

TEST(Compare, StringViewComparator) {
  {
    std::string_view s1{"abcdefg"};
    std::string_view s2{"zxcvbnm"};

    ASSERT_EQ(0, Comparator<std::string_view>{}(s1, s1));
    ASSERT_EQ(s1.compare(s2), Comparator<std::string_view>{}(s1, s2));
    ASSERT_EQ(s2.compare(s1), Comparator<std::string_view>{}(s2, s1));
  }
  {
    std::string_view s1{"abcdefg"};
    std::string_view s2{"abcdefghij"};
    ASSERT_EQ(0, Comparator<std::string_view>{}(s1, s1));
    ASSERT_EQ(s1.compare(s2), Comparator<std::string_view>{}(s1, s2));
    ASSERT_EQ(s2.compare(s1), Comparator<std::string_view>{}(s2, s1));
  }
}

TEST(Compare, PointerComparator) {
  auto x = std::make_unique<int>(100);
  auto y = std::make_unique<int>(200);

  ASSERT_EQ(0, Comparator<int *>{}(x.get(), x.get()));
  ASSERT_EQ(-1, Comparator<int *>{}(x.get(), y.get()));
  ASSERT_EQ(1,
            Comparator<int *>{}(x.get(), std::make_unique<int>(-10000).get()));
}

TEST(Compare, SmartPointerComparator) {
  {
    using SmartT = std::unique_ptr<int>;

    auto x = std::make_unique<int>(100);
    auto y = std::make_unique<int>(200);

    ASSERT_EQ(0, Comparator<SmartT>{}(x, x));
    ASSERT_EQ(-1, Comparator<SmartT>{}(x, y));
    ASSERT_EQ(1, Comparator<SmartT>{}(x, std::make_unique<int>(-10000)));
  }
  {
    using SmartT = std::shared_ptr<int>;

    auto x = std::make_shared<int>(100);
    auto y = std::make_shared<int>(200);

    ASSERT_EQ(0, Comparator<SmartT>{}(x, x));
    ASSERT_EQ(-1, Comparator<SmartT>{}(x, y));
    ASSERT_EQ(1, Comparator<SmartT>{}(x, std::make_shared<int>(-10000)));
  }
}

}  // namespace jaclks
