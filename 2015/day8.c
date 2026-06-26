#include "../aoc.h"

void count(str line, long *chars, long *bytes, long *encoded) {
  *encoded += 6; // the start/end double quotes
  *bytes += line.len;
  for (size_t i = 0; i < line.len; i++) {
    if (line.data[i] == '\\') {
      *encoded += 2;
      if (i == line.len - 1) {
        fprintf(stderr, "Error in string escape: %.*s\n", (int)line.len,
                line.data);
        return;
      }
      if (line.data[i + 1] == '\\' || line.data[i + 1] == '"') {
        *chars += 1;
        i += 1;
        *encoded += 2;
      } else if (line.data[i + 1] == 'x') {
        *chars += 1;
        i += 3;
        *encoded += 3;
      }
    } else if (line.data[i] != '"') {
      *chars += 1;
      *encoded += 1;
    }

  }
}

void day8(Day *day) {
  str lines = day->input;
  str line;
  long chars = 0, bytes = 0, encoded = 0;
  while (str_each_line(&lines, &line)) {
    count(line, &chars, &bytes, &encoded);
  }
  PART1(bytes - chars);
  PART2(encoded - bytes);
}
