#include "../aoc.h"

uint64_t visit[(1000*1000)/64] = {0};

#define IDX(x, y) ((((y) + 500) * 1000 + (x + 500)) / 64)
#define BIT(x, y) (uint64_t)1 << ((((y) + 500) * 1000 + (x + 500)) % 64)

void day1(Day *day) {
  str in = day->input;
  long x = 0, y = 0, dir = 0; // 0 = north, 1 = east, 2 = south, 3 = west

  bool double_visit = false;
  long double_x,double_y;

  while(in.len) {
    switch(in.data[0]) {
    case 'R': dir = (dir+1)%4; break;
    case 'L': dir = dir ? (dir-1) : 3; break;
    default: goto badline;
    }
    in = str_drop(in, 1);
    long hops = str_to_long_rest(in, &in);
    if (!double_visit) {
      // check each position
      for (int h = 0; h < hops; h++) {
        switch (dir) {
        case 0: y -= 1; break;
        case 1: x += 1; break;
        case 2: y += 1; break;
        case 3: x -= 1; break;
        }
        if (!double_visit && visit[IDX(x, y)] & BIT(x, y)) {
          double_visit = true;
          double_x = x;
          double_y = y;
        }
        visit[IDX(x,y)] |= BIT(x,y);
      }
    } else {
      switch(dir) {
      case 0: y -= hops; break;
      case 1: x += hops; break;
      case 2: y += hops; break;
      case 3: x -= hops; break;
      }
    }
    if(in.len)
      in = str_drop(in,2);
  }
  if(x < 0) x = -x;
  if(y < 0) y = -y;
  PART1(x+y);
  if (double_x < 0)
    double_x = -double_x;
  if (double_y < 0)
    double_y = -double_y;
  PART2(double_x+double_y);
  return;
 badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)in.len, in.data);

}
