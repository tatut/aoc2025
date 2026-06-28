#include "../aoc.h"

#define GRID_SIZE (1 + (100 * 100) / 64)
uint64_t grid1[GRID_SIZE] = {0};
uint64_t grid2[GRID_SIZE] = {0};

#define IDX(x, y) (((y) * 100 + (x)) / 64)
#define BIT(x, y) (((y) * 100 + (x)) % 64)

#define is_on(grid, x, y) (grid[IDX(x, y)] & (uint64_t)1 << BIT(x, y))

#define set_on(grid, x, y) (grid[IDX(x, y)] |= (uint64_t)1 << BIT(x, y))
#define set_off(grid, x, y) (grid[IDX(x, y)] &= ~((uint64_t)1 << BIT(x, y)))

long popcount(uint64_t n) {
  long count = 0;
  while (n) {
    n &= (n - 1);
    count++;
  }
  return count;
}

int on_neighbors(uint64_t *grid, int tx, int ty) {
  int on = 0;
  for (int y = ty - 1; y <= ty + 1; y++) {
    for (int x = tx - 1; x <= tx + 1; x++) {
      if ((y == ty && x == tx) ||
          x < 0 || x > 99 || y < 0 || y > 99)
        continue;
      if(is_on(grid,x,y)) on++;
    }
  }
  return on;
}

void animate(uint64_t *from, uint64_t *to, bool stuck) {
  for (int y = 0; y < 100; y++) {
    for (int x = 0; x < 100; x++) {
      if (stuck && ((x == 0 && y == 0) || (x == 99 && y == 0) ||
                    (x == 0 && y == 99) || (x == 99 && y == 99))) {
        set_on(to, x, y);
      } else {
        int c = on_neighbors(from, x, y);
        if (is_on(from, x, y)) {
          if (c == 2 || c == 3) {
            // stay on
            set_on(to, x, y);
          } else {
            set_off(to, x, y);
          }
        } else {
          if (c == 3) {
            // switch on
            set_on(to, x, y);
          } else {
            set_off(to, x, y);
          }
        }
      }
    }
  }
}

void parse(str lines) {
  int y = 0;
  str line;
  while (str_each_line(&lines, &line)) {
    for (int x = 0; x < 100; x++) {
      if (line.data[x] == '#') {
        set_on(grid1, x, y);
      } else {
        set_off(grid1, x, y);
      }
    }
    y++;
  }
}

long total_on(uint64_t *grid) {
  long on = 0;
  for (int i = 0; i < GRID_SIZE; i++) {
    on += popcount(grid[i]);
  }
  return on;
}

void stuck_corners(uint64_t *grid) {
  set_on(grid, 0, 0);
  set_on(grid, 99, 0);
  set_on(grid, 0, 99);
  set_on(grid, 99, 99);
}

long solve(str input, bool stuck) {
  parse(input);
  if(stuck) stuck_corners(grid1);
  uint64_t *from, *to, *tmp;
  from = grid2;
  to = grid1;
  for (int i = 0; i < 100; i++) {
    tmp = from;
    from = to;
    to = tmp;
    animate(from, to, stuck);
  }
  return total_on(to);
}
void day18(Day *day) {
  PART1(solve(day->input, false));
  PART2(solve(day->input, true));
}
