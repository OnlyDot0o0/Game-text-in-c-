CC = g++
CFLAGS = -std=c++11 -Wall
LIBS = -ljsoncpp

all: main

main: main.o Game.o
	$(CC) $(CFLAGS) -o main main.o Game.o $(LIBS)

main.o: main.cpp Game.h
	$(CC) $(CFLAGS) -c main.cpp

Game.o: Game.cpp Game.h
	$(CC) $(CFLAGS) -c Game.cpp

clean:
	rm -rf *.o main
