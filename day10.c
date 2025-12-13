#include "aoc.h"
#include <stdio.h>

#include "z3.h"
#include "z3_api.h"
#include "z3_optimization.h"

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
  u16 joltage[10];
  u8 count;
} joltages;

typedef struct machine {
  u32 indicators;
  u16 buttons[16];
  joltages joltage;
} machine;

void print_joltages(joltages *p) {
  printf("{");
  for (int i = 0; i < p->count; i++) {
    if(i) printf(",");
    printf("%d", p->joltage[i]);
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
    m.joltage.joltage[i++] = str_to_long(joltage);
  }
  if (in.len)
    m.joltage.joltage[i++] = str_to_long(in);
  m.joltage.count = i;
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
    printf("%d", m.joltage.joltage[i]);
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
Z3_context ctx;
Z3_sort int_sort;

struct openset {
  u32 node;
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

int least_presses(machine m, u16 target) {
  struct openset *openSet = {0};
  struct score *gScore = {0};
  struct score *fScore = {0};
  struct camefrom *cameFrom = {0};

  u16 node = 0; // start node is all indicators of,
  hmput(gScore, node, 0);
  hmput(fScore, node, 0);

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



void z3_init() {
  Z3_config cfg = Z3_mk_config();
  ctx = Z3_mk_context(cfg);
  int_sort = Z3_mk_int_sort(ctx);
}

void z3_deinit() {
  Z3_del_context(ctx);
}


int desired_joltage(machine m, joltages desired) {

  Z3_optimize s = Z3_mk_optimize(ctx);
  Z3_optimize_inc_ref(ctx, s);

  Z3_ast total_clicks = Z3_mk_const(ctx, Z3_mk_string_symbol(ctx, "total"), int_sort);

  Z3_ast btn_clicks[16];
  Z3_ast jolt[10];

  for(int i=0;i<desired.count;i++) {
    jolt[i] = Z3_mk_int(ctx, desired.joltage[i], int_sort);
  }

  Z3_ast zero = Z3_mk_int(ctx, 0, int_sort);

  // create variables for button clicks
  for(int i=0;i<16;i++) {
    if(m.buttons[i]) {
      char buf[4];
      snprintf(buf, 4, "b%d", i);
      btn_clicks[i] = Z3_mk_const(ctx, Z3_mk_string_symbol(ctx, buf), int_sort);
      Z3_optimize_assert(ctx, s, Z3_mk_ge(ctx, btn_clicks[i], zero));
    }
  }

  // make joltage additions, add clicks of each button that increments
  for(int j=0;j<desired.count;j++) {
    Z3_ast btn[16];
    int b=0;

    for(int i=0;i<16;i++) {
      if(ON(m.buttons[i], j)) {
        btn[b++] = btn_clicks[i];
      }
    }
    //printf("joltage has button: %d\n", b);
    Z3_ast add = Z3_mk_add(ctx, b, btn);
    Z3_ast clicks_eq_joltage = Z3_mk_eq(ctx, add, jolt[j]); // combined clicks must equal joltage
    Z3_optimize_assert(ctx, s, clicks_eq_joltage);
  }

  // make final addition for all clicks that equal total clicks
  Z3_ast btn[16];
  int b=0;
  for(int i=0;i<16;i++) {
    if(m.buttons[i]) {
      btn[b++] = btn_clicks[i];
    }
  }
  Z3_ast add = Z3_mk_add(ctx, b, btn);
  Z3_ast eq = Z3_mk_eq(ctx, add, total_clicks);
  Z3_optimize_assert(ctx, s, eq);

  int clicks_idx = Z3_optimize_minimize(ctx, s, total_clicks);


  Z3_lbool sat = Z3_optimize_check(ctx, s, 0, NULL);
  if(sat == Z3_L_TRUE) {
    Z3_model m = Z3_optimize_get_model(ctx, s);
    Z3_ast tc_min = Z3_optimize_get_lower(ctx, s, clicks_idx);
    Z3_ast tc;

    //Z3_model_eval(ctx, m, tc_min, 1, &tc);
    int clicks;
    Z3_get_numeral_int(ctx, tc_min, &clicks);
    return clicks;
  } else {
    fprintf(stderr, "Unsatisfiable!\n");
    exit(1);
  }
}

void day10(str input) {
  machine *ms = {0};
  str line;
  while (str_splitat(input, "\n", &line, &input)) {
    arrput(ms, parse_machine(line));
  }

  int part1 = 0, part2 = 0;

  z3_init();
  for (int i = 0; i < arrlen(ms); i++) {
    part1 += least_presses(ms[i], (ms[i].indicators & 0xFFFF));
    part2 += desired_joltage(ms[i], ms[i].joltage);
  }

  printf("Part1: %d\nPart2: %d\n", part1, part2);

}
