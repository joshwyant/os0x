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

int test_false_equal_m1_fails() {
  EXPECT_EQUAL(false, -1);
  return 0;
}

int test_true_not_equal_1_fails() {
  EXPECT_NOT_EQUAL(true, 1);
  return 0;
}

int test_false_not_equal_0_fails() {
  EXPECT_NOT_EQUAL(false, 0);
  return 0;
}

int test_expect_success_fails() {
  return 0;
}

int test_expect_empty_fails() {
  return 0;
}

int test_expect_not_empty_fails() {
  return 0;
}

int test_expect_null_fails() {
  return 0;
}

int test_expect_m1_fails() {
  return 0;
}

int test_expect_0_fails() {
  return 0;
}

int test_expect_1_fails() {
  return 0;
}

int test_expect_true_fails() {
  return 0;
}

int test_expect_false_fails() {
  return 0;
}

int test_expect_nonnull_fails() {
  return 0;
}

int test_expect_not_m1_fails() {
  return 0;
}

int test_expect_not_0_fails() {
  return 0;
}

int test_expect_not_1_fails() {
  return 0;
}

int test_expect_greater_than_fails() {
  return 0;
}

int test_expect_less_than_fails() {
  return 0;
}

int test_expect_greater_than_or_equal_fails() {
  return 0;
}

int test_expect_less_than_or_equal_fails() {
  return 0;
}

int test_expect_not_greater_than_fails() {
  return 0;
}

int test_expect_not_less_than_fails() {
  return 0;
}

int test_expect_not_greater_than_or_equal_fails() {
  return 0;
}

int test_expect_not_less_than_or_equal_fails() {
  return 0;
}

int test_expect_success() {
  return 1;
}

int test_true_equal_0() {
  return 1;
}

int test_false_equal_m1() {
  return 1;
}

int test_true_not_equal_1() {
  return 1;
}

int test_false_not_equal_0() {
  return 1;
}

int test_expect_empty() {
  return 1;
}

int test_expect_not_empty() {
  return 1;
}

int test_expect_null() {
  return 1;
}

int test_expect_m1() {
  return 1;
}

int test_expect_0() {
  return 1;
}

int test_expect_1() {
  return 1;
}

int test_expect_true() {
  return 1;
}

int test_expect_false() {
  return 1;
}

int test_expect_nonnull() {
  return 1;
}

int test_expect_not_m1() {
  return 1;
}

int test_expect_not_0() {
  return 1;
}

int test_expect_not_1() {
  return 1;
}

int test_expect_greater_than() {
  return 1;
}

int test_expect_less_than() {
  return 1;
}

int test_expect_greater_than_or_equal() {
  return 1;
}

int test_expect_less_than_or_equal() {
  return 1;
}

int test_expect_not_greater_than() {
  return 1;
}

int test_expect_not_less_than() {
  return 1;
}

int test_expect_not_greater_than_or_equal() {
  return 1;
}

int test_expect_not_less_than_or_equal() {
  return 1;
}

int test() {
  return 1;
}

int failure_tests() {
  EXPECT_FAIL(test_expect_fail_fails);
  EXPECT_FAIL(test_expect_success_fails);
  EXPECT_FAIL(test_true_equal_0_fails);
  EXPECT_FAIL(test_false_equal_m1_fails);
  EXPECT_FAIL(test_true_not_equal_1_fails);
  EXPECT_FAIL(test_false_not_equal_0_fails);
  EXPECT_FAIL(test_expect_empty_fails);
  EXPECT_FAIL(test_expect_not_empty_fails);
  EXPECT_FAIL(test_expect_null_fails);
  EXPECT_FAIL(test_expect_m1_fails);
  EXPECT_FAIL(test_expect_0_fails);
  EXPECT_FAIL(test_expect_1_fails);
  EXPECT_FAIL(test_expect_true_fails);
  EXPECT_FAIL(test_expect_false_fails);
  EXPECT_FAIL(test_expect_nonnull_fails);
  EXPECT_FAIL(test_expect_not_m1_fails);
  EXPECT_FAIL(test_expect_not_0_fails);
  EXPECT_FAIL(test_expect_not_1_fails);
  EXPECT_FAIL(test_expect_greater_than_fails);
  EXPECT_FAIL(test_expect_less_than_fails);
  EXPECT_FAIL(test_expect_greater_than_or_equal_fails);
  EXPECT_FAIL(test_expect_less_than_or_equal_fails);
  EXPECT_FAIL(test_expect_not_greater_than_fails);
  EXPECT_FAIL(test_expect_not_less_than_fails);
  EXPECT_FAIL(test_expect_not_greater_than_or_equal_fails);
  EXPECT_FAIL(test_expect_not_less_than_or_equal_fails);
  return 0;
}

int tests() {
  EXPECT_SUCCESS(successful_test);
  EXPECT_SUCCESS(test_expect_fail);
  EXPECT_SUCCESS(test_expect_success);
  EXPECT_SUCCESS(test_true_equal_0);
  EXPECT_SUCCESS(test_false_equal_m1);
  EXPECT_SUCCESS(test_true_not_equal_1);
  EXPECT_SUCCESS(test_false_not_equal_0);
  EXPECT_SUCCESS(test_expect_empty);
  EXPECT_SUCCESS(test_expect_not_empty);
  EXPECT_SUCCESS(test_expect_null);
  EXPECT_SUCCESS(test_expect_m1);
  EXPECT_SUCCESS(test_expect_0);
  EXPECT_SUCCESS(test_expect_1);
  EXPECT_SUCCESS(test_expect_true);
  EXPECT_SUCCESS(test_expect_false);
  EXPECT_SUCCESS(test_expect_nonnull);
  EXPECT_SUCCESS(test_expect_not_m1);
  EXPECT_SUCCESS(test_expect_not_0);
  EXPECT_SUCCESS(test_expect_not_1);
  EXPECT_SUCCESS(test_expect_greater_than);
  EXPECT_SUCCESS(test_expect_less_than);
  EXPECT_SUCCESS(test_expect_greater_than_or_equal);
  EXPECT_SUCCESS(test_expect_less_than_or_equal);
  EXPECT_SUCCESS(test_expect_not_greater_than);
  EXPECT_SUCCESS(test_expect_not_less_than);
  EXPECT_SUCCESS(test_expect_not_greater_than_or_equal);
  EXPECT_SUCCESS(test_expect_not_less_than_or_equal);
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