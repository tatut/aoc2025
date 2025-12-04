#include "aoc.h"

int joltage(char *in, char *end) {
  int sum = 0;

  while(in < end) {
    int a = -1, b = -1;
    // naively go through from here to end of line
    char *end = strchr(in, '\n');
    int max = end - in;
    int bank_max = -1;
    for(int b1=0; b1<max-1; b1++) {
      int a = in[b1] - 48;
      for(int b2=b1+1; b2<max; b2++) {
        int b = in[b2] - 48;
        int joltage = a*10 + b;
        if(joltage > bank_max) bank_max = joltage;
      }
    }
    sum += bank_max;
    in = end + 1;
  }
  return sum;
}

long num12(long *D) {
  long n = 0;
  for(int i=0;i<12;i++) {
    n = (n*10) + D[i];
  }
  return n;
}

long joltage2(char *in, char *end) {
  long sum = 0;
  while(in < end) {
    char *end = strchr(in, '\n');
    int len = end - in;
    long digits[12] = {0};
    long p = 0; // add position
    int last_pos = -1; // last of digit we took
    while(p<12) {
      // find the biggest digit that still has 12-p-1 digits after it
      // if there are multiple, take the earliest one
      long largest = -1;
      int pos = -1;
      for(int i=last_pos+1; i<len-(12-p-1); i++) {
        long dig = in[i]-48;
        if(dig > largest) {
          largest = dig;
          pos = i;
        }
      }
      digits[p++] = largest;
      last_pos = pos;

    }
    long bank_joltage = num12(digits);
    sum += bank_joltage;
    in = end + 1;
  }
  return sum;
}

int main(int argc, char **argv) {
  size_t len;
  char *in = input("day3.txt", &len);

  char *end = in+len;
  printf("Part1: %d\n", joltage(in, end));
  printf("Part2: %ld\n", joltage2(in, end));
}
