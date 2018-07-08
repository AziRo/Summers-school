all: fileManager.o
	gcc -g fileManager.o -lncurses -pthread

fileManager.o:	fileManager.c
	gcc -c -g fileManager.c

debug: all
	valgrind --leak-check=full ./a.out

clean:
	rm -f *.o
