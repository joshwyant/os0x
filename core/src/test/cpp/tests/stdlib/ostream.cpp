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
  static void stdlib_ostream_tests() { TEST(test_sstream); }
};  // class ostream_tests}

}  // namespace rtk

void stdlib_ostream_tests() {
  rtk::ostream_tests::stdlib_ostream_tests();
}