#include "../aoc.h"
#include "../stb_ds.h"
#include <stdint.h>

#define MAX_NODES 8
typedef struct Node {
  str name;
  long distance[MAX_NODES];
} Node;

Node nodes[MAX_NODES] = {0};
int last_node = 0;

int node_by_name(str name) {
  for (int i = 0; i < last_node; i++) {
    if(str_eq(nodes[i].name, name)) return i;
  }
  int idx = last_node;
  nodes[idx] = (Node){.name = name};
  //printf("Adding node %d: %.*s\n", idx, (int)name.len, name.data);
  last_node ++;
  return idx;
}

void print_visited(uint8_t visited) {
  for (int i = 0; i < MAX_NODES; i++) {
    if (visited & ((uint8_t)1 << i))
      printf(" [X]");
    else
      printf(" [ ]");
    printf(" %.*s", (int)nodes[i].name.len, nodes[i].name.data);
  }
  printf("\n");
}

void visit(int depth, int at, uint8_t visited, long accum, long *shortest, long *longest) {
  // if all is visited, check the accumulator
  if (visited == 0b11111111) {
    //printf("=> all visited, end up at %.*s, accum: %ld\n", (int)nodes[at].name.len, nodes[at].name.data, accum);
    // all 7 places visited
    if (accum < *shortest)
      *shortest = accum;
    if (accum > *longest)
      *longest = accum;
  } else {
    // recurse into visiting all unvisited nodes
    for (int i = 0; i < MAX_NODES; i++) {
      if(i == at) continue;
      uint8_t f = (uint8_t)1 << i;
      if (!(visited & f) && (nodes[at].distance[i] || nodes[i].distance[at])) {
        long dist = nodes[at].distance[i];
        if(!dist) dist = nodes[i].distance[at];
        //for(int p=0;p<depth;p++) printf(" ");
        /*printf("from %.*s TO %.*s (+ %ld distance)\n", (int)nodes[at].name.len,
               nodes[at].name.data, (int)nodes[i].name.len, nodes[i].name.data,
               dist);*/
        //print_visited(visited|f);
        visit(depth+2, i, visited | f, accum + dist, shortest, longest);
      }
    }
  }
}

void visit_all(long *shortest, long *longest) {
  for (int i = 0; i < MAX_NODES; i++) {
    visit(0, i, (uint8_t)1 << i, 0, shortest, longest);
  }
}

void day9(Day *day) {
  str lines = day->input;
  str line, line_orig;
  while (str_each_line(&lines, &line)) {
    line_orig = line;
    str name, dest;
    long distance;
    if (!str_splitat(line, " ", &name, &line))
      goto badline;
    if (!str_startswith(line, str_constant("to ")))
      goto badline;
    line = str_drop(line, 3);
    if (!str_splitat(line, " ", &dest, &line))
      goto badline;
    if (!str_startswith(line, str_constant("= ")))
      goto badline;
    line = str_drop(line, 2);
    distance = str_to_long(line);
    int from = node_by_name(name);
    int to = node_by_name(dest);
    nodes[from].distance[to] = distance;
  }

  long shortest = 99999999999;
  long longest = 0;
  visit_all(&shortest, &longest);
  PART1(shortest);
  PART2(longest);
  return;
 badline:
  fprintf(stderr, "bad line: %.*s\n  at pos: %.*s\n", (int)line_orig.len, line_orig.data,
          (int)line.len, line.data);
}
