#include "../aoc.h"

const char *bad_pairs = "abcdpqxy";

bool is_nice_1(str line) {
  char prev = 0;
  uint8_t letters[255] = {0};
  bool has_double = false;
  for (int i = 0; i < line.len; i++) {
    char ch = line.data[i];
    for (int b = 0; b < 4; b++) {
      if (prev == bad_pairs[b * 2 + 0] && ch == bad_pairs[b * 2 + 1]) {
        return false;
      }
    }
    if(prev == ch) has_double = true;
    letters[ch] += 1;
    prev = ch;
  }
  int vowels = 0;
  vowels += letters['a'];
  vowels += letters['e'];
  vowels += letters['i'];
  vowels += letters['o'];
  vowels += letters['u'];
  if (vowels < 3) {
    return false;
  }
  if (!has_double) {
    return false;
  }
  return true;
}

bool is_nice_2(str line) {
  // if any 2 letters are repeated later
  bool has_repeat_pair = false;
  bool has_repeat_ch = false; // repeated ch with gap
  for (int i = 0; i < line.len - 2; i++) {
    char p[2] = {line.data[i], line.data[i + 1]};
    for (int j = i + 2; j < line.len - 1; j++) {
      if (p[0] == line.data[j] && p[1] == line.data[j + 1])
        has_repeat_pair = true;
    }
    if (line.data[i] == line.data[i + 2])
      has_repeat_ch = true;
  }

  return has_repeat_pair && has_repeat_ch;
}

void day5(Day *day) {
  str in = day->input;
  str line;
  long nice_count_1 = 0, nice_count_2 = 0;
  while (str_each_line(&in, &line)) {
    if (is_nice_1(line))
      nice_count_1 += 1;
    if (is_nice_2(line))
      nice_count_2 += 1;
  }

  PART1(nice_count_1);
  PART2(nice_count_2);
}
