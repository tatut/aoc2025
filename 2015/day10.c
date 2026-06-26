#include "../aoc.h"

char *buf1;
char *buf2;

void look_and_say_naive(int round) {
  char *look = round % 2 == 0 ? buf1 : buf2;
  char *say = round % 2 == 0 ? buf2 : buf1;

  char ch = 0;
  int count = 0;
  while (*look) {
    if (ch == 0) {
      ch = *look;
      count = 1;
    } else if (*look == ch) {
      count++;
    } else {
      say += snprintf(say, 10, "%d%c", count, ch);
      ch = *look;
      count = 1;
    }
    look++;
  }
  if (count) {
    say += snprintf(say, 10, "%d%c", count, ch);
  }
  *say = 0;
}

void day10(Day *day) {
  buf1 = malloc(10 * 1024 * 1024);
  buf2 = malloc(10 * 1024 * 1024);
  str in = str_trim(day->input);
  memcpy(buf1, in.data, in.len);
  buf1[in.len] = 0;
  long p1, p2;
  for (int i = 0; i < 50; i++) {
    look_and_say_naive(i);
    if (i == 39)
      p1 = strlen(buf1);
    if (i == 49)
      p2 = strlen(buf1);
  }
  free(buf1);
  free(buf2);
  PART1(p1);
  PART2(p2);

}
