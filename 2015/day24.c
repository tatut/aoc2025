#include "../aoc.h"

long nums[32] = {};
long n = 0;

#define each_num(num, used, body) for(int _it=0;_it<n;_it++) { if(used & (uint32_t)1<<_it) {long num = nums[_it]; body }}


long qe(uint32_t used) {
  long res = 0;
  each_num(n, used, {
      res = res ? res * n : n;
    });
  return res;
}

long popcount(uint32_t n) {
  long count = 0;
  while (n) {
    n &= (n - 1);
    count++;
  }
  return count;
}

void combine(uint32_t used, long sum, long target, long *min_pop, long *min_qe) {
  //printf("sum: %ld, target: %ld\n", sum, target);
  long pop = popcount(used);
  if (sum == target) {
    if (pop < *min_pop)
      *min_pop = pop;
    long q = qe(used);
    if (pop == *min_pop && q < *min_qe) {
      *min_qe = q;
    }
    /*printf("found %ld => ", target);
    each_num(n, used, {
        printf("%ld ", n);
      });
      printf("\n");*/
  }

  // don't check values with more items than min pop
  if (pop >= *min_pop)
    return;

  // start from the max number to get minimum target
  for(int i=n-1; i >= 0; i--) {
    uint32_t f = (uint32_t)1<<i;
    if(!(used & f) && sum+nums[i] <= target) {
      combine(used|f, sum+nums[i], target, min_pop, min_qe);
    }
  }
}

void parse(str lines) {
  str line;
  n=0;
  while (str_each_line(&lines, &line)) {
    nums[n++] = str_to_long(line);
  }
}
void day24(Day *day) {
  long total = 0;
  parse(day->input);
  for(int i=0;i<n;i++) total += nums[i];

  // find smallest size to get a sum of third
  long min_qe = 999999999999999999;
  long min_pop = n;
  combine(0, 0, total/3, &min_pop, &min_qe);
  PART1(min_qe);

  min_qe = 99999999999999999;
  min_pop = n;
  combine(0, 0, total/4, &min_pop, &min_qe);
  PART2(min_qe);

}
