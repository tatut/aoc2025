#include "aoc.h"

long shape_count[] = {5,7,6,7,7,7}; // hardcoded number of '#' in input shapes

typedef struct region {
  long w, h;
  long count[6]; // counts of different shapes to fit
} region;



void day12(str input) {
  region *rs = {0};
  str line;
  while(str_each_line(&input, &line)) {
    if(line.len == 0) continue;
    char ch = line.data[1];
    if(ch == ':' || ch == '#' || ch == '.') continue;
    str_indexof(line, 'x');
    region r = (region) {
      .w = str_to_long(str_take(line, 2)),
      .h = str_to_long(str_take(str_drop(line, 3), 2))
    };
    line = str_drop(line, 7);
    for(int i=0;i<6;i++) {
      r.count[i] = str_to_long(str_take(line, 2));
      line = str_drop(line, 3);
    }
    arrput(rs, r);
  }

  long fits = 0;
  for(int i=0;i<arrlen(rs);i++) {
    long needed = 0;
    for(int j=0;j<6;j++) {
      needed += shape_count[j] * rs[i].count[j];
    }
    if(needed <= rs[i].w*rs[i].h)
      fits++;
  }
  printf("Part1: %ld\n", fits); // 495 was right, no fitting needed!
}
