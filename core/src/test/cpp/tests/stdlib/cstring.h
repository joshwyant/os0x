#include <algorithm>
#include "core/stdlib/cstring.h"

int test_strlen_nulltpr() {
  EXPECT_0(rtk::strlen(nullptr));
  return 0;
}

int test_strlen_empty() {
  EXPECT_0(rtk::strlen(""));
  return 0;
}

int test_strlen_hello() {
  EXPECT_EQUAL(rtk::strlen("hello"), 5);
  return 0;
}

int test_strlen_constexpr() {
  constexpr auto len = rtk::strlen("hello");
  static_assert(len == 5);
  return 0;
}

int test_strcpy() {
  char dest[32];
  char src[] = "hello";
  std::fill(std::begin(dest), std::end(dest), 1);
  auto result = rtk::strcpy(dest, src);
  void* aptr = result;
  void* bptr = dest;
  EXPECT_EQUAL((const char*)src, "hello");  // not overwritten
  EXPECT_EQUAL((const char*)dest, "hello");
  EXPECT_EQUAL(aptr, bptr);  // result should be dest
  EXPECT_0(dest[5]);         // null terminator
  EXPECT_1(dest[6]);         // not overwritten
  return 0;
}
int test_strcpy_nulls() {
  char mystr[]{"hello"};
  EXPECT_NONNULL(rtk::strcpy(mystr, nullptr));
  EXPECT_NULL(rtk::strcpy(nullptr, "HELLO"));
  EXPECT_NULL(rtk::strcpy(nullptr, nullptr));
  EXPECT_EQUAL((const char*)mystr, "hello");
  return 0;
}
int test_strcpy_overlap() {
  char src[]{"hello"};
  void* result = rtk::strcpy(src, src);
  EXPECT_EQUAL(result, src);
  EXPECT_EQUAL((const char*)src, "hello");
  return 0;
}

int test_strncpy() {
  char dest[32];
  char src[] = "hello";
  std::fill(std::begin(dest), std::end(dest), 1);
  auto result = rtk::strncpy(dest, src, sizeof(dest));
  void* aptr = result;
  void* bptr = dest;
  EXPECT_EQUAL((const char*)src, "hello");  // not overwritten
  EXPECT_EQUAL((const char*)dest, "hello");
  EXPECT_EQUAL(aptr, bptr);  // result should be dest
  for (auto i = sizeof("hello") - 1; i < sizeof(dest); i++) {
    EXPECT_0(dest[i]);  // filled with 0s
  }
  return 0;
}
int test_strncpy_nulls() {
  char mystr[]{"hello"};
  EXPECT_NONNULL(rtk::strncpy(mystr, nullptr, sizeof(mystr)));
  EXPECT_NULL(rtk::strncpy(nullptr, "HELLO", sizeof(mystr)));
  EXPECT_NULL(rtk::strncpy(nullptr, nullptr, sizeof(mystr)));
  EXPECT_EQUAL((const char*)mystr, "hello");
  return 0;
}
int test_strncpy_overlap() {
  char src[]{"hello"};
  void* result = rtk::strncpy(src, src, sizeof(src));
  EXPECT_EQUAL(result, src);
  EXPECT_EQUAL((const char*)src, "hello");
  return 0;
}

int test_strncpy_small() {
  char dest[]{"Hello, world!"};
  rtk::strncpy(dest, "HELLO", 5);
  EXPECT_EQUAL((const char*)dest,
               "HELLO, world!");  // not prematurely terminated
  return 0;
}

void stdlib_cstring_tests() {
  // strlen
  TEST(test_strlen_nulltpr);
  TEST(test_strlen_empty);
  TEST(test_strlen_hello);
  TEST(test_strlen_constexpr);
  // strcpy
  TEST(test_strcpy);
  TEST(test_strcpy_nulls);
  TEST(test_strcpy_overlap);
  // strncpy
  TEST(test_strncpy);
  TEST(test_strncpy_nulls);
  TEST(test_strncpy_overlap);
  TEST(test_strncpy_small);
}