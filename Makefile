main: main.c
	cc -O3 -o main main.c

day%.o: day%.c
	cc -fPIC -shared -o $@ $<
