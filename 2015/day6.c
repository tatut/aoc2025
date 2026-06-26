#include "../aoc.h"

typedef enum Op { ON, OFF, TOGGLE } Op;

#define SZ (1000 * 1000 / 64)

uint64_t grid[SZ] = {0};
uint32_t grid2[1000 * 1000] = {0}; // brightness

#define IDX(x, y) ((((y) * 1000) + (x)) >> 6)
#define BIT(x, y) ((((y) * 1000) + (x)) % 64)

#define LIGHT_ON(x, y)                                                               \
  grid[IDX(x,y)] |= ((uint64_t)1 << BIT(x,y))

#define LIGHT_OFF(x, y)                                                              \
  grid[IDX(x,y)] &= ~((uint64_t)1 << BIT(x,y))

#define LIGHT_TOGGLE(x, y)                                                              \
  grid[IDX(x,y)] ^= ((uint64_t)1 << BIT(x,y))

long popcount(uint64_t n) {
  uint64_t orig = n;
  long count = 0;
  while (n) {
    n &= (n - 1);
    count++;
  }
  /*  if (count)
      printf("%llu has %ld bits on\n", orig, count); */
  return count;
}

long lit_count() {
  long lit = 0;
  for (size_t i = 0; i < SZ; i++) {
    lit += popcount(grid[i]);
  }
  return lit;
}

long total_brightness() {
  long b = 0;
  for (int i = 0; i < 1000 * 1000; i++)
    b += grid2[i];
  return b;
}

void day6(Day *day) {
  str lines = day->input; //toggle 901,901 through 901,901\nturn off 900,900 through 900,900\n"); //// day->input;
  str line, part;
  long x1, y1, x2, y2;
  while (str_each_line(&lines, &line)) {
    Op op;
    if (str_startswith(line, str_constant("turn on "))) {
      line = str_drop(line, 8);
      op = ON;
    } else if (str_startswith(line, str_constant("turn off "))) {
      line = str_drop(line, 9);
      op = OFF;
    } else if (str_startswith(line, str_constant("toggle "))) {
      line = str_drop(line, 7);
      op = TOGGLE;
    } else {
      goto badline;
    }

    if (!str_splitat(line, ",", &part, &line))
      goto badline;
    x1 = str_to_long(part);
    if (!str_splitat(line, " ", &part, &line))
      goto badline;
    y1 = str_to_long(part);
    if (!str_startswith(line, str_constant("through ")))
      goto badline;
    line = str_drop(line, 8);
    if (!str_splitat(line, ",", &part, &line))
      goto badline;
    x2 = str_to_long(part);
    y2 = str_to_long(line);

    /*printf("OP %d FROM (%ld,%ld) TO (%ld,%ld) (area: %ld)\n", op, x1, y1, x2, y2,
           ((x2-x1)+1) * ((y2-y1)+1)
           );*/
    for (uint64_t y = y1; y <= y2; y++) {
      for (uint64_t x = x1; x <= x2; x++) {
        switch (op) {
        case ON:
          LIGHT_ON(x, y);
          grid2[y*1000 + x]++;
          break;
        case OFF:
          LIGHT_OFF(x, y);
          if(grid2[y*1000 +x])
            grid2[y*1000 + x]--;
          break;
        case TOGGLE:
          LIGHT_TOGGLE(x, y);
          grid2[y*1000 + x]+=2;
          break;
        }
      }
    }
  }

  PART1(lit_count());
  PART2(total_brightness());
  return;
badline:
  fprintf(stderr, "Could not parse line: '%.*s'\n", (int)line.len, line.data);
}
