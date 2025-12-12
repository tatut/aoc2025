#include "aoc.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint64_t u64;

// each shape is 3x3
typedef struct shape {
  u16 data; // u16 original shape
  u8 count; // total number of '#' pieces
} shape;

#define BIT(x,y) (1<<(((y)*3)+(x)))
#define GET(v,x,y) ((v) & BIT(x,y))
#define SET(v,x,y) ((v) |= BIT(x,y))

// rotate
u16 rotate(u16 from) {
  u16 to = 0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      if(GET(from,i,j)) {
        SET(to, 2-j, i);
      }
    }
  }
  return to;
}

u16 flipx(u16 from) {
  u16 to = 0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      if(GET(from,i,j)) {
        SET(to, i-2, j);
      }
    }
  }
  return to;
}

u16 flipy(u16 from) {
  u16 to = 0;
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      if(GET(from,i,j)) {
        SET(to, i, 2-j);
      }
    }
  }
  return to;
}

void print(u16 shape) {
  for(int y=0;y<3;y++) {
    for(int x=0;x<3;x++) {
      putchar(GET(shape,x,y) ? '#' : '.');
    }
    printf("\n");
  }
}

shape shapes[] = {
  (shape){.count = 5, .data = 0b001011110},
  (shape){.count = 7, .data = 0b101111101},
  (shape){.count = 6, .data = 0b100110111},
  (shape){.count = 7, .data = 0b011111101},
  (shape){.count = 7, .data = 0b011011111},
  (shape){.count = 7, .data = 0b101101111}
};

typedef struct region {
  u16 w, h;
  u16 count[6]; // counts of different shapes to fit
} region;



void day12(str input) {
  region *rs = {0};
  str line;
  while(str_each_line(&input, &line)) {
    if(line.len == 0) continue;
    char ch = line.data[1];
    if(ch == ':' || ch == '#' || ch == '.') continue;
    //printf("got region: %.*s\n", (int)line.len, line.data);
    str_indexof(line, 'x');
    region r = (region) {
      .w = str_to_long(str_take(line, 2)),
      .h = str_to_long(str_take(str_drop(line, 3), 2))
    };
    line = str_drop(line, 7);
    for(int i=0;i<6;i++) {
      r.count[i] = str_to_long(str_take(line, 2));
      line = str_drop(line, 3);
    }
    arrput(rs, r);
  }

  long fits = 0;
  for(int i=0;i<arrlen(rs);i++) {
    long needed = 0;
    for(int j=0;j<6;j++) {
      needed += shapes[j].count * rs[i].count[j];
    }
    printf("needed: %ld vs %d\n", needed, rs[i].w*rs[i].h);
    if(needed < rs[i].w*rs[i].h)
      fits++;
  }
  printf("Part1: %ld\n", fits); // 495 was right, no fitting needed!


  // prepare shapes
  for(int i=0;i<6;i++) {
    printf("\n\nshape %d\n", i);
    print(shapes[i].data);
    printf("\n");
    for(int i=0;i<3;i++) {
      print(rotate(shapes[i].data));
    }
  }
}
