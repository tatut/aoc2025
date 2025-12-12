#include "aoc.h"
#include <stdio.h>

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint8_t u8;

// indicators encoding:
// u32 with
// high 8 bits = number of indicators
// low bits = target state
//
// [.####.#..#] (len 9)
// ........ ........ ........ ........
// 00001010 00000000 00000010 01011110
// ^^len 9^               ^^bits on ^^ reversed!
//
// button is a u16 with the bits that it toggles on
// (3,4,6,7,9)
//
// ........ ........
// 00000010 11011000
//       9  76 43
//
// a combination of currently on and button
// is u32
// (indicators << 16) + button


// get indicators len
#define LEN(ind) ((ind) >> 24)

// check if machine indicator target is on or button toggles
#define ON(x, n) ((x) & (1 << (n)))

// toggle specific indicator bit
#define TOGGLE(x, n) ((x) ^= (1 << (n)))

// hacky, but array pointers are poor values as not assignable directly
typedef struct joltages {
  u16 j0, j1, j2, j3, j4, j5, j6, j7, j8, j9;
  u8 count;
} joltages;

typedef struct machine {
  u32 indicators;
  u16 buttons[16];
  joltages joltage;
} machine;

void set_joltage(joltages *p, int i, u16 value) {
  switch (i) {
  case 0: p->j0 = value; break;
  case 1: p->j1 = value; break;
  case 2: p->j2 = value; break;
  case 3: p->j3 = value; break;
  case 4: p->j4 = value; break;
  case 5: p->j5 = value; break;
  case 6: p->j6 = value; break;
  case 7: p->j7 = value; break;
  case 8: p->j8 = value; break;
  case 9: p->j9 = value; break;
  default:
    fprintf(stderr, "Max joltages is 10 (0 - 9)\n");
    exit(1);
  }
  if(p->count < i+1) p->count = i+1;
}

u16 get_joltage(joltages *p, int i) {
  switch (i) {
  case 0: return p->j0;
  case 1: return p->j1;
  case 2: return p->j2;
  case 3: return p->j3;
  case 4: return p->j4;
  case 5: return p->j5;
  case 6: return p->j6;
  case 7: return p->j7;
  case 8: return p->j8;
  case 9: return p->j9;
  }
  fprintf(stderr, "Joltage index out of range %d, 0 - 9\n", i);
  exit(1);
}

long combined_joltage(joltages *p) {
  return p->j0 + p->j1 + p->j2 + p->j3 + p->j4 + p->j5 + p->j6 + p->j7 + p->j8 +
         p->j9;
}

void print_joltages(joltages *p) {
  printf("{");
  for (int i = 0; i < p->count; i++) {
    if(i) printf(",");
    printf("%d", get_joltage(p, i));
  }
  printf("}\n");
}

machine parse_machine(str in) {
  machine m = {0};
  in = str_drop(in, 1);
  char ch = str_char_at(in, 0);
  int len = str_indexof(in, ']');
  m.indicators = len << 24;
  int i=0;
  do {
    if (ch == '#') {
      TOGGLE(m.indicators, i);
    }
    i++;
    in = str_drop(in, 1);
    ch = str_char_at(in, 0);
  } while(ch != ']');
  in = str_ltrim(str_drop(in,1));
  ch = str_char_at(in, 0);
  i = 0;
  do {
    str list, idx;
    str_splitat(str_drop(in,1), ")", &list, &in);
    u16 b = 0;

    while (str_splitat(list, ",", &idx, &list)) {
      long indicator = str_to_long(idx);
      TOGGLE(b, indicator);
    }
    if (list.len) {
      long indicator = str_to_long(list);
      TOGGLE(b, indicator);
    }
    m.buttons[i++] = b;
    in = str_drop(in, 1);
    ch = str_char_at(in, 0);
  } while (ch == '(');

  in = str_ltrim(str_drop(in, 1));
  in.len--; // ignore ending '}'
  str joltage;
  i = 0;
  while (str_splitat(in, ",", &joltage, &in)) {
    set_joltage(&m.joltage, i++, str_to_long(joltage));
  }
  if (in.len)
    set_joltage(&m.joltage, i++, str_to_long(in));
  return m;
}

void print_machine(machine m) {
  printf("[");
  for (int i = 0; i < LEN(m.indicators); i++) {
    printf("%c", ON(m.indicators, i) ? '#' : '.');
  }
  printf("]");
  for (int i = 0; i < 16; i++) {
    if(!m.buttons[i]) break;
    printf(" (");
    bool first = true;
    for (int b = 0; b < 16; b++) {
      if (ON(m.buttons[i], b)) {
        if (!first)
          printf(",");
        printf("%d", b);
        first = false;
      }
    }
    printf(")");
  }

  printf(" {");
  for (int i = 0; i < m.joltage.count; i++) {
    if(i) printf(",");
    printf("%d", get_joltage(&m.joltage, i));
  }
  printf("}");
}

/* use A* to get to target nodes, each button is a possible neighbor,
 * the distance is how many indicator indexes are in correct state
 *
 * [..#.#] vs
 * [.#..#]
 * => 2 distance, idx 1 and 2 differ
 *
 * when all are the same, we have reached the target
 */

void print_indicators(int len, uint16_t on) {
  printf("[");
  for (int i = 0; i < len; i++) {
    printf("%c", ON(on, i) ? '#' : '.');
  }
  printf("]\n");
}

/* hashtable structure to keep node scores */
struct score {
  u32 key;
  long value;
};

struct camefrom {
  u16 key;
  u16 value;
};

struct openset {
  u32 node;
  long score;
};

struct score_j {
  joltages key;
  long value;
};

struct camefrom_j {
  joltages key;
  joltages value;
};

struct openset_j {
  joltages node;
  long score;
};

#define INITIAL_SCORE 999999999

long get_score(struct score *scores, u32 node) {
  int idx = hmgeti(scores, node);
  if (idx == -1) {
    return INITIAL_SCORE;
  } else {
    return scores[idx].value;
  }
}

long get_score_j(struct score_j *scores, joltages node) {
  int idx = hmgeti(scores, node);
  if (idx == -1) {
    return INITIAL_SCORE;
  } else {
    return scores[idx].value;
  }
}

long distance(machine m, u32 node) {
  u16 indicators = node >> 16;
  long d = 0;
  for (int i = 0; i < LEN(m.indicators); i++) {
    if(ON(indicators, i) != ON(m.indicators, i)) d++;
  }
  return d;
}

long distance_j(joltages *a, joltages *b) {
  long d = 0;
  for (int i = 0;i < a->count; i++) {
    d += abs( (int) get_joltage(a, i) - (int) get_joltage(b, i));
  }
  return d;
}

void enqueue(struct openset **openSet, struct openset item) {
  // add to array from end, while item.score > at.score
  int len = arrlen(*openSet);
  for (int i = 0; i < len; i++) {
    if((*openSet)[i].node == item.node) return;
  }
  if (len == 0 || item.score < (*openSet)[len-1].score) {
    arrput(*openSet, item);
  } else {
    for (int p = arrlen(*openSet)-1; p; p--) {
      if (item.score < (*openSet)[p-1].score) {
        arrins(*openSet, p, item);
        return;
      }
    }
    arrins(*openSet, 0, item);
  }
}

void enqueue_j(struct openset_j **openSet, struct openset_j item) {
  // add to array from end, while item.score > at.score
  int len = arrlen(*openSet);
  for (int i = 0; i < len; i++) {
    if(memcmp(&(*openSet)[i].node, &item.node, sizeof(joltages))==0) return;
  }
  if (len == 0 || item.score < (*openSet)[len-1].score) {
    arrput(*openSet, item);
  } else {
    for (int p = arrlen(*openSet)-1; p; p--) {
      if (item.score < (*openSet)[p-1].score) {
        arrins(*openSet, p, item);
        return;
      }
    }
    arrins(*openSet, 0, item);
  }
}

int least_presses(machine m) {
  struct openset *openSet = {0};
  struct score *gScore = {0};
  struct score *fScore = {0};
  struct camefrom *cameFrom = {0};

  u16 target = (m.indicators & 0xFFFF);
  u16 node = 0; // start node is all indicators of,
  hmput(gScore, node, 0);
  hmput(fScore, node, distance(m, 0));

  struct openset from = (struct openset){.node = 0, .score=0};
  enqueue(&openSet, from);

  long result = -1;

  while (arrlen(openSet)) {
    //printf(" openset: %ld\n", arrlen(openSet));
    node = arrpop(openSet).node;
    //print_indicators(LEN(m.indicators), node>>16);
    if (node == (m.indicators & 0xFFFF)) {
      int len = 0;
      u32 at = node;
      int idx = hmgeti(cameFrom, at);
      while(idx != -1) {
        at = cameFrom[idx].value;
        len++;
        idx = hmgeti(cameFrom, at);
      }
      result = len;
      goto done;
    } else {
      // go through all neighbors, every button is a possible neighbor
      for (int i = 0; i < 16; i++) {
        if (m.buttons[i]) {
          u16 b = m.buttons[i];
          u16 neighbor = node;
          for (int i = 0; i < 16; i++) {
            if (ON(b, i)) {
              TOGGLE(neighbor, i);
            }
          }

          long tentative_gScore = get_score(gScore, node) + 1;
          long current_gScore = get_score(gScore, neighbor);

          if (tentative_gScore < current_gScore) {
            // better than previous path
            hmput(cameFrom, neighbor, node);
            hmput(gScore, neighbor, tentative_gScore);
            long fs = tentative_gScore;
            // distance is useless as we calculate steps only
            // + distance(m, neighbor);
            hmput(fScore, neighbor, fs);
            enqueue(&openSet, (struct openset){.node = neighbor, .score=fs});
          }
        }
      }
    }
  }
 done:
  hmfree(gScore);
  hmfree(fScore);
  hmfree(cameFrom);
  arrfree(openSet);
  if(result == -1) {
    fprintf(stderr, "didn't find solution!");
    exit(1);
  }
  return result;
 }


int popcount(u16 b) {
  int c = 0;
  for (int i = 0; i < 16; i++) {
    if (b & (1 << i))
      c++;
  }
  return c;
}

int compare_popcount(const void *a, const void *b) {
  u16 b1 = *(u16 *)a;
  u16 b2 = *(u16 *)b;
  return popcount(b2) - popcount(b1);
}

bool below_threshold(joltages *at, joltages *target) {
  for (int i = 0; i < at->count; i++) {
    int at_j = get_joltage(at, i);
    int target_j = get_joltage(target, i);
    if( (target_j - at_j) < 3) return false; // check correct
  }
  return true;
}

typedef struct button_bounds {
  int b;
  int min;
  int max;
} button_bounds;

int desired_joltage(machine m) {

  // FIXME: use https://github.com/starwing/amoeba solver


  // for each joltage calculate how many times a given button
  // must be pressed at minimum and at maximum
  button_bounds *bs = {0};

  for (int i = 0; i < 16; i++) {
    if (m.buttons[i]) {
      button_bounds b =
          (button_bounds){.b = m.buttons[i], .min = -1, .max = -1};
      //
    }
  }

  struct openset_j *openSet = {0};
  struct score_j *gScore = {0};
  struct score_j *fScore = {0};
  struct camefrom_j *cameFrom = {0};

  // sort buttons by popcount
  qsort(&m.buttons, 16, sizeof(u16), compare_popcount);
  for (int i = 0; i < 16; i++) {
    printf("button %d pc %d : ", i, popcount(m.buttons[i]));
    print_indicators(16, m.buttons[i]);
  }
  // click that until it would go over, then continue A*
  long pre_rounds = 0;

  joltages target = m.joltage;

  joltages node = {0};
  node.count = m.joltage.count;

  // take the button that would increment the most while every joltage
  // is below threshold, only then start doing A* for rest
  while (below_threshold(&node, &target)) {
    // use the button that causes biggest increment
    joltages next = node;
    int max_incr = 0;
    for (int i = 0; i < 16; i++) {
      joltages incr = node;
      if (m.buttons[i]) {
        int inc = 0;
        for (int j = 0; j < 16; j++) {
          if (ON(m.buttons[i], j)) {
            set_joltage(&incr, j, 1 + get_joltage(&incr, j));
            inc++;
          }
        }
        if (inc > max_incr) {
          printf("inc %d  ", inc); print_joltages(&incr);
          next = incr;
          max_incr = inc;
        }
      }
    }
    pre_rounds++;
    node = next;
  }
  printf("pre_rounds: %ld  node: ", pre_rounds); print_joltages(&node);

  hmput(gScore, node, 0);
  hmput(fScore, node, distance(m, 0));

  struct openset_j from = (struct openset_j){.node = node, .score=0};
  enqueue_j(&openSet, from);

  long result = -1;

  while (arrlen(openSet)) {
    //printf(" openset: %ld\n", arrlen(openSet));
    node = arrpop(openSet).node;
    //print_joltages(&node);
    if (memcmp(&node, &target, sizeof(joltages)) == 0) {
      int len = 0;
      joltages at = node;
      int idx = hmgeti(cameFrom, at);
      while(idx != -1) {
        at = cameFrom[idx].value;
        len++;
        idx = hmgeti(cameFrom, at);
      }
      result = len;
      goto done;
    } else {
      // go through all neighbors, every button is a possible neighbor
      for (int i = 0; i < 16; i++) {
        if (m.buttons[i]) {
          //printf(" button: "); print_indicators(target.count, m.buttons[i]);
          u16 b = m.buttons[i];
          joltages neighbor = node;
          for (int i = 0; i < 16; i++) {
            if (ON(b, i)) {
              set_joltage(&neighbor, i, 1 + get_joltage(&neighbor, i));
            }
          }

          // check if this is valid node, if any thing is over
          bool valid = true;
          for (int i = 0; i < target.count; i++) {
            if (get_joltage(&neighbor, i) > get_joltage(&target, i)) {
              valid = false;
              //printf("invalid state! "); print_joltages(&neighbor); print_joltages(&target);
              break;
            }
          }

          //printf(" new joltages: "); print_joltages(&neighbor);
          long tentative_gScore = get_score_j(gScore, node) + 1;
          long current_gScore = get_score_j(gScore, neighbor);

          if (tentative_gScore < current_gScore) {
            // better than previous path
            hmput(cameFrom, neighbor, node);
            hmput(gScore, neighbor, tentative_gScore);
            long fs = tentative_gScore + distance_j(&target, &neighbor);
            hmput(fScore, neighbor, fs);
            enqueue_j(&openSet, (struct openset_j){.node = neighbor, .score=fs});
          }
        }
      }
    }
  }
 done:
  hmfree(gScore);
  hmfree(fScore);
  hmfree(cameFrom);
  arrfree(openSet);
  if(result == -1) {
    fprintf(stderr, "didn't find joltage solution!");
    exit(1);
  }
  return pre_rounds+result;
}



void day10(str input) {
  machine *ms = {0};
  str line;
  while (str_splitat(input, "\n", &line, &input)) {
    arrput(ms, parse_machine(line));
  }

  int part1 = 0, part2 = 0;

  for (int i = 0; i < arrlen(ms); i++) {
    printf("Machine %d: ", i);
    print_machine(ms[i]);
    printf("\n");

    part1 += least_presses(ms[i]);
    part2 += desired_joltage(ms[i]);
    //printf(" => %d presses\n", d);
  }

  printf("Part1: %d\nPart2: %d\n", part1, part2); // 409 is right
  // part2 = 15489

}
