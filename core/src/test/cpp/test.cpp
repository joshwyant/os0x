#include <iostream>

#include "test/test.h"

extern void stdlib_cstring_tests();
extern void stdlib_memory_tests();
extern void stdlib_utility_tests();

bool testk::test_logging = true;
int testk::successful_tests = 0;
int testk::failed_tests = 0;

constexpr const char* corestdlibinc = "core/stdlib/";

void failure_tests() {}
void succeeding_tests() {
  std::cout << "\n" << corestdlibinc << "cstring.cpp:\n";
  stdlib_cstring_tests();
  std::cout << "\n" << corestdlibinc << "memory.cpp\n";
  stdlib_memory_tests();
  std::cout << "\n" << corestdlibinc << "utility.cpp:\n";
  stdlib_utility_tests();
  // std::cout << "\n" << corestdlibinc << "vector.cpp\n";
  // stdlib_vector_tests();
}

int main(int argc, const char** argv) {
  failure_tests();
  succeeding_tests();

  std::cout << "\n"
            << testk::successful_tests << " succeeded, " << testk::failed_tests
            << " failed." << std::endl;

  return testk::failed_tests;
}