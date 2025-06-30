#include <iostream>

#include "core/stdlib/string.h"
#include "test/test.h"

bool testk::test_logging = true;
int testk::successful_tests = 0;
int testk::failed_tests = 0;

int successful_test() {
  return 0;
}
int failing_test() {
  return 1;
}

int test_expect_fail_fails() {
  EXPECT_FAIL(successful_test);
  return 0;
}

int test_expect_fail() {
  EXPECT_FAIL(failing_test);
  return 0;
}

int test_true_equal_0_fails() {
  EXPECT_EQUAL(true, 0);
  return 0;
}

int failure_tests() {
  EXPECT_FAIL(test_expect_fail_fails);
  EXPECT_FAIL(test_true_equal_0_fails);
  return 0;
}

int tests() {
  EXPECT_SUCCESS(successful_test);
  EXPECT_SUCCESS(test_expect_fail);
  EXPECT_EQUAL(true, 1);
  EXPECT_EQUAL(false, 0);
  EXPECT_EQUAL(1, 1);

  EXPECT_NOT_EQUAL(true, 2);
  EXPECT_NOT_EQUAL(true, -1);
  EXPECT_NOT_EQUAL(false, -1);
  EXPECT_NOT_EQUAL(1, 2);

  EXPECT("");
  EXPECT(true);
  EXPECT(1);

  // EXPECT_NOT(nullptr); // generates warning
  EXPECT_NOT(false);
  EXPECT_NOT(0);

  EXPECT_TRUE(1);
  EXPECT_TRUE(true);

  EXPECT_FALSE(0);
  EXPECT_FALSE(false);

  EXPECT_EMPTY("");
  EXPECT_NOT_EMPTY(" ");

  return 0;
}

int main(int argc, const char** argv) {
  TEST(failure_tests);
  TEST(tests);

  std::cout << testk::successful_tests << " succeeded, " << testk::failed_tests
            << " failed." << std::endl;

  return testk::failed_tests;
}