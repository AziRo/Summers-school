all: calc.o add.o sub.o mult.o div.o pow_b.o _sin.o _cos.o log_b.o
	gcc -g calc.o add.o sub.o mult.o div.o pow_b.o _sin.o _cos.o log_b.o -lm
	make clean
	
calc.o: calc.c
	gcc -g -c calc.c

add.o: add.c
	gcc -c add.c

sub.o: sub.c
	gcc -c sub.c

mult.o: mult.c
	gcc -c mult.c

div.o: div.c
	gcc -c div.c

pow_b.o: pow_b.c
	gcc -c pow_b.c

_sin.o: _sin.c
	gcc -c _sin.c -lm

_cos.o: _cos.c
	gcc -c _cos.c -lm

log_b.o: log_b.c
	gcc -c log_b.c -lm

clean:
	rm -f ./*.o
