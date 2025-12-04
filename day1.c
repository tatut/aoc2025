#include "aoc.h"

int count(char *in, void (*rotate)(int*, int*, int)) {
  int count = 0;
  int current = 50;
  while(*in == 'R' || *in == 'L') {
    int dx = *in == 'R' ? 1 : -1;
    in++;
    int how_much = atoi(in);
    while(*in != '\n') in++;
    in++;
    rotate(&count, &current, dx*how_much);
  }
  return count;
}

void part1(int *count, int *current, int how_much) {
  *current += (how_much%100);
  if(*current < 0) *current = 100 - abs(*current);
  if(*current > 99) *current -= 100;
  if(*current == 0) {
    *count += 1;
  }
}

void part2(int *count, int *current, int how_much) {
  int d = how_much < 0 ? -1 : 1;
  int c = abs(how_much);
  while(c) {
    *current += d;
    if(*current == -1) *current = 99;
    else if(*current == 100) *current = 0;
    if(*current == 0) *count += 1;
    c--;
  }
}

int main(int argc, char **argv) {
  char *in = input("day1.txt", NULL);
  printf("Part1: %d\n", count(in, part1));
  printf("Part2: %d\n", count(in, part2));
  return 0;
}
