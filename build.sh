#!/bin/sh

BASE="$1/day$2"

cc -std=c17 -O3 -fPIC -Wl,-undefined -Wl,dynamic_lookup -shared -I . -o $1/day$2.o $1/day$2.c $(pkg-config --cflags --libs z3)
