.PHONY: all, tests, test, clean

OBJ_DIR := obj/main/cpp
TEST_OBJ_DIR := obj/test/cpp
STDLIB_TESTS_OBJ_DIR := $(TEST_OBJ_DIR)/tests/stdlib
OBJS := $(OBJ_DIR)/status.o $(OBJ_DIR)/ostream.o
TEST_OBJS := $(OBJS) $(TEST_OBJ_DIR)/test.o \
	$(STDLIB_TESTS_OBJ_DIR)/cstring.o \
	$(STDLIB_TESTS_OBJ_DIR)/memory.o \
	$(STDLIB_TESTS_OBJ_DIR)/utility.o \
	$(STDLIB_TESTS_OBJ_DIR)/string.o \
	$(STDLIB_TESTS_OBJ_DIR)/vector.o \
	$(STDLIB_TESTS_OBJ_DIR)/ostream.o

all: tests

tests: bin/test

obj:
	mkdir -p obj $(OBJ_DIR) $(TEST_OBJ_DIR) $(STDLIB_TESTS_OBJ_DIR)

bin:
	@mkdir -p bin

obj/%.o: src/%.cpp | obj
	clang $< -I../include -Isrc/include -c -g -O0 -MMD -MP -o $@

bin/test: $(TEST_OBJS) | bin
	clang $(TEST_OBJS) -lstdc++ -g -O0 -MMD -MP -o $@

test: bin/test
	./bin/test

clean:
	$(RM) -rf bin
	$(RM) -rf obj

-include $(TEST_OBJS:.o=.d)
-include $(OBJS:.o=.d)