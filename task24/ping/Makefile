all: ping.o
	gcc ping.o -o ping

ping.o: ping.c
	gcc -g -c ping.c

debug_ping: all
	valgrind --leak-check=full ./ping

clean:
	rm -f *.out
	rm -f *.o
