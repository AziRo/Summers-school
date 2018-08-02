all: tracert.o
	gcc tracert.o -o tracert -pthread

tracert.o: tracert.c
	gcc -g -c tracert.c

debug_tracert: all
	valgrind --leak-check=full ./tracert

clean:
	rm -f *.out
	rm -f *.o
