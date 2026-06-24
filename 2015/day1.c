#include "../aoc.h"

void day1(Day *day) {
  int level = 0;
  str in = day->input;
  for (size_t i = 0; i < in.len; i++) {
    if (in.data[i] == '(')
      level++;
    else if (in.data[i] == ')')
      level--;
  }
  PART1(level);

  level = 0;
  long basement_pos = 1;
  for (size_t i = 0; i < in.len; i++) {
    if (in.data[i] == '(')
      level++;
    else if (in.data[i] == ')')
      level--;
    if (level == -1) {
      break;
    }
    basement_pos ++;
  }
  PART2(basement_pos);
}
