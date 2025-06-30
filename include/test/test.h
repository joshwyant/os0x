#pragma once

#include <cstring>

namespace testk {
template <typename T1, typename T2>
bool equals(T1 a, T2 b) {
  return a == b;
}
template <>
bool equals(const char* a, const char* b) {
  return strcmp(a, b) == 0;
}
template <typename T1, typename T2>
bool gt(T1 a, T2 b) {
  return a > b;
}
template <typename T1, typename T2>
bool lt(T1 a, T2 b) {
  return a < b;
}
template <typename T1, typename T2>
bool gte(T1 a, T2 b) {
  return a >= b;
}
template <typename T1, typename T2>
bool lte(T1 a, T2 b) {
  return a <= b;
}
template <typename T>
bool evaluates_to_false(T val) {
  return !val;
}
template <typename T>
bool evaluates_to_true(T val) {
  return !!val;
}
template <typename T>
bool empty(T str) {
  return str == "";
}
template <>
bool empty(const char* str) {
  return strlen(str) == 0;
}

#define _EXPECT1(fn, msg, expression)                                 \
  do {                                                                \
    auto result = (expression);                                       \
    if (!(fn(result))) {                                              \
      if (testk::test_logging) {                                      \
        std::cout << __FILE__ ":" << __LINE__ << ": " << msg << "\n"; \
      }                                                               \
      return 1;                                                       \
    }                                                                 \
  } while (0)

#define _EXPECT2(fn, msg, a, b)                                       \
  do {                                                                \
    auto aresult = (a);                                               \
    auto bresult = (b);                                               \
    if (!(fn(aresult, bresult))) {                                    \
      if (testk::test_logging) {                                      \
        std::cout << __FILE__ ":" << __LINE__ << ": " << msg << "\n"; \
      }                                                               \
      return 1;                                                       \
    }                                                                 \
  } while (0)

#define EXPECT_EQUAL(expression, expected)                                 \
  _EXPECT2(                                                                \
      testk::equals,                                                       \
      "expected ((" #expression ") == " #expected ") but got " << aresult, \
      expression, expected)

#define EXPECT_NOT_EQUAL(expression, expected)                              \
  _EXPECT2(!testk::equals, "expected ((" #expression ") != " #expected ")", \
           expression, expected)

#define _EXPECT_ACTUAL(fn, op, a, b) \
  _EXPECT2(fn, "expected ((" #a ") " #op " " #b "), actual: " << aresult, a, b)

#define _EXPECT_NOT_ACTUAL(fn, op, a, b)                                       \
  _EXPECT2(!fn, "expected !((" #a ") " #op " " #b "), actual: " << aresult, a, \
           b)

#define EXPECT(expression)                                              \
  _EXPECT1(testk::evaluates_to_true,                                    \
           "expected (" #expression ") to be true, actual: " << result, \
           expression)

#define EXPECT_NOT(expression)                                           \
  _EXPECT1(testk::evaluates_to_false,                                    \
           "expected (" #expression ") to be false, actual: " << result, \
           expression)

#define EXPECT_EMPTY(expression)                                       \
  _EXPECT1(testk::empty,                                               \
           "expected ((" #expression ") == \"\"), actual: " << result, \
           expression)

#define EXPECT_NOT_EMPTY(expression) \
  _EXPECT1(!testk::empty, "expected ((" #expression ") != \"\")", expression)

#define EXPECT_NULL(expression) EXPECT_EQUAL(expression, nullptr);
#define EXPECT_M1(expression) EXPECT_EQUAL(expression, -1);
#define EXPECT_0(expression) EXPECT_EQUAL(expression, 0);
#define EXPECT_1(expression) EXPECT_EQUAL(expression, 1);
#define EXPECT_TRUE(expression) EXPECT_EQUAL(expression, true);
#define EXPECT_FALSE(expression) EXPECT_EQUAL(expression, false);

#define EXPECT_NONNULL(expression) EXPECT_NOT_EQUAL(expression, nullptr);
#define EXPECT_NOT_M1(expression) EXPECT_NOT_EQUAL(expression, -1);
#define EXPECT_NOT_0(expression) EXPECT_NOT_EQUAL(expression, 0);
#define EXPECT_NOT_1(expression) EXPECT_NOT_EQUAL(expression, 1);

#define EXPECT_GREATER_THAN(expression, constant) \
  _EXPECT_ACTUAL(testk::gt, >, expression, constant)
#define EXPECT_LESS_THAN(expression, constant) \
  _EXPECT_ACTUAL(testk::lt, <, expression, constant)
#define EXPECT_GREATER_THAN_OR_EQUAL(expression, constant) \
  _EXPECT_ACTUAL(testk::gte, >=, expression, constant)
#define EXPECT_LESS_THAN_OR_EQUAL(expression, constant) \
  _EXPECT_ACTUAL(testk::lte, <=, expression, constant)

#define EXPECT_NOT_GREATER_THAN(expression, constant) \
  _EXPECT_NOT_ACTUAL(testk::gt, >, expression, constant)
#define EXPECT_NOT_LESS_THAN(expression, constant) \
  _EXPECT_NOT_ACTUAL(testk::lt, <, expression, constant)
#define EXPECT_NOT_GREATER_THAN_OR_EQUAL(expression, constant) \
  _EXPECT_NOT_ACTUAL(testk::gte, >=, expression, constant)
#define EXPECT_NOT_LESS_THAN_OR_EQUAL(expression, constant) \
  _EXPECT_NOT_ACTUAL(testk::lte, <=, expression, constant)

#define EXPECT_FAIL(testname)                                       \
  do {                                                              \
    auto prev_logging = testk::test_logging;                        \
    testk::test_logging = false;                                    \
    auto testresult = testname();                                   \
    testk::test_logging = prev_logging;                             \
    if (testresult == 0) {                                          \
      if (testk::test_logging) {                                    \
        std::cout << __FILE__ ":" << __LINE__ << ": "               \
                  << "expected test \"" #testname "()\" to fail\n"; \
      }                                                             \
      return 1;                                                     \
    }                                                               \
  } while (0)

#define EXPECT_SUCCESS(testname)                                          \
  _EXPECT2(testk::equals, "expected test \"" #testname "()\" to succeed", \
           testname(), 0)

#define TEST(testname)                          \
  do {                                          \
    std::cout << "--- Test: " #testname "()\n"; \
    if (testname() == 0) {                      \
      testk::successful_tests++;                \
      std::cout << "Success.\n\n";              \
    } else {                                    \
      testk::failed_tests++;                    \
      std::cout << "Fail.\n\n";                 \
    }                                           \
  } while (0)

extern bool test_logging;
extern int failed_tests;
extern int successful_tests;
}  // namespace testk