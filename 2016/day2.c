#include "../aoc.h"

char buttons[3*3] = {'1','2','3','4','5','6','7','8','9'};
char buttons2[5 * 5] = {
    0,   0,   '1', 0,   0,   // line 1
    0,   '2', '3', '4', 0,   // line 2
    '5', '6', '7', '8', '9', // line 3
    0,   'A', 'B', 'C', 0,   // line 4
    0,   0,   'D', 0,   0    // line 5
};

char btn(int x,int y) { return buttons[(y * 3) + x]; }
char btn2(int x,int y) { return buttons2[(y * 5) + x]; }


// button position is 2 array of int

void move(int *from, char dir) {
  switch(dir) {
  case 'U': {
    if(from[1]) from[1]--;
    break;
  }
  case 'D': {
    if(from[1] < 2) from[1]++;
    break;
  }
  case 'L': {
    if(from[0]) from[0]--;
    break;
  }
  case 'R': {
    if (from[0] < 2) from[0]++;
    break;
  }
  }
}

void move2(int *from, char dir) {
  switch (dir) {
  case 'U': {
    if(from[1] && btn2(from[0],from[1]-1)) from[1]--;
    break;
  }
  case 'D': {
    if(from[1] < 4 && btn2(from[0],from[1]+1)) from[1]++;
    break;
  }
  case 'L': {
    if(from[0] && btn2(from[0]-1,from[1])) from[0]--;
    break;
  }
  case 'R': {
    if (from[0] < 4 && btn2(from[0]+1,from[1])) from[0]++;
    break;
  }
  }
}
void solve(Answer *a, str lines, int x, int y, char (*btnfn)(int,int), void (*movefn)(int*,char)) {
  int p[] = {x,y};
  str line;
  char p1[10] = {0};
  int p1_idx = 0;
  while(str_each_line(&lines, &line)) {
    while(line.len) {
      movefn(p, line.data[0]);
      line = str_drop(line, 1);
    }
    printf("button at (%d, %d) = %c\n", p[0], p[1], btnfn(p[0],p[1]));
    p1[p1_idx++] = btnfn(p[0],p[1]);
  }
  set_str_answer(a, (str){.data = p1, .len = p1_idx});

}
void day2(Day *day) {
  solve(&day->part1, day->input, 1, 1, btn, move);
  solve(&day->part2, day->input, 0, 2, btn2, move2);
}
