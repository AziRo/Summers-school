all: fileManager.o
	gcc -g fileManager.o -lncurses

fileManager.o:	fileManager.c
	gcc -c -g fileManager.c

clean:
	rm -f *.o
