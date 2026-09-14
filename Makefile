CC = g++
CFLAGS = -Wall -Wextra -O0 -g -I include/
SRC = src/*.cc

all:
	$(CC) $(CFLAGS) $(SRC) -o use
