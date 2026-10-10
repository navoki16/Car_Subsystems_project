PRODUCT=EngineTest

CPPFLAGS=-Iinclude -Itest -Isrc
CXXFLAGS=-Wall -Werror -Wextra -pedantic -ggdb -O0 $(CPPFLAGS)

SOURCES=$(wildcard src/Engine.cpp) \
        $(wildcard test/engineTest.cpp)

HEADERS=$(wildcard include/*.h) \
        $(wildcard test/*.h)

LDFLAGS=-lgtest -lgmock_main -lpthread -lgmock

CXX=g++

.PHONY: all clean test

all: $(PRODUCT)

$(PRODUCT): $(SOURCES) $(HEADERS)
	@$(CXX) $(CXXFLAGS) $(SOURCES) -o $@ $(LDFLAGS)

clean:
	@rm -rf $(PRODUCT)
	@rm -rf kwinject.out .kwlp .kwps

klocwork:
	@kwcheck run

klocwork_after_makefile_change: clean
	@/opt/klocwork/kwenv.sh
	@kwcheck run

test: $(PRODUCT)
	@./$(PRODUCT)