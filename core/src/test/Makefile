.PHONY: all, tests

all: tests

tests: bin/test

bin:
	mkdir -p bin

bin/test: cpp/test.cpp | bin
	clang $< -I../../../include -I./cpp/include -lstdc++ -g -O0 -MMD -MP -o $@

-include $(wildcard bin/*.d)