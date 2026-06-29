#include "aoc.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <dlfcn.h>

clock_t started;

void time_start() { started = clock(); }
void time_end(int year, int day) {
  clock_t ended = clock();
  printf("=> %d/%d took %ldms\n\n", year, day, (ended-started) * 1000 / CLOCKS_PER_SEC);
}

bool download_input(Day *day, const char *to_file) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    char url[1024];
    snprintf(url, 1024, "https://adventofcode.com/%d/day/%d/input", day->year,
             day->day);

    FILE *cookief = fopen(".cookie", "r");
    if (!cookief) {
      fprintf(stderr, "No .cookie file to download input with!\n");
      return false;
    }
    char *cookie = NULL;
    size_t linecap = 0;
    if (getline(&cookie, &linecap, cookief) < 8) {
      fprintf(stderr, "Couldn't read cookie\n");
      fclose(cookief);
      return false;
    }
    printf("GOT COOKIE: '%s'\n", cookie);
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(to_file, "w");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        curl_easy_setopt(curl, CURLOPT_COOKIE, cookie);
        res = curl_easy_perform(curl);
        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
        return true;
    }
    return false;
}

char *input(Day *day, const char *file, size_t *len) {
  FILE *f = fopen(file, "r");
  if(!f) {
    if(!download_input(day, file)) {
      fprintf(stderr, "Input does not exist and couldn't be downloaded: %s\nCheck .session file.", file);
      return NULL;
    }
    f = fopen(file, "r");
    if(!f) {
      fprintf(stderr, "Input could not be opened, even tho it was downloaded ok ¯\\_(ツ)_/¯\n");
      return NULL;
    }
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

void try_compile(int year, int day) {
  char cmd[512];
  snprintf(cmd, 512,
           "cc -std=c17 -O3 -fPIC -Wl,-undefined -Wl,dynamic_lookup -shared -I "
           ". -o %d/day%d.o %d/day%d.c $(pkg-config --cflags --libs z3) -lm",
           year, day, year, day);
  printf("compile: %s\n", cmd);
  system(cmd);
}

bool run_day(Day *day, bool example) {
  // load input and solution module

  char input_file[16];
  snprintf(input_file, 16, "%d/day%d%s.txt", day->year, day->day, example ? "_ex" : "");

  char module_file[16];
  snprintf(module_file, 16, "%d/day%d.o", day->year, day->day);

  char function_name[16];
  snprintf(function_name, 16, "day%d", day->day);

  void *solution = dlopen(module_file, RTLD_NOW);
  if (!solution) {
    try_compile(day->year, day->day);
    solution = dlopen(module_file, RTLD_NOW);
    if(!solution) {
      fprintf(stderr, "Can't open module: %s\n", module_file);
      return false;
    }
  }

  void (*dayfn)(Day*) = dlsym(solution, function_name);
  if(!dayfn) {
    fprintf(stderr, "Day solve function not found: %s\n", function_name);
    return false;
  }

  size_t len;
  char *in = input(day, input_file, &len);
  if (!in)
    return false;


  day->input = (str){.len = len, .data = in};
  day->start = clock();
  dayfn(day);
  day->end = clock();
  return true;
}

typedef struct dt {
  pthread_t id;
  bool example;
  Day day;
} dt;

void print_answer(const char *label, Answer a) {
  switch (a.type) {
  case NO_ANSWER:
    printf("%s=(no answer)", label);
    break;
  case LONG_ANSWER:
    printf("%s=%ld", label, a.answer.long_answer);
    break;
  case STR_ANSWER:
    printf("%s=%.*s", label, (int) a.answer.str_answer.len, a.answer.str_answer.data);
    break;
  }
}

void print_day(Day d) {
  printf("%d day %d, ", d.year, d.day);
  print_answer("part1", d.part1);
  printf(", ");
  print_answer("part2", d.part2);
  printf(" [%ld ms]\n", (d.end - d.start) * 1000 / CLOCKS_PER_SEC);
}

int last_day(int year) {
  if(year == 2025) return 12;
  return 25;
}

int main(int argc, char **argv) {
  if(argc < 3) {
    fprintf(stderr, "Specify year and day, eg. ./main 2025 1 [ex]\n");
    return 1;
  }

  bool example = argc > 3 && strcmp(argv[3],"ex")==0;
  int year = atoi(argv[1]);

  if (strcmp(argv[2], "all") == 0) {
    for (int i = 1; i <= last_day(year); i++) {
      Day d = (Day){.year = year, .day = i};
      run_day(&d, false);
      print_day(d);
    }
  } else {
    Day d = (Day){.year = atoi(argv[1]), .day = atoi(argv[2])};
    if (!run_day(&d, example))
      return 1;
    print_day(d);
  }
  return 0;
}
