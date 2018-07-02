all: fileEditor.o
	gcc -g fileEditor.o -lncurses

fileEditor.o:	fileEditor.c
	gcc -c -g fileEditor.c

clean:
	rm -f *.o
