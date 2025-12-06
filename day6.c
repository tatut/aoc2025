#include "aoc.h"
#include <ctype.h>

typedef struct problem {
  long *nums;
  char op;
} problem;

long calc(problem p) {
  long out = p.nums[0];
  for(int i=1;i<arrlen(p.nums);i++) {
    switch(p.op) {
    case '*': out *= p.nums[i]; break;
    case '+': out += p.nums[i]; break;
    default:
      fprintf(stderr, "No such op: %c\n", p.op);
      exit(1);
    }
  }
  return out;
}

void day6(str input) {
  str full_input = input;
  problem *ps = NULL;
  str line;

  while(str_splitat(input, "\n", &line, &input)) {
    line = str_trim(line);
    bool parse_nums = line.data[0] >= '0' && line.data[0] <= '9';
    str n;
    int i=0;
    while(line.len) {
      if(str_splitat(line, " ", &n, &line)) {
        n = str_trim(n);
        line = str_trim(line);
      } else {
        n = line;
        line = (str){0};
      }
      char op = parse_nums ? 0 : n.data[0];
      long num = parse_nums ? str_to_long(n) : 0;
      if(arrlen(ps) <= i) {
        problem p = (problem) {0};
        arrput(p.nums, num);
        arrput(ps, p);
      } else {
        if(parse_nums) arrput(ps[i].nums, num);
        else ps[i].op = op;
      }
      i++;
    }
  }

  long part1 = 0;
  for(int i=0;i<arrlen(ps);i++) {
    part1 += calc(ps[i]);
  }
  printf("Part1: %ld\n", part1);

  /* Part2: parse from right to left, numbers are in columns.
   * split into an array of lines, saving max len
   */
  ps = NULL; //arrsetlen(ps, 0);
  input = full_input;
  str *lines = {0};
  size_t max_len = 0;
  while(str_splitat(input, "\n", &line, &input)) {
    arrput(lines, line);
    max_len = line.len > max_len ? line.len : max_len;
  }

  /* start from max_len-1 go down each column, when you encounter an op, a problem is done */
  int c = max_len-1;
  size_t nlines = arrlen(lines);
  int i = 0;
  arrsetlen(ps, 1);
  do {
    long n=0;
    for(int l=0;l<nlines;l++) {
      char ch = lines[l].len > c ? lines[l].data[c] : 0;

      if(isdigit(ch)) {
        n = n*10 + (ch-48);
      }
      if(l == nlines-1) { arrput(ps[i].nums, n); }
      if(ch == '*' || ch == '+') {
        ps[i].op = ch;
        if(c) {
          i++;
          arrsetlen(ps, i+1);
          ps[i] = (problem){0};
        }
        c--; // skip space
      }
    }

    c--;
  } while(c >= 0);

  long part2 = 0;
  for(int i=0;i<arrlen(ps);i++) {
    part2 += calc(ps[i]);
  }
  printf("Part2: %ld\n", part2);


}
