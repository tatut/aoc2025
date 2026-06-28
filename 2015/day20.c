#include "../aoc.h"

void day20(Day *day) {
  long wanted_presents = str_to_long(str_trim(day->input));
  long *presents = calloc(2 * wanted_presents, sizeof(long));

  long part1=0, part2=0;
  for (long elf = 1; elf < wanted_presents && (part1 == 0 || part2 == 0);
       elf++) {
    int i=0;
    for (long house = elf; house < wanted_presents; house += elf) {
      presents[2 * house + 0] += elf * 10;
      if(i < 50) {
        presents[2 * house + 1] += elf * 11;
        i++;
      }
    }
    if (!part1 && presents[2 * (elf - 1) + 0] > wanted_presents) {
      part1 = elf-1;
    }
    if (!part2 && presents[2 * (elf - 1) + 1] > wanted_presents) {
      part2 = elf - 1;
    }
  }
  free(presents);
  PART1(part1);
  PART2(part2);
}
