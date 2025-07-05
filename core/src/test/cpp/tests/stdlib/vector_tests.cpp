#include "core/stdlib/vector.h"
#include "test/test.h"

namespace rtk {
class vector_tests {
 public:
  static int test_stdlib_vector() {
    rtk::vector<int> v;
    v.reserve(6);
    auto expected_capacity = 0;
    void* vec_ptr = nullptr;
    for (auto i = 1; i <= 10; i++) {
      v.push_back(i);
      EXPECT_EQUAL(v.size(), i);
      EXPECT_EQUAL(v[i - 1], i);
      if (i > expected_capacity) {
        if (expected_capacity == 0) {
          expected_capacity = v.initial_capacity_;
        } else {
          expected_capacity = expected_capacity * v.num_ / v.denom_;
        }
        EXPECT_EQUAL(v.capacity_, expected_capacity);
        EXPECT_NOT_EQUAL(v.elems_.get(), vec_ptr);
        vec_ptr = v.elems_.get();
      } else {
        EXPECT_EQUAL(v.elems_.get(), vec_ptr);
      }
    }
    for (auto i = 1; i <= 10; i++) {
      EXPECT_EQUAL(v[i - 1], i);
    }
    return 0;
  }
  static void stdlib_vector_tests() { TEST(test_stdlib_vector); }
};  // class vector_testsd
}  // namespace rtk

void stdlib_vector_tests() {
  rtk::vector_tests::stdlib_vector_tests();
}