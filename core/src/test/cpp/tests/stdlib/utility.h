#include "core/stdlib/utility.h"
#include "test/test.h"

class moveable {
 public:
  int val;
  moveable() : val{0} {}
  moveable(int val) : val{val} {}
  moveable(moveable&& other) { rtk::swap(val, other.val); }
};

int test_move() {
  moveable a{0x1BADBABE};
  moveable b{rtk::move(a)};
  EXPECT_NOT_EQUAL(a.val, 0x1BADBABE);  // why not 0?
  EXPECT_EQUAL(b.val, 0x1BADBABE);
  return 0;
}

int test_swap_ints() {
  int a = 2, b = 3;
  rtk::swap(a, b);
  EXPECT_EQUAL(a, 3);
  EXPECT_EQUAL(b, 2);
  return 0;
}

int test_swap_char_ptrs() {
  const char *a = "hello", *b = "world";
  rtk::swap(a, b);
  EXPECT_EQUAL(a, "world");
  EXPECT_EQUAL(b, "hello");
  return 0;
}

void stdlib_utility_tests() {
  TEST(test_move);
  TEST(test_swap_ints);
  TEST(test_swap_char_ptrs);
}