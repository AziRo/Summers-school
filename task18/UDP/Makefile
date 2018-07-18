all: client.o server.o
	gcc client.o -o client.out
	gcc server.o -lm -pthread -o server.out

client.o: client.c
	gcc -g -c client.c

server.o: server.c
	gcc -g -c server.c

debug_client: all
	valgrind --leak-check=full ./client.out

debug_server: all
	valgrind --leak-check=full ./server.out

clean:
	rm -f *.out
	rm -f *.o
