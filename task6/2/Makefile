all: calc.o libadd.so libsub.so libmult.so libdiv.so
	gcc -g calc.o -L. -ldl
	
calc.o: calc.c
	gcc -c calc.c

add.o: add.c
	gcc -fpic -c add.c

sub.o: sub.c
	gcc -fpic -c sub.c

mult.o: mult.c
	gcc -fpic -c mult.c

div.o: div.c
	gcc -fpic -c div.c

libadd.so: add.o
	gcc -shared add.o -o libadd.so

libsub.so: sub.o
	gcc -shared sub.o -o libsub.so

libmult.so: mult.o
	gcc -shared mult.o -o libmult.so

libdiv.so: div.o
	gcc -shared div.o -o libdiv.so

clean:
	rm -f ./*.o 
	rm -f ./*.so
