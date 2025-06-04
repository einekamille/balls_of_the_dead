CC=g++

CFLAGS=-c -Wall

LDFLAGS=-lsfml-system -lsfml-graphics -lsfml-window -lsfml-audio


all: balls

clean:
	rm -rf balls *.o

balls: balls.o
	$(CC) balls.o -o balls $(LDFLAGS)

balls.o: balls.cpp
	$(CC) $(CFLAGS) balls.cpp
	
