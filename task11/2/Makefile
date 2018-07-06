all: main.o
	gcc main.o -pthread

main.o: main.c
	gcc -g -c main.c

debug: all
	valgrind --leak-check=full ./a.out

clean:
	rm -f *.o
