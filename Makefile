main: main.c
	cc -O3 -o main main.c

day%.o: day%.c
	cc -O3 -fPIC -shared -o $@ $<

clean:
	rm *.o
	rm main
