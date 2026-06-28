#include "../aoc.h"

int containers[20];
int last_container = 0;
void parse(str lines) {
  str line;
  while (str_each_line(&lines, &line)) {
    containers[last_container++] = str_to_long(line);
  }
}

long popcount(uint32_t n) {
  long count = 0;
  while (n) {
    n &= (n - 1);
    count++;
  }
  return count;
}

void day17(Day *day) {
  parse(day->input);
  long found = 0;
  long min_pop = 20;
  long by_pop[21] = {0};
  for (uint32_t p = 0; p < ((uint32_t)1 << 20); p++) {
    int eggnog=0;
    for (int i = 0; i < 20; i++) {
      if (p & (uint32_t)1 << i)
        eggnog += containers[i];
    }
    if (eggnog == 150) {
      found++;
      long pop = popcount(p);
      by_pop[pop]++;
      if (pop < min_pop) {
        min_pop = pop;
      }
    }
  }
  PART1(found);
  PART2(by_pop[min_pop]);
}
