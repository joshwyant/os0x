.PHONY: all, tests, test, clean

OBJ_DIR := obj/main/cpp
TEST_OBJ_DIR := obj/test/cpp
TEST_TESTS_OBJ_DIR := $(TEST_OBJ_DIR)/tests
OBJS := 
TEST_OBJS := $(OBJS) $(TEST_OBJ_DIR)/test.o

all: tests

tests: bin/test_tests

obj:
	mkdir -p obj $(OBJ_DIR) $(TEST_OBJ_DIR) $(STDLIB_TESTS_OBJ_DIR)

bin:
	@mkdir -p bin

obj/%.o: src/%.cpp | obj
	clang $< -I../include -Isrc/include -c -g -O0 -MMD -MP -o $@

bin/test_tests: $(TEST_OBJS) | bin
	clang $(TEST_OBJS) -lstdc++ -g -O0 -MMD -MP -o $@

test: bin/test_tests
	./bin/test_tests

clean:
	$(RM) -rf bin
	$(RM) -rf obj

-include $(TEST_OBJS:.o=.d)
-include $(OBJS:.o=.d)