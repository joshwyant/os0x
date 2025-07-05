.PHONY: all, tests, test, clean

OBJ_DIR := obj/main/cpp
TEST_OBJ_DIR := obj/test/cpp
STDLIB_TESTS_OBJ_DIR := $(TEST_OBJ_DIR)/tests/stdlib
OBJS := $(OBJ_DIR)/status.o $(OBJ_DIR)/ostream.o
TEST_OBJS := $(OBJS) $(TEST_OBJ_DIR)/core_tests.o \
	$(STDLIB_TESTS_OBJ_DIR)/cstring_tests.o \
	$(STDLIB_TESTS_OBJ_DIR)/memory_tests.o \
	$(STDLIB_TESTS_OBJ_DIR)/utility_tests.o \
	$(STDLIB_TESTS_OBJ_DIR)/string_tests.o \
	$(STDLIB_TESTS_OBJ_DIR)/vector_tests.o \
	$(STDLIB_TESTS_OBJ_DIR)/ostream_tests.o

all: tests

tests: bin/core_tests

obj:
	mkdir -p obj $(OBJ_DIR) $(TEST_OBJ_DIR) $(STDLIB_TESTS_OBJ_DIR)

bin:
	@mkdir -p bin

obj/%.o: src/%.cpp | obj
	clang $< -I../include -Isrc/include -c -g -O0 -MMD -MP -o $@

bin/core_tests: $(TEST_OBJS) | bin
	clang $(TEST_OBJS) -lstdc++ -g -O0 -MMD -MP -o $@

test: bin/core_tests
	./bin/core_tests

clean:
	$(RM) -rf bin
	$(RM) -rf obj

-include $(TEST_OBJS:.o=.d)
-include $(OBJS:.o=.d)