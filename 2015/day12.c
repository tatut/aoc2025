#include "../aoc.h"

#include "../stb_ds.h"

long part1(str in) {
  long sum = 0;
  while (in.len) {
    char ch = in.data[0];
    if (is_digit(ch) || ch == '-') {
      long n = str_to_long_rest(in, &in);
      sum += n;
    } else {
      in = str_drop(in, 1);
    }
  }
  return sum;
}

str skip_if_red(str in) {
  // Skip whole object if there is a ':"red"' at this nesting level
  int depth = 1;
  in = str_drop(in, 1);
  str orig = in;
  bool is_red = false;
  while (depth) {
    char ch = in.data[0];
    if (ch == '{') {
      depth++;
    } else if (ch == '}') {
      depth--;
    } else if (depth == 1 && str_startswith(in, str_constant(":\"red\""))) {
      is_red = true;
    }
    in = str_drop(in, 1);
  }
  return is_red ? in : orig;
}

long part2(str in) {
  long sum = 0, depth = 0;
  bool is_red = false;
  while (in.len) {
    char ch = in.data[0];
    if (is_digit(ch) || ch == '-') {
      long n = str_to_long_rest(in, &in);
      sum += n;
    } else {
      if (ch == '{') {
        in = skip_if_red(in);
      } else {
        in = str_drop(in, 1);
      }
    }
  }
  return sum;
}


void day12(Day *day) {
  PART1(part1(day->input));
  PART2(part2(day->input));
}
