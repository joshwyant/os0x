#include <iostream>

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
  EXPECT_SUCCESS(failing_test);
  return 0;
}

int test_expect_empty_fails() {
  EXPECT_EMPTY(".");
  return 0;
}

int test_expect_not_empty_fails() {
  EXPECT_NOT_EMPTY("");
  return 0;
}

int test_expect_null_fails() {
  EXPECT_NULL("");
  return 0;
}

int test_expect_m1_fails() {
  EXPECT_M1(1);
  return 0;
}

int test_expect_0_fails() {
  EXPECT_0(1);
  return 0;
}

int test_expect_1_fails() {
  EXPECT_1(-1);
  return 0;
}

int test_expect_true_fails() {
  EXPECT_TRUE(false);
  return 0;
}

int test_expect_false_fails() {
  EXPECT_FALSE(true);
  return 0;
}

int test_expect_nonnull_fails() {
  EXPECT_NONNULL(nullptr);
  return 0;
}

int test_expect_not_m1_fails() {
  EXPECT_NOT_M1(-1);
  return 0;
}

int test_expect_not_0_fails() {
  EXPECT_NOT_0(0);
  return 0;
}

int test_expect_not_1_fails() {
  EXPECT_NOT_1(1);
  return 0;
}

int test_expect_greater_than_fails() {
  EXPECT_GREATER_THAN(0, 1);
  return 0;
}

int test_expect_less_than_fails() {
  EXPECT_LESS_THAN(1, 0);
  return 0;
}

int test_expect_greater_than_or_equal_fails() {
  EXPECT_GREATER_THAN_OR_EQUAL(0, 1);
  return 0;
}

int test_expect_less_than_or_equal_fails() {
  EXPECT_LESS_THAN_OR_EQUAL(1, 0);
  return 0;
}

int test_expect_not_greater_than_fails() {
  EXPECT_NOT_GREATER_THAN(1, 0);
  return 0;
}

int test_expect_not_less_than_fails() {
  EXPECT_NOT_LESS_THAN(0, 1);
  return 0;
}

int test_expect_not_greater_than_or_equal_fails() {
  EXPECT_NOT_GREATER_THAN_OR_EQUAL(1, 0);
  return 0;
}

int test_expect_not_less_than_or_equal_fails() {
  EXPECT_NOT_LESS_THAN_OR_EQUAL(0, 1);
  return 0;
}

int test_expect_success() {
  EXPECT_SUCCESS(successful_test);
  return 0;
}

int test_true_equal_1() {
  EXPECT_EQUAL(true, 1);
  return 0;
}

int test_false_equal_0() {
  EXPECT_EQUAL(false, 0);
  return 0;
}

int test_true_not_equal_0() {
  EXPECT_NOT_EQUAL(true, 0);
  return 0;
}

int test_false_not_equal_1() {
  EXPECT_NOT_EQUAL(false, 1);
  return 0;
}

int test_expect_empty() {
  EXPECT_EMPTY("");
  return 0;
}

int test_expect_not_empty() {
  EXPECT_NOT_EMPTY(".");
  return 0;
}

int test_expect_null() {
  const char* str = nullptr;
  EXPECT_NULL(str);
  return 0;
}

int test_expect_m1() {
  EXPECT_M1(-1);
  return 0;
}

int test_expect_0() {
  EXPECT_0(0);
  return 0;
}

int test_expect_1() {
  EXPECT_1(1);
  return 0;
}

int test_expect_true() {
  EXPECT_TRUE(true);
  return 0;
}

int test_expect_false() {
  EXPECT_FALSE(false);
  return 0;
}

int test_expect_nonnull() {
  const char* str = ".";
  EXPECT_NONNULL(str);
  return 0;
}

int test_expect_not_m1() {
  EXPECT_NOT_M1(0);
  return 0;
}

int test_expect_not_0() {
  EXPECT_NOT_0(-1);
  return 0;
}

int test_expect_not_1() {
  EXPECT_NOT_1(0);
  return 0;
}

int test_expect_greater_than() {
  EXPECT_GREATER_THAN(1, 0);
  return 0;
}

int test_expect_less_than() {
  EXPECT_LESS_THAN(0, 1);
  return 0;
}

int test_expect_greater_than_or_equal() {
  EXPECT_GREATER_THAN_OR_EQUAL(0, 0);
  EXPECT_GREATER_THAN_OR_EQUAL(1, 0);
  return 0;
}

int test_expect_less_than_or_equal() {
  EXPECT_LESS_THAN_OR_EQUAL(0, 1);
  EXPECT_LESS_THAN_OR_EQUAL(1, 1);
  return 0;
}

int test_expect_not_greater_than() {
  EXPECT_NOT_GREATER_THAN(0, 1);
  EXPECT_NOT_GREATER_THAN(1, 1);
  return 0;
}

int test_expect_not_less_than() {
  EXPECT_NOT_LESS_THAN(0, 0);
  EXPECT_NOT_LESS_THAN(1, 0);
  return 0;
}

int test_expect_not_greater_than_or_equal() {
  EXPECT_NOT_GREATER_THAN_OR_EQUAL(0, 1);
  return 0;
}

int test_expect_not_less_than_or_equal() {
  EXPECT_NOT_LESS_THAN_OR_EQUAL(1, 0);
  return 0;
}

void failure_tests() {
  FAIL_TEST(test_expect_fail_fails);
  FAIL_TEST(test_expect_success_fails);
  FAIL_TEST(test_true_equal_0_fails);
  FAIL_TEST(test_false_equal_m1_fails);
  FAIL_TEST(test_true_not_equal_1_fails);
  FAIL_TEST(test_false_not_equal_0_fails);
  FAIL_TEST(test_expect_empty_fails);
  FAIL_TEST(test_expect_not_empty_fails);
  FAIL_TEST(test_expect_null_fails);
  FAIL_TEST(test_expect_m1_fails);
  FAIL_TEST(test_expect_0_fails);
  FAIL_TEST(test_expect_1_fails);
  FAIL_TEST(test_expect_true_fails);
  FAIL_TEST(test_expect_false_fails);
  FAIL_TEST(test_expect_nonnull_fails);
  FAIL_TEST(test_expect_not_m1_fails);
  FAIL_TEST(test_expect_not_0_fails);
  FAIL_TEST(test_expect_not_1_fails);
  FAIL_TEST(test_expect_greater_than_fails);
  FAIL_TEST(test_expect_less_than_fails);
  FAIL_TEST(test_expect_greater_than_or_equal_fails);
  FAIL_TEST(test_expect_less_than_or_equal_fails);
  FAIL_TEST(test_expect_not_greater_than_fails);
  FAIL_TEST(test_expect_not_less_than_fails);
  FAIL_TEST(test_expect_not_greater_than_or_equal_fails);
  FAIL_TEST(test_expect_not_less_than_or_equal_fails);
}

void succeeding_tests() {
  TEST(successful_test);
  TEST(test_expect_fail);
  TEST(test_expect_success);
  TEST(test_true_equal_1);
  TEST(test_false_equal_0);
  TEST(test_true_not_equal_0);
  TEST(test_false_not_equal_1);
  TEST(test_expect_empty);
  TEST(test_expect_not_empty);
  TEST(test_expect_null);
  TEST(test_expect_m1);
  TEST(test_expect_0);
  TEST(test_expect_1);
  TEST(test_expect_true);
  TEST(test_expect_false);
  TEST(test_expect_nonnull);
  TEST(test_expect_not_m1);
  TEST(test_expect_not_0);
  TEST(test_expect_not_1);
  TEST(test_expect_greater_than);
  TEST(test_expect_less_than);
  TEST(test_expect_greater_than_or_equal);
  TEST(test_expect_less_than_or_equal);
  TEST(test_expect_not_greater_than);
  TEST(test_expect_not_less_than);
  TEST(test_expect_not_greater_than_or_equal);
  TEST(test_expect_not_less_than_or_equal);
}

int main(int argc, const char** argv) {
  failure_tests();
  succeeding_tests();

  std::cout << "\n"
            << testk::successful_tests << " succeeded, " << testk::failed_tests
            << " failed." << std::endl;

  return testk::failed_tests;
}