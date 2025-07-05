#include "core/stdlib/string.h"
#include "core/stdlib/utility.h"
#include "test/test.h"

namespace rtk {
class string_tests {
  static constexpr auto kAbc = "abcdefghijklmnopqrstuvwxyz";
  static constexpr auto kAbcLen = rtk::strlen(kAbc);
  static constexpr auto kHello = "hello";
  static constexpr auto kHelloLen = rtk::strlen(kHello);

 public:
  static int test_string_short() {
    string hello{kHello};
    EXPECT_TRUE(hello.is_short_);
    EXPECT_EQUAL(hello.length(), hello.shortlen_);
    EXPECT_EQUAL(hello.shortlen_, kHelloLen);
    EXPECT_EQUAL((const char*)hello.shortstr_, kHello);
    return 0;
  }
  static int test_string_long() {
    string abc{kAbc};
    EXPECT_FALSE(abc.is_short_);
    EXPECT_EQUAL(abc.length(), abc.len_);
    EXPECT_EQUAL(abc.len_, kAbcLen);
    EXPECT_EQUAL((const char*)abc.cstr_.get(), (const char*)abc.c_str());
    EXPECT_EQUAL((const char*)abc.cstr_.get(), kAbc);
    return 0;
  }
  static int test_string_copy_long_to_short() {
    string hello{kHello};
    string abc{kAbc};
    hello = abc;
    EXPECT_FALSE(hello.is_short_);
    EXPECT_EQUAL(hello.len_, kAbcLen);
    EXPECT_EQUAL((const char*)hello.c_str(), kAbc);
    return 0;
  }
  static int test_string_copy_short_to_long() {
    string abc{kAbc};
    string hello{kHello};
    abc = hello;
    EXPECT_TRUE(abc.is_short_);
    EXPECT_EQUAL(abc.length(), kHelloLen);
    EXPECT_EQUAL((const char*)abc.c_str(), kHello);
    return 0;
  }
  static int test_string_move_long_to_short() {
    string hello{kHello};
    string abc{kAbc};
    hello = move(abc);
    EXPECT_FALSE(hello.is_short_);
    EXPECT_EQUAL(hello.shortlen_, kAbcLen);
    EXPECT_EQUAL((const char*)hello.c_str(), kAbc);

    // abc should be reset
    EXPECT_TRUE(abc.is_short_);
    EXPECT_EQUAL(abc.shortlen_, 0);
    EXPECT_EQUAL((const char*)abc.c_str(), "");
    return 0;
  }
  static int test_string_move_short_to_long() {
    string abc{kAbc};
    string hello{kHello};

    abc = move(hello);

    EXPECT_TRUE(abc.is_short_);
    EXPECT_EQUAL(abc.shortlen_, kHelloLen);
    EXPECT_EQUAL((const char*)abc.c_str(), kHello);
    // Hello should be reset
    EXPECT_TRUE(hello.is_short_);
    EXPECT_EQUAL(hello.shortlen_, 0);
    EXPECT_EQUAL((const char*)hello.c_str(), "");
    return 0;
  }
  static int test_string_append() {
#define A "Hello, "
#define B "world!"
#define C " This is a really long string."
#define D "\n"
    string a{A};
    string b{B};
    a.append(b);  // Add short string instance to short string
    EXPECT_EQUAL(rtk::as_const(a).c_str(), A B);
    a.append(C);  // add long string to short string
    EXPECT_EQUAL(rtk::as_const(a).c_str(), A B C);
    a.append(D);  // add short string to long string
    EXPECT_EQUAL(rtk::as_const(a).c_str(), A B C D);
    return 0;
  };
  static int test_string_concat_operator() {
    string a{A};
    string b{B};
    a = a + b;  // Add short string instance to short string
    EXPECT_EQUAL(rtk::as_const(a).c_str(), A B);
    a = a + C;  // add long string to short string
    EXPECT_EQUAL(rtk::as_const(a).c_str(), A B C);
    a = a + D;  // add short string to long string
    EXPECT_EQUAL(rtk::as_const(a).c_str(), A B C D);
    return 0;
#undef A
#undef B
#undef C
  };
  static void stdlib_string_tests() {
    TEST(test_string_short);
    TEST(test_string_long);
    TEST(test_string_copy_short_to_long);
    TEST(test_string_copy_long_to_short);
    TEST(test_string_move_short_to_long);
    TEST(test_string_move_long_to_short);
    TEST(test_string_append);
    TEST(test_string_concat_operator);
  }
};  // class string_tests
}  // namespace rtk

void stdlib_string_tests() {
  rtk::string_tests::stdlib_string_tests();
}