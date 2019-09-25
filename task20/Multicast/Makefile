CC = arm-marvell-linux-gnueabi-gcc


all: sender.o reciver.o
	$(CC) sender.o -o sender.out -static
	$(CC) reciver.o -o reciver.out -static

sender.o: sender.c
	$(CC) -g -c sender.c 

reciver.o: reciver.c
	$(CC) -g -c reciver.c

debug_sender: all
	valgrind --leak-check=full ./sender.out

debug_reciver: all
	valgrind --leak-check=full ./reciver.out

clean:
	rm -f *.out
	rm -f *.o
