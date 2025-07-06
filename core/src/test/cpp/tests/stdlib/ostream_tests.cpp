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
    ostringstream ss;
    ss << hex << 0x1badbabeUL;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "1badbabe");

    ss = {};
    ss << hex << showbase << 0x1badbabeUL;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "0x1badbabe");

    ss = {};
    ss << hex << showbase << uppercase << 0x1badbabeUL;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "0X1BADBABE");

    ss = {};
    ss << hex << showbase << setw(18) << setfill('0') << internal
       << 0x1badbabeUL;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "0x000000001badbabe");

    ss = {};
    ss << hex << showbase << setw(18) << setfill('0') << left << 0x1badbabeUL;
    EXPECT_EQUAL(as_const(ss.str()).c_str(),
                 "000000000x1badbabe");  // wrong config but expected behavior

    ss = {};
    ss << hex << showbase << setw(18) << setfill('0') << right << 0x1badbabeUL;
    EXPECT_EQUAL(as_const(ss.str()).c_str(),
                 "0x1badbabe00000000");  // wrong config but expected behavior

    ss = {};
    ss << 0;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "0");

    ss = {};
    ss << setfill('0') << setw(4) << 0;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "0000");

    ss = {};
    ss << setfill(' ') << setw(4) << left << 23;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "  23");

    ss = {};
    ss << true;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "true");

    ss = {};
    ss << false;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "false");

    ss = {};
    ss << uppercase << true;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "TRUE");

    ss = {};
    ss << uppercase << false;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "FALSE");

    ss = {};
    ss << 123L;
    EXPECT_EQUAL(as_const(ss.str()).c_str(), "123");

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