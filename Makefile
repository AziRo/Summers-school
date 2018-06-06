all: main.o pow_n.o
	gcc main.o pow_n.o

main.o: main.c
	gcc -c main.c

pow_n.o: pow_n.c
	gcc -c pow_n.c
