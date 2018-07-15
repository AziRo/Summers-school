all: main.o
	gcc main.o

main.o: main.c
	gcc -c main.c

debug: all
	valgrind --leak-check=full ./a.out

clean:
	rm -f *.o
	rm -f *.out
