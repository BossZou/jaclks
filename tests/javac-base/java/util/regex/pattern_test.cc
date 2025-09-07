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

}  // namespace jaclks::javac_base
