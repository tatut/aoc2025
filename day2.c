#include "aoc.h"

static long pow10[] = {1,         10,         100,         1000,
                       10000,     100000,     1000000,     10000000,
                       100000000, 1000000000, 10000000000, 100000000000};

int ndigits(long l) {
  long d = 1;
  long c = 10;
  while(c < l) {
    d++;
    c *= 10;
  }
  return d;
}

bool is_invalid_p1(long n) {
  long pow = pow10[ndigits(n)>>1];
  long h1 = n/pow;
  long h2 = n%pow;
  return h1 == h2;
}


bool is_invalid_p2(long num) {
  long nd = ndigits(num);
  long half = pow10[nd>>1];

  for(long pow=10; pow <= half; pow *= 10) {
    if(nd%(ndigits(pow-1)) != 0) continue; // num digits must divide evenly
    long n = num;
    long init = n%pow;
    n /= pow;
    bool invalid = true;
    while(n) {
      long here = n%pow;
      if(here != init) {
        invalid = false;
        break;
      }
      n /= pow;
    }
    if(invalid) return true;
  }
  return false;
}

int main(int argc, char **argv) {
  size_t len;
  char *in = input("day2.txt", &len);
  char *end = in + len;
  long invalid_sum_p1 = 0, invalid_sum_p2 = 0;
  while(in < end) {
    long low = atol(in);
    while(*in != '-') in++;
    in++;
    long high = atol(in);
    while(*in != ',' && in < end) in++;
    in++;
    for(long n = low; n<=high; n++) {
      if(is_invalid_p1(n)) invalid_sum_p1 += n;
      if(is_invalid_p2(n)) invalid_sum_p2 += n;
    }
  }
  printf("Part1: %ld\nPart2: %ld\n", invalid_sum_p1, invalid_sum_p2);
  return 0;
}
