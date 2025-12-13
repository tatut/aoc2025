main: main.c
	cc -O3 -o main main.c

day10.o: day10.c aoc.h
	cc -O3 -fPIC -shared -o day10.o day10.c $$(pkg-config --cflags --libs z3)

day%.o: day%.c aoc.h
	cc -O3 -fPIC -shared -o $@ $<

clean:
	rm *.o
	rm main

day10: aoc.h day10.c
