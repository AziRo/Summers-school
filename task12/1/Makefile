all: main.o
	gcc -g main.o

main.o: main.c
	gcc -g -c main.c

debug: all
	valgrind --leak-check=full ./a.out
