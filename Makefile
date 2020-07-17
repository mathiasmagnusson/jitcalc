jitcalc: *.c *.h
	gcc -o $@ *.c -Wall -g

run: jitcalc
	./jitcalc

debug: jitcalc
	gdb jitcalc

clean:
	rm jitcalc *.o

release: *.c *.h
	gcc -o jitcalc *.c -Wall -O3
	strip jitcalc

.PHONY: run debug clean
