#include "../aoc.h"

typedef struct Ingredient {
  str name;
  long capacity, durability, flavor, texture, calories;
} Ingredient;

#define MAX_INGREDIENTS 5
Ingredient ingredients[MAX_INGREDIENTS];
int last_ingredient = 0;

void parse(str lines) {
  str line;
  while (str_each_line(&lines, &line)) {
    Ingredient ing;
    if (!str_scan("%s: capacity %ld, durability %ld, flavor %ld, texture %ld, "
                  "calories %ld",
                  line, &ing.name, &ing.capacity, &ing.durability, &ing.flavor,
                  &ing.texture, &ing.calories))
      goto badline;
    ingredients[last_ingredient++] = ing;
  }

  return;
badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}

void day15(Day *day) {
  parse(day->input);
  long p1 = -1;
  long p2 = -1;
  // only 4 ingredients, just naively make 4 inner loops, still fast
  for (int a = 0; a <= 100; a++) {
    for (int b = 0; b <= 100; b++) {
      for (int c = 0; c <= 100; c++) {
        for (int d = 0; d <= 100; d++) {
          if(a+b+c+d != 100) continue;
          long capacity =
              a * ingredients[0].capacity + b * ingredients[1].capacity +
              c * ingredients[2].capacity + d * ingredients[3].capacity;
          long durability =
              a * ingredients[0].durability + b * ingredients[1].durability +
              c * ingredients[2].durability + d * ingredients[3].durability;
          long flavor =
              a * ingredients[0].flavor + b * ingredients[1].flavor +
              c * ingredients[2].flavor + d * ingredients[3].flavor;
          long texture =
              a * ingredients[0].texture + b * ingredients[1].texture +
              c * ingredients[2].texture + d * ingredients[3].texture;

          long score = (capacity < 0 ? 0 : capacity) *
                       (durability < 0 ? 0 : durability) *
                       (flavor < 0 ? 0 : flavor) * (texture < 0 ? 0 : texture);
          if (score > p1)
            p1 = score;

          long cal = a * ingredients[0].calories + b * ingredients[1].calories +
                     c * ingredients[2].calories + d * ingredients[3].calories;
          if (cal == 500 && score > p2)
            p2 = score;
        }
      }
    }
  }
  PART1(p1);
  PART2(p2);

}
