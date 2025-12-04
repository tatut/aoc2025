#include "aoc.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <dlfcn.h>

clock_t started;

void time_start() { started = clock(); }
void time_end() {
  clock_t ended = clock();
  printf("Took %ldms\n",  (ended-started) * 1000 / CLOCKS_PER_SEC);
}


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

int main(int argc, char **argv) {
  if(argc < 2) {
    fprintf(stderr, "Specify day, eg. ./main 1\n");
    return 1;
  }

  // load input and solution module

  char input_file[16];
  snprintf(input_file, 16, "day%s.txt", argv[1]);

  char module_file[16];
  snprintf(module_file, 16, "day%s.o", argv[1]);

  char function_name[16];
  snprintf(function_name, 16, "day%s", argv[1]);

  void *solution = dlopen(module_file, RTLD_NOW);
  if(!solution) {
    fprintf(stderr, "Can't open module: %s\n", module_file);
    return 1;
  }

  void (*dayfn)(str) = dlsym(solution, function_name);
  if(!dayfn) {
    fprintf(stderr, "Day solve function not found: %s\n", function_name);
    return 1;
  }

  size_t len;
  char *in = input(input_file, &len);

  time_start();
  dayfn((str){.len = len, .data = in});
  time_end();
  return 0;

}
