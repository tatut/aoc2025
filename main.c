#include "aoc.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <dlfcn.h>

clock_t started;

void time_start() { started = clock(); }
void time_end(int day) {
  clock_t ended = clock();
  printf("=> Day%d took %ldms\n\n", day, (ended-started) * 1000 / CLOCKS_PER_SEC);
}


char *input(const char *file, size_t *len) {
  FILE *f = fopen(file, "r");
  if(!f) {
    fprintf(stderr, "Can't open input file: %s\n", file);
    return NULL;
  }
  struct stat fs;
  int in = fileno(f);
  if(fstat(in, &fs) == -1) {
    fprintf(stderr, "Can't stat input file: %s\n", file);
    return NULL;
  }
  char *map = mmap(0,fs.st_size, PROT_READ, MAP_SHARED, in, 0);
  if(!map) {
    fprintf(stderr, "Can't mmap input file: %s\n", file);
    return NULL;
  }
  if(len) *len = fs.st_size;

  char *data = malloc(*len);
  memcpy(data, map, *len);
  munmap(map, *len);
  return data;
}

int run_day(int day, bool example) {
  // load input and solution module

  char input_file[16];
  snprintf(input_file, 16, "day%d%s.txt", day, example ? "_ex" : "");

  char module_file[16];
  snprintf(module_file, 16, "day%d.o", day);

  char function_name[16];
  snprintf(function_name, 16, "day%d", day);

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
  if (!in)
    return 1;

  time_start();
  dayfn((str){.len = len, .data = in});
  time_end(day);
  return 0;
}

int main(int argc, char **argv) {
  if(argc < 2) {
    fprintf(stderr, "Specify day, eg. ./main 1 [ex]\n");
    return 1;
  }

  bool example = argc > 2 && strcmp(argv[2],"ex")==0;
  int ret=0;
  if(strcmp(argv[1], "all")==0) {
    for(int i=1;i<=12;i++) {
      ret += run_day(i, example);
    }
  } else {
    ret = run_day(atoi(argv[1]), example);
  }
  return ret;

}
