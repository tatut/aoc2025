#include "aoc.h"

void day7(str input) {
  bool *beams = NULL; // sorted X positions of beam
  long *ts;
  str line;
  str_splitat(input, "\n", &line, &input);
  // find start pos
  int len = line.len;
  beams = malloc(sizeof(bool)*len);
  memset(beams, 0, sizeof(bool)*len);
  ts = malloc(sizeof(long)*len); // count timelines
  memset(ts, 0, sizeof(long)*len);

  for(int i=0;i<line.len;i++) {
    if(line.data[i] == 'S') {
      beams[i] = true;
      ts[i] = 1;
      break;
    }
  }

  // Go through rest of lines
  int splits = 0;
  while(str_splitat(input, "\n", &line, &input)) {
    for(int i=0;i<line.len;i++) {
      if(line.data[i] == '^') {
        if(beams[i]) splits++;
        beams[i] = 0;
        beams[i-1] = 1;
        beams[i+1] = 1;

        long b = ts[i];
        ts[i] -= b;
        ts[i-1] += b;
        ts[i+1] += b;
      }
    }
  }
  long timelines = 0;
  for(int i=0;i<len;i++) {
    timelines += ts[i];
  }

  printf("Part1: %d\nPart2: %ld\n", splits, timelines);
}
