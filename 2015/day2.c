#include "../aoc.h"

int cmp_long(const void *aptr, const void *bptr) {
  long a = *((long *)aptr);
  long b = *((long *)bptr);
  return a - b;
}

void day2(Day *day) {
  long paper = 0, ribbon = 0;
  str input = day->input;
  str line;
  while (str_each_line(&input, &line)) {
    str ws,hs,ls;
    str_splitat(line, "x", &ws, &line);
    str_splitat(line, "x", &hs, &ls);
    long d[3] = {str_to_long(ws), str_to_long(hs), str_to_long(ls)};
    qsort(d, 3, sizeof(long), cmp_long);
    paper += (3 * d[0]*d[1]) + (2 * d[0]*d[2]) + (2 * d[1]*d[2]);
    ribbon += (2*d[0]) + (2*d[1]) + (d[0]*d[1]*d[2]);
  }
  PART1(paper);
  PART2(ribbon);

}
