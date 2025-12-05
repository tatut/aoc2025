#include "aoc.h"

typedef struct range {
  long low; long high;
} range;

bool fresh(range *r, long ingredient) {
  size_t len = arrlen(r);
  for (size_t i = 0; i < len; i++) {
    if (r[i].low <= ingredient && r[i].high >= ingredient)
      return true;
  }
  return false;
}

int compare_range(const void *a, const void *b) {
  range *r1 = (range *)a;
  range *r2 = (range *)b;
  if (r1->low < r2->low)
    return -1;
  else if (r1->low > r2->low)
    return 1;
  else
    return 0;
}

void day5(str input) {
  range *ranges = NULL;
  long *ingredients = NULL;

  str line;
  // parse fresh ranges
  while (str_splitat(input, "\n", &line, &input)) {
    if (line.len == 0)
      break; // done parsing ranges
    str lo, hi;
    str_splitat(line, "-", &lo, &hi);
    range r = (range){.low = str_to_long(lo), .high = str_to_long(hi)};
    arrput(ranges, r);
  }

  // parse ingredients
  while (str_splitat(input, "\n", &line, &input)) {
    arrput(ingredients, str_to_long(line));
  }

  long fresh_count = 0;
  long len = arrlen(ingredients);
  for (size_t i = 0; i < len; i++) {
    if(fresh(ranges, ingredients[i])) fresh_count++;
  }
  printf("Part1: %ld\n", fresh_count);

  /* count unique items in ranges */
  range *unique = NULL;
  long unique_count = 0;

  // sort ranges by low
  qsort(ranges, arrlen(ranges), sizeof(range), compare_range);
  range prev = ranges[0];
  for (int r = 1; r < arrlen(ranges); r++) {
    range at = ranges[r];
    if (at.low <= prev.high) {
      // if range is low is within prev range, extend prev if bigger
      if(at.high > prev.high)
        prev.high = at.high;
    } else {
      // if low starts after prev high, add prev to unique
      unique_count += (1 + (prev.high - prev.low));
      prev = at;
    }
  }
  unique_count += (1 + (prev.high - prev.low));

  printf("Part2: %ld\n", unique_count);
}
