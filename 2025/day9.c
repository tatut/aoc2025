#include "aoc.h"

typedef struct pos {
  long x, y;
} pos;

typedef enum tile { OTHER = '.', RED = '#', GREEN = 'X' } tile;

typedef struct grid {
  long min_x, min_y, max_x, max_y;
  char *data;
} grid;

#define G_WIDTH(g) ((g)->max_x - (g)->min_x + 1)
#define G_HEIGHT(g) ((g)->max_y - (g)->min_y + 1)

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

long calc_area(pos p1, pos p2) {
  long dx = labs(p1.x - p2.x) + 1;
  long dy = labs(p1.y - p2.y) + 1;
  return dx*dy;
}

bool is_inside(pos c1, pos c2, pos p) {
  int min_x = min(c1.x, c2.x);
  int max_x = max(c1.x, c2.x);
  int min_y = min(c1.y, c2.y);
  int max_y = max(c1.y, c2.y);
  return (p.x > min_x && p.x < max_x && p.y > min_y && p.y < max_y);
}

void day9(str input) {
  pos *ps = NULL;
  str line;
  while (str_splitat(input, "\n", &line, &input)) {
    str xp, yp;
    str_splitat(line, ",", &xp, &yp);
    pos p = (pos){.x = str_to_long(xp), .y = str_to_long(yp)};
    arrput(ps, p);
  }

  grid g = {.min_x = -1, .max_x = -1, .min_y = -1, .max_y = -1};
  long area = 0;
  for (int i = 0; i < arrlen(ps); i++) {
    // calculate area of our whole drawing surface
    if (g.min_x == -1 || ps[i].x < g.min_x)
      g.min_x = ps[i].x;
    if (g.max_x == -1 || ps[i].x > g.max_x)
      g.max_x = ps[i].x;
    if (g.min_y == -1 || ps[i].y < g.min_y)
      g.min_y = ps[i].y;
    if (g.max_y == -1 || ps[i].y > g.max_y)
      g.max_y = ps[i].y;


    for (int j = i + 1; j < arrlen(ps); j++) {
      long a = calc_area(ps[i], ps[j]);
      if(a > area) area = a;
    }
  }
  printf("Part1: %ld\n", area); // 4725826296


  /* write svg visualization of part 2 as we go */

  FILE *f = fopen("poly.svg", "w");
  fprintf(f,
          "<svg xmlns=\"http://www.w3.org/2000/svg\" "
          "width=\"800px\" "
          "height=\"800px\" "
          "viewBox=\"%ld %ld %ld %ld\">",
          g.min_x - 1000, g.min_y - 1000, G_WIDTH(&g) + 2000,
          G_HEIGHT(&g) + 2000);
  fprintf(f, "<polygon points=\"");
  for (int i = 0; i < arrlen(ps); i++) {
    fprintf(f, "%ld,%ld ", ps[i].x, ps[i].y);
  }

  fprintf(f,
          "\" style=\"fill: green; stroke: darkgreen; stroke-width: 100;\" />");

  for (int i = 0; i < arrlen(ps); i++) {
    fprintf(f, "<rect x=\"%ld\" y=\"%ld\" width=\"500\" height=\"500\" fill=\"red\" />",
            ps[i].x - 250, ps[i].y - 250);
  }


  long max_area = -1;
  pos pos_a, pos_b;
  int half = arrlen(ps)/2;
  //for (int i = arrlen(ps)/2; i < arrlen(ps)/2+2; i++) {
  for (int i = 0; i < arrlen(ps); i++) {
    for(int j=i+1; j<arrlen(ps);j++) {
      long area = calc_area(ps[i], ps[j]);
      if (max_area == -1 || area > max_area) {
        // check that we are on either side of center and no other point is inside
        bool valid = ((i <= half && j <= half) || (i > half && j > half));

        if(valid) {
          for (int k = 0; k < arrlen(ps); k++) {
            if (k != i && k != j && is_inside(ps[i], ps[j], ps[k])) {
              valid = false;
              break;
            }
          }
        }
        if (valid) {
          max_area = area;
          pos_a = ps[i];
          pos_b = ps[j];
        }
      }
    }
  }

  long min_x = min(pos_a.x, pos_b.x);
  long max_x = max(pos_a.x, pos_b.x);
  long min_y = min(pos_a.y, pos_b.y);
  long max_y = max(pos_a.y, pos_b.y);

  fprintf(f,
          "<rect x=\"%ld\" y=\"%ld\" width=\"%ld\" height=\"%ld\" style=\"fill: magenta;\" />",
          min_x, min_y, max_x-min_x, max_y-min_y);

  printf("Part2: %ld\n", max_area); // 1637556834


  fprintf(f, "</svg>");
  fclose(f);
}
