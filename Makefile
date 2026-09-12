CC = g++
CFLAGS = -O0 -g -I include/
SRC = src/*.cc

all:
	$(CC) $(CFLAGS) $(SRC) -o use