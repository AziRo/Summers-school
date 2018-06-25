all: calc.o libcalc.a
	gcc -g calc.o --static -L. -lcalc -lm
	
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
	gcc -c _sin.c

_cos.o: _cos.c
	gcc -c _cos.c

log_b.o: log_b.c
	gcc -c log_b.c

libcalc.a: add.o sub.o mult.o div.o pow_b.o _sin.o _cos.o log_b.o
	ar rc libcalc.a add.o sub.o mult.o div.o pow_b.o _sin.o _cos.o log_b.o

clean:
	rm -f ./*.o
