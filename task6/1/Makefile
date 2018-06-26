all: calc.o libcalc1.so libcalc2.so
	gcc calc.o -L. -lcalc1 -lcalc2 -lm
	
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

pow_b.o: pow_b.c
	gcc -fpic -c pow_b.c

_sin.o: _sin.c
	gcc -fpic -c _sin.c

_cos.o: _cos.c
	gcc -fpic -c _cos.c

log_b.o: log_b.c
	gcc -fpic -c log_b.c

libcalc1.so: add.o sub.o mult.o div.o
	gcc -shared add.o sub.o mult.o div.o -o libcalc1.so

libcalc2.so: pow_b.o _sin.o _cos.o log_b.o
	gcc -shared pow_b.o _sin.o _cos.o log_b.o -o libcalc2.so -lm


clean:
	rm -f ./*.o 
	rm -f ./*.so
