#include "../aoc.h"

long row, col;
long grid[4*3200*3200];

#define g(x, y) grid[((y-1) * 3200) + (x-1)]

void parse(str in) {
  if(!str_scan("To continue, please consult the code grid in the manual.  Enter the code at row %ld, column %ld.\n", in, &row, &col)) exit(1);
}
void day25(Day *day){
  parse(day->input);
  long last = 20151125;
  g(1, 1) = last;
  int r = 2;
  int c = 1;
  int nr = 3;
  while (true) {
    g(c, r) = (last * 252533) % 33554393;
    last = g(c, r);
    if(c == col && r == row) { PART1(last); break; }
    if (r > 1) {
      c++; r--;
    } else {
      r = nr++;
      c = 1;
    }
  }
}
