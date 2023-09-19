CC=gcc
FLAGS=-Wall -O3

all:
	$(CC) vector.c -c $(FLAGS)
	$(CC) compute.c -c $(FLAGS)
	$(CC) main.c vector.o compute.o -o bin $(FLAGS)

clean:
	rm -rf *o *bin
