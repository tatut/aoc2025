#include "../aoc.h"

uint16_t counts[8*26] = {0};

char maxch(uint16_t *counts) {
  uint16_t max = 0;
  char ch;
  for (int i = 0; i < 26; i++) {
    if (counts[i] > max) {
      max = counts[i];
      ch = i + 'a';
    }
  }
  return ch;
}

char minch(uint16_t *counts) {
  uint16_t min = 65000;
  char ch;
  for (int i = 0; i < 26; i++) {
    if (counts[i] < min) {
      min = counts[i];
      ch = i + 'a';
    }
  }
  return ch;
}

void day6(Day *day){
  str line, lines = day->input;
  while(str_each_line(&lines, &line)) {
    for (int i = 0; i < 8; i++) {
      char ch = line.data[i];
      counts[i*26+(ch-'a')]++;
    }
  }
  char max[8], min[8];
  for (int i = 0; i < 8; i++) {
    max[i] = maxch(&counts[i * 26]);
    min[i] = minch(&counts[i * 26]);
  }
  set_str_answer(&day->part1, (str){.data = max, .len = 8});
  set_str_answer(&day->part2, (str){.data = min, .len=8});
}
