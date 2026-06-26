#include "../aoc.h"

// password is base27 number
typedef uint64_t pw;

void print_pw(pw pass) {
  char out[8];
  memcpy(out, (void*)&pass, 8);
  printf("%.*s", 8, out);
}

pw to_pw(str input) {
  pw out = 0;
  memcpy(&out, (void*)input.data, 8);
  return out;
}

pw inc(pw in) {
  char out[8];
  memcpy(out, (void*)&in, 8);
  for(int i=7;i>=0;i--) {
    if(out[i] < 'z') {
      out[i]++;
      for(int j=i+1;j<8;j++) out[j] = 'a';
      break;
    }
  }
  pw res;
  memcpy((void*)&res, out, 8);
  return res;
}

bool valid(pw in) {
  char ch[8];
  memcpy(ch, (void*)&in, 8);
  bool has_incr = false;
  int pair1 = -1 , pair2 = -1;

  for(int i=0;i<8;i++) {
    //printf("validating %d\n", i);
    if(i > 0) {
      if(ch[i-1] == ch[i]) {
        // found pair
        if(pair1 == -1) {
          pair1 = i;
        } else if(pair2 == -1 && i - pair1 > 1) {
          pair2 = i;
        }
      }
    }
    //printf(" hep\n");
    if(!has_incr && i > 1) {
      if(ch[i-2] == ch[i-1]-1 && ch[i-1] == ch[i]-1) has_incr = true;
    }
    if(ch[i] == 'i' || ch[i] == 'o' || ch[i] == 'l') return false;
  }
  return has_incr && pair1 != -1 && pair2 != -1;
}

void day11(Day *day) {
  str orig = day->input;
  pw p = to_pw(orig);
  p = inc(p);
  while(!valid(p)){
    p = inc(p);
  }
  str p1 = (str){.len = 8, .data = (char*)&p};
  set_str_answer(&day->part1, p1);

  p = inc(p);
  while(!valid(p)){
    p = inc(p);
  }
  str p2 = (str){.len = 8, .data = (char *)&p};
  set_str_answer(&day->part2, p2);


}
