#include "aoc.h"
#include <math.h>
#include <stdio.h>


typedef struct box {
  long x, y, z;
  int circuit; // id of circuit this is in (starting from 1, 0 means not in any)
} box;

typedef struct circuit {
  int *bs; // box indexes
  bool defunct; // if true, this was merged with another one
} circuit;

double distance(box a, box b) {
  long dx = (b.x - a.x);
  long dy = (b.y - a.y);
  long dz = (b.z - a.z);
  return sqrt(dx*dx + dy*dy + dz*dz);
}

typedef struct pair {
  double dist;
  // index to boxes
  uint16_t a;
  uint16_t b;
} pair;

int cmp(const void *a, const void *b) { return (*(int *)b) - (*(int *)a); }

int cmp_pair(const void *a, const void *b) {
  pair *p1 = (pair *)a;
  pair *p2 = (pair *)b;
  if (p2->dist > p1->dist)
    return -1;
  else if (p1->dist > p2->dist)
    return 1;
  else
    return 0;
}

void day8(str input) {
  box *bs = NULL;     // junction boxes
  circuit *cs = NULL; // circuits
  str line;
  while (str_splitat(input, "\n", &line, &input)) {
    str c;
    str_splitat(line, ",", &c, &line);
    box b = {0};
    b.x = str_to_long(c);
    str_splitat(line, ",", &c, &line);
    b.y = str_to_long(c);
    b.z = str_to_long(line);
    arrput(bs, b);
  }

  /* build distance pairs for all boxes */
  pair *ps = NULL;
  uint16_t len = arrlen(bs);
  arrsetcap(ps, len*len); // preallocate enough memory

  for (uint16_t i = 0; i < len; i++) {
    for (uint16_t j = i+1; j < len; j++) {
      pair p = (pair){.a = i, .b = j, .dist = distance(bs[i], bs[j])};
      arrput(ps, p);
    }
  }
  qsort(ps, arrlen(ps), sizeof(pair), cmp_pair);


  long part1_rounds = 1000;
  long rounds=0;
  for (;;) {
    pair p = ps[rounds];

    box *ca = &bs[p.a];
    box *cb = &bs[p.b];
    if (ca->circuit && cb->circuit && ca->circuit == cb->circuit) {
      // do nothing, already in same circuit
    } else if (ca->circuit && cb->circuit && ca->circuit != cb->circuit) {
      circuit *to = &cs[ca->circuit - 1];
      circuit *from = &cs[cb->circuit - 1];
      for (int i = 0; i < arrlen(from->bs); i++) {
        int bi = from->bs[i];
        arrput(to->bs, bi);
        bs[bi].circuit = ca->circuit;
      }
      from->defunct = true;
      } else if (ca->circuit) {
      circuit *c = &cs[ca->circuit - 1];
      cb->circuit = ca->circuit;
      arrput(c->bs, p.b);
    } else if (cb->circuit) {
      circuit *c = &cs[cb->circuit - 1];
      ca->circuit = cb->circuit;
      arrput(c->bs, p.a);
    } else {
      // new circuit
      int id = arrlen(cs)+1;
      circuit c = {0};
      ca->circuit = id;
      cb->circuit = id;
      arrput(c.bs, p.a);
      arrput(c.bs, p.b);
      arrput(cs, c);
    }

    rounds++;
    if (rounds == part1_rounds) {
      int *sizes = NULL;
      for (int i = 0; i < arrlen(cs); i++) {
        if (!cs[i].defunct) {
          arrput(sizes, arrlen(cs[i].bs));
        }
      }
      qsort(sizes, arrlen(sizes), sizeof(int), cmp);
      printf("Part1: %d\n", sizes[0] * sizes[1] * sizes[2]);
      arrfree(sizes);
    }

    /* check if last connection */
    if (arrlen(cs[ca->circuit-1].bs) == arrlen(bs)) {
      printf("Part2: %ld\n", ca->x * cb->x);
      break;
    }
  }

}
