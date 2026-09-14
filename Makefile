CXX = g++
CXXFLAGS = --std=c++20 -Wall -Wextra -O0 -g -I include/
SRC = src/*.cc

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o use
