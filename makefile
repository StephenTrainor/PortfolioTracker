CXX ?= g++
CXXFLAGS += -Wall
LDLIBS += -lfmt -lcurl

sources = $(wildcard src/*.cpp)
exec = myAPI

all: $(exec)
	./$(exec)

$(exec): $(sources)
	$(LINK.cc) $^ -o $@ $(LDLIBS)
