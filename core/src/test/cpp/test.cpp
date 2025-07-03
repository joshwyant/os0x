#include <iostream>

#include "test/test.h"
#include "tests/stdlib/cstring.h"
#include "tests/stdlib/memory.h"
#include "tests/stdlib/utility.h"

bool testk::test_logging = true;
int testk::successful_tests = 0;
int testk::failed_tests = 0;

void failure_tests() {}
void succeeding_tests() {
  std::cout << "\ncore/stdlib/cstring.h:\n";
  stdlib_cstring_tests();
  std::cout << "\ncore/stdlib/memory.h\n";
  stdlib_memory_tests();
  std::cout << "\ncore/stdlib/utility.h:\n";
  stdlib_utility_tests();
}

int main(int argc, const char** argv) {
  failure_tests();
  succeeding_tests();

  std::cout << "\n"
            << testk::successful_tests << " succeeded, " << testk::failed_tests
            << " failed." << std::endl;

  return testk::failed_tests;
}