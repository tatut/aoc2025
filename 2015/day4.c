#include "../aoc.h"
#include <stdint.h>

#include "../vendor/md5-c/md5.h"
#include "../vendor/md5-c/md5.c"

void day4(Day *day) {
  char in[64];
  int num = 0;

  str prefix = str_trim(day->input);
  size_t pos = snprintf(in, 64, "%.*s", (int)prefix.len, prefix.data);

  bool part1 = false, part2 = false;
  for (;;) {
    uint8_t result[16];
    snprintf(&in[pos], 64 - pos, "%d", num);
    md5String(in, result);

    char start[7];
    for (int i = 0; i < 3; i++) {
      snprintf(&start[i*2], 3, "%02x", result[i]);
    }
    if (strncmp(start, "000000", 6) == 0) {
      PART2(num);
      part2 = true;
    } else if (strncmp(start, "00000", 5) == 0 && !part1) {
      PART1(num);
      part1 = true;
    }
    num += 1;

    if(part1 && part2) break;
  }

  printf("\n");
}
