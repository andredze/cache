CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O0 -g -I include/
SRC = src/*.cc

.PHONY: all
all:
	$(CXX) $(CXXFLAGS) $(SRC) -o use

.PHONY: clean
clean:
	rm -rf *.o use
