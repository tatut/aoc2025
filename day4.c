#include "aoc.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef struct Grid {
  int w, h;
  char *data;
} Grid;

char grid_at(Grid *g, int x, int y) {
  if(x < 0 || x >= g->w || y < 0 || y >= g->h) return 0;
  return g->data[y*(g->w+1)+x];
}

void grid_set(Grid *g, int x, int y, char v) {
  if(x < 0 || x >= g->w || y < 0 || y >= g->h) return;
  g->data[y*(g->w+1)+x] = v;
}


void grid_move(int dir, int x, int y, int *tox, int *toy) {
  switch(dir) {
  case 0: *tox = x;   *toy = y-1; break; // NORTH
  case 1: *tox = x+1; *toy = y-1; break; // NORTH-EAST
  case 2: *tox = x+1; *toy = y;   break; // EAST
  case 3: *tox = x+1; *toy = y+1; break; // SOUTH-EAST
  case 4: *tox = x;   *toy = y+1; break; // SOUTH
  case 5: *tox = x-1; *toy = y+1; break; // SOUTH-WEST
  case 6: *tox = x-1; *toy = y;   break; // WEST
  case 7: *tox = x-1; *toy = y-1; break; // NORTH-WEST
  default:
    fprintf(stderr, "There's no such direction: %d\n", dir);
    exit(1);
  }
}

void accessible_rolls(Grid *g, void (*accessible)(int,int)) {
  for(int y=0;y<g->h;y++) {
    for(int x=0;x<g->w;x++) {
      if(grid_at(g, x, y) != '@') continue;
      int around=0;
      for(int d=0;d<8;d++) {
        int tox, toy;
        grid_move(d, x, y, &tox, &toy);
        if(grid_at(g, tox, toy)=='@') around++;
      }
      if(around<4) { accessible(x,y); }
    }
  }
}

int accessible;
void count_accessible(int _x, int _y) {
  accessible += 1;
}

typedef struct Pos { int x; int y; } Pos;
Pos *removes;
void mark_remove(int x, int y) {
  Pos p = (Pos){.x = x, .y = y};
  arrput(removes, p);
}

int main(int argc, char **argv) {
  size_t len;
  char *in = input("day4.txt", &len);
  char *nl = strchr(in, '\n');
  Grid g = {.w = nl-in, .h = len/(nl-in+1), .data=in};
  accessible = 0;
  accessible_rolls(&g, count_accessible);
  printf("Part1: %d\n", accessible);

  int total_removed=0;
  /* While removes has items, do another round */
  do {
    arrsetlen(removes, 0);
    accessible_rolls(&g, mark_remove);
    total_removed += arrlen(removes);
    for(int i=0;i<arrlen(removes); i++) {
      grid_set(&g, removes[i].x, removes[i].y, '.');
    }
  } while(arrlen(removes));
  printf("Part2: %d\n", total_removed);
  return 0;
}
