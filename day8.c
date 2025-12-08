#include "aoc.h"
#include <math.h>


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

int cmp(const void *a, const void *b) { return (*(int *)b) - (*(int *)a); }

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

  /* Take 2 closest ones
   * naive 2 nested loops */
  double min_dist = 0; // dist must be greater than this, update on each match
  long part1_rounds = 1000;
  long rounds=0;
  for(;;) {
    box a, b;
    int closest_a, closest_b; // index
    double closest_distance = 0.0;
    for (int i = 0; i < arrlen(bs); i++) {
      a = bs[i];
      for (int j = 0; j < arrlen(bs); j++) {
        if (j == i)
          continue;
        b = bs[j];

        double d = distance(a, b);
        if (d > min_dist && (!closest_distance || d < closest_distance)) {
          //printf(" closer A %ld, %ld, %ld and B %ld, %ld, %ld   (distance: %lf)\n", a.x, a.y, a.z, b.x, b.y, b.z, d);
          closest_distance = d;
          closest_a = i;
          closest_b = j;
        }
      }
    }
    min_dist = closest_distance;
    box *ca = &bs[closest_a];
    box *cb = &bs[closest_b];

    if (ca->circuit && cb->circuit && ca->circuit == cb->circuit) {
      // do nothing, already in same circuit
    } else if (ca->circuit && cb->circuit && ca->circuit != cb->circuit) {
      //  printf(" merge circuit %d with %d\n", ca->circuit, cb->circuit);
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
      arrput(c->bs, closest_b);
    } else if (cb->circuit) {
      circuit *c = &cs[cb->circuit - 1];
      ca->circuit = cb->circuit;
      arrput(c->bs, closest_a);
    } else {
      // new circuit
      int id = arrlen(cs)+1;
      circuit c = {0};
      ca->circuit = id;
      cb->circuit = id;
      arrput(c.bs, closest_a);
      arrput(c.bs, closest_b);
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
