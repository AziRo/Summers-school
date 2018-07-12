all: client.o server.o
	gcc client.o -pthread -o client.out
	gcc server.o -pthread -o server.out

client.o: client.c
	gcc -g -c client.c

server.o: server.c
	gcc -g -c server.c

debug_client:
	valgrind --leak-check=full ./client.out

debug_server:
	valgrind --leak-check=full ./server.out

clean:
	rm -f *.o
	rm -f *.out
