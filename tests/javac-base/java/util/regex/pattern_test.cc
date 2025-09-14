#include "jaclks/javac-base/java/util/regex/pattern.h"

#include <gtest/gtest.h>

namespace jaclks::javac_base {

namespace {

class PatternTest : public ::testing::Test {
 public:
  template <typename E>
  void AssertVectorEquals(const std::vector<E> &expected,
                          const std::vector<E> &actual) {
    auto se = expected.size();
    auto sa = actual.size();

    ASSERT_EQ(se, sa) << "expected size: " << se << ", actual size: " << sa;

    for (std::size_t i = 0; i < expected.size(); ++i) {
      ASSERT_EQ(expected[i], actual[i]);
    }
  }
};

}  // namespace

TEST_F(PatternTest, Normal) {
  auto regex = Pattern::Compile("123");
}

TEST_F(PatternTest, Matches) {
  ASSERT_TRUE(Pattern::Matches("\\d+", "1234567890"));
  ASSERT_FALSE(Pattern::Matches("\\d+", "123456x7890"));
}

TEST_F(PatternTest, BasicSplit) {
  auto pattern = Pattern::Compile(" ");
  String input{"hello world boost regex", true};
  std::vector<String> expected{"hello", "world", "boost", "regex"};

  auto result = pattern.Split(input);
  AssertVectorEquals<String>(expected, result);
}

TEST_F(PatternTest, SpliterSplit) {
  auto pattern = Pattern::Compile("TAKE SEL|TAKE|NAME");
  String input{"NAME some name stuff\nTAKE some take stuff\nTAKE SEL some take "
               "sel stuff\n",
               true};
  std::vector<String> expected{
      "", " some name stuff\n", " some take stuff\n", " some take sel stuff\n"};

  auto result = pattern.Split(input);
  AssertVectorEquals<String>(expected, result);
}

TEST_F(PatternTest, NamedGroup) {
  {
    auto pattern = Pattern::Compile("(?<year>\\d{4})");
    auto matcher = pattern.Matcher("2023");
    ASSERT_TRUE(matcher.Matches());
    ASSERT_EQ(String{"2023"}, matcher.Group("year"));
  }
  {
    auto pattern =
        Pattern::Compile("Name: (?<name>[a-zA-Z ]+), Age: (?<age>\\d+)");
    auto matcher = pattern.Matcher("Name: John Doe, Age: 30");
    ASSERT_TRUE(matcher.Matches());
    ASSERT_EQ((String{"John Doe", true}), matcher.Group("name"));
    ASSERT_EQ((String{"30", true}), matcher.Group("age"));
  }
}

TEST_F(PatternTest, FindIndexGroup) {
  auto pattern = Pattern::Compile("(?<year>\\d{4})-(?<month>\\d{2})-(?<day>\\d{2})");
  auto matcher = pattern.Matcher("2023-01-02 AND 2025-09-10");

  ASSERT_TRUE(matcher.Find());
  ASSERT_EQ(String{"2023-01-02"}, matcher.Group(0));
  ASSERT_EQ(String{"2023"}, matcher.Group(1));
  ASSERT_EQ(String{"01"}, matcher.Group(2));
  ASSERT_EQ(String{"02"}, matcher.Group(3));

  ASSERT_TRUE(matcher.Find());
  ASSERT_EQ(String{"2025-09-10"}, matcher.Group(0));
  ASSERT_EQ(String{"2025"}, matcher.Group(1));
  ASSERT_EQ(String{"09"}, matcher.Group(2));
  ASSERT_EQ(String{"10"}, matcher.Group(3));
}

TEST_F(PatternTest, FindNamedGroup) {
  auto pattern = Pattern::Compile("(?<year>\\d{4})");
  auto matcher = pattern.Matcher("2023-2024-2025");

  ASSERT_TRUE(matcher.Find());
  ASSERT_EQ(String{"2023"}, matcher.Group("year"));
  ASSERT_TRUE(matcher.Find());
  ASSERT_EQ(String{"2024"}, matcher.Group("year"));
  ASSERT_TRUE(matcher.Find());
  ASSERT_EQ(String{"2025"}, matcher.Group("year"));
}

}  // namespace jaclks::javac_base
