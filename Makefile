jitcalc: *.c *.h
	gcc -o $@ *.c -Wall -g

run: jitcalc
	./jitcalc

debug: jitcalc
	gdb jitcalc

clean:
	rm jitcalc *.o

.PHONY: run debug clean
