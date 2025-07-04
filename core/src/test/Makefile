.PHONY: all, tests, test, clean

STDLIBDIR := obj/tests/stdlib
OBJS := $(STDLIBDIR)/cstring.o $(STDLIBDIR)/memory.o $(STDLIBDIR)/utility.o

all: tests

tests: bin/test

obj:
	@mkdir -p obj/tests/stdlib

bin:
	@mkdir -p bin

obj/tests/%.o: cpp/tests/%.cpp | obj
	clang $< -I../../../include -I./cpp/include -c -g -O0 -MMD -MP -o $@

bin/test: $(OBJS) cpp/test.cpp | bin
	clang $^ -I../../../include -I./cpp/include -lstdc++ -g -O0 -MMD -MP -o $@

test: bin/test
	./bin/test

clean:
	$(RM) -rf bin
	$(RM) -rf obj

-include $(wildcard bin/*.d)