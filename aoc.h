#ifndef aoc_h
#define aoc_h

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

char *input(const char *file, size_t *len) {
  FILE *f = fopen(file, "r");
  struct stat fs;
  int in = fileno(f);
  fstat(in, &fs);
  char *map = mmap(0,fs.st_size, PROT_READ, MAP_SHARED, in, 0);
  if(!map) {
    fprintf(stderr, "Can't mmap input file: %s\n", file);
    exit(1);
  }
  if(len) *len = fs.st_size;

  char *data = malloc(*len);
  memcpy(data, map, *len);
  munmap(map, *len);
  return data;
}


#endif
