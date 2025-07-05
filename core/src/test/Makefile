.PHONY: all, tests, test, clean

STDLIBDIR := obj/tests/stdlib
OBJS := $(STDLIBDIR)/cstring.o $(STDLIBDIR)/memory.o $(STDLIBDIR)/utility.o $(STDLIBDIR)/string.o obj/test.o

all: tests

tests: bin/test

obj:
	@mkdir -p obj obj/tests/stdlib

bin:
	@mkdir -p bin

obj/%.o: cpp/%.cpp | obj
	clang $< -I../../../include -I./cpp/include -c -g -O0 -MMD -MP -o $@

bin/test: $(OBJS) | bin
	clang $(OBJS) -lstdc++ -g -O0 -MMD -MP -o $@

test: bin/test
	./bin/test

clean:
	$(RM) -rf bin
	$(RM) -rf obj

-include $(OBJS:.o=.d)