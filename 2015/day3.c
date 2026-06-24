#include "../aoc.h"
#include "../stb_ds.h"

typedef struct housepos {
  int x;
  int y;
} housepos;

typedef struct house {
  housepos key;
  int value;
} house;


long visit(str input, size_t num_santas, housepos *santas) {
  house *tbl = NULL;
  housepos at = (housepos){0,0};
  hmput(tbl, at, 1);

  for (size_t i = 0; i < input.len; i++) {
    housepos at = santas[i % num_santas];
    char d = input.data[i];
    switch (d) {
    case '>':
      at.x++;
      break;
    case '<':
      at.x--;
      break;
    case '^':
      at.y--;
      break;
    case 'v':
      at.y++;
      break;
    default:
      fprintf(stderr, "Unknown direction: %c\n", d);
      return -1;
    }
    santas[i%num_santas] = at;
    int idx = hmgeti(tbl, at);
    if (idx == -1) {
      hmput(tbl, at, 1);
    } else {
      tbl[idx].value++;
    }
  }
  long num_houses = hmlen(tbl);
  hmfree(tbl);
  return num_houses;
}

void day3(Day *day) {
  housepos santas[2] = {(housepos){0,0}, (housepos){0,0}};
  long part1 = visit(day->input, 1, santas);

  santas[0] = (housepos){0, 0};
  long part2 = visit(day->input, 2, santas);
  PART1(part1);
  PART2(part2);
}
