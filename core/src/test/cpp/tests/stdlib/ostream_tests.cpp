#include "core/stdlib/iomanip.h"
#include "core/stdlib/ostream.h"
#include "core/stdlib/sstream.h"
#include "test/test.h"

namespace rtk {
class ostream_tests {
 public:
  static int test_sstream() {
    ostringstream ss;
    ss << "Hello," << " " << "world!" << rtk::endl;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "Hello, world!\n");

    return 0;
  }
  static int test_ostream_hex() {
    ostringstream ss1;
    ss1 << hex << 0x1badbabeUL;
    EXPECT_EQUAL(as_const(ss1.str()).c_str(), "1badbabe");

    ostringstream ss2;
    ss2 << hex << showbase << 0x1badbabeUL;
    EXPECT_EQUAL(as_const(ss2.str()).c_str(), "0x1badbabe");

    ostringstream ss3;
    ss3 << hex << showbase << uppercase << 0x1badbabeUL;
    EXPECT_EQUAL(as_const(ss3.str()).c_str(), "0X1BADBABE");

    ostringstream ss4;
    ss4 << hex << showbase << setw(18) << setfill('0') << internal
        << 0x000000001badbabeUL;
    EXPECT_EQUAL(as_const(ss4.str()).c_str(), "0x000000001badbabe");

    ostringstream ss5;
    ss5 << 0ULL;
    EXPECT_EQUAL(as_const(ss5.str()).c_str(), "0");

    ostringstream ss6;
    ss6 << 123L;
    EXPECT_EQUAL(as_const(ss6.str()).c_str(), "123");

    return 0;
  }
  static void stdlib_ostream_tests() {
    TEST(test_sstream);
    TEST(test_ostream_hex);
  }
};  // class ostream_tests}

}  // namespace rtk

void stdlib_ostream_tests() {
  rtk::ostream_tests::stdlib_ostream_tests();
}