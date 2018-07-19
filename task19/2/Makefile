all: client.o server.o client_tcp.o
	gcc client.o -o client.out
	gcc client_tcp.o -o client_tcp.out
	gcc server.o -lm -pthread -o server.out

client.o: client.c
	gcc -g -c client.c

client_tcp.o: client_tcp.c
	gcc -g -c client_tcp.c


server.o: server.c
	gcc -g -c server.c

debug_client: all
	valgrind --leak-check=full ./client.out

debug_client_tcp: all
	valgrind --leak-check=full ./client_tcp.out

debug_server: all
	valgrind --leak-check=full ./server.out

clean:
	rm -f *.out
	rm -f *.o
