OBJECTS=$(patsubst src/%.cpp,build/%.o,$(wildcard src/*cpp))
CXX?=g++
CXXFLAGS=-O2 -Wall -std=c++14 -Iheaders -ggdb

.PHONY: all clean

all: build/terminal-view

build/terminal-view: ${OBJECTS}
	${CXX} ${OBJECTS} ${LDFLAGS} -o build/terminal-view

build/%.o: src/%.cpp $(wildcard headers/*hpp) $(wildcard program-options/headers/*hpp)
	${CXX} ${CXXFLAGS} -c -o $@ $<

clean:
	rm -f build/terminal-view ${OBJECTS}
