all: sender.o reciver.o
	gcc sender.o -o sender.out
	gcc reciver.o -o reciver.out

sender.o: sender.c
	gcc -g -c sender.c

reciver.o: reciver.c
	gcc -g -c reciver.c

debug_sender: all
	valgrind --leak-check=full ./sender.out

debug_reciver: all
	valgrind --leak-check=full ./reciver.out

clean:
	rm -f *.out
	rm -f *.o
