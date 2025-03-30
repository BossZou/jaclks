#include "hash/hasher.h"

#include <gtest/gtest.h>

namespace jaclks {

TEST(HasherTest, Integer) {
  ASSERT_EQ(100UL, Hasher<int>{}(100));
}

}
