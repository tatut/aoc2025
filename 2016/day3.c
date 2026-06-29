#include "../aoc.h"

#define MAX_TRIANGLES 2000
long triangles[3*MAX_TRIANGLES];

void day3(Day *day) {
  long possible1 = 0, possible2 = 0;
  str lines = day->input, line;
  int i=0;
  while (str_each_line(&lines, &line)) {
    long a,b,c;
    line = str_trim(line);
    a = triangles[i++] = str_to_long_rest(line, &line);
    line = str_trim(line);
    b = triangles[i++] = str_to_long_rest(line, &line);
    line = str_trim(line);
    c = triangles[i++] = str_to_long_rest(line, &line);

    if(a < b+c && c < a+b && b < a+c) possible1++;
  }
  int last_idx = i;
  for (int i = 0; i < last_idx; i += 9) {
    for(int t=0;t<3;t++) {
      long a = triangles[i+t], b = triangles[i + t + 3],
        c = triangles[i + t + 6];
      if (a < b + c && c < a + b && b < a + c)
        possible2++;
    }
  }

  PART1(possible1);
  PART2(possible2);
}
