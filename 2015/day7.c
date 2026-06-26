#include "../aoc.h"
#include <unistd.h>
// input type
typedef enum itype { IN_VALUE, IN_GATE } itype;

// gate operation type
typedef enum optype {
  GATE_NOP, // direct value is 1st input
  GATE_AND, // and the inputs
  GATE_OR,  // or the inputs
  GATE_NOT,    // complement the 1st input
  GATE_LSHIFT, // shift 1st input left by 2nd input
  GATE_RSHIFT  // shift 1st input right by 2nd input
} optype;

typedef struct input {
  itype type;
  uint16_t value;
} input;

typedef struct gate {
  input left;
  input right;
  optype type;
  bool has_value;
  uint16_t value;
  bool used;
} gate;

#define GATE_IDX(s) (((s).data[0] - 96)*27 + ((s).len > 1 ? ((s).data[1] - 96) : 0))

#define GATE_IDX_FMT "%c%c"
#define GATE_IDX_ARG(g) ((g) / 27 + 96), ((g) % 27 ? ((g) % 27 + 96) : ' ')

gate gates[27*27] = {0};

bool is_letter(char ch) { return ch >= 'a' && ch <= 'z'; }
bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }

uint16_t read_gate_idx(str *line) {
  uint16_t idx = (line->data[0] - 'a') * 26;
  if (line->len == 1 || !is_letter(line->data[0])) {
    *line = str_drop(*line, 1);
  } else {
    idx += (line->data[1] - 'a');
    *line = str_drop(*line, 2);
  }
  return idx;
}

bool read_input(input *in, str *line) {
  if (!line->len)
    return false;
  str inp;
  if (!str_splitat(*line, " ", &inp, line)) {
    inp = *line;
    *line = (str){.len = 0};
  }
  //printf("PARSE INPUT at: %.*s\n", (int)inp.len, inp.data);
  if (is_digit(inp.data[0])) {
    in->value = str_to_long(inp);
    in->type = IN_VALUE;
  } else if (is_letter(inp.data[0])) {
    in->value = GATE_IDX(inp);
    in->type = IN_GATE;
  } else {
    printf(" not digit or letter! %c\n", inp.data[0]);
    return false;
  }
  return true;
}

bool input_ready(input in) {
  return (in.type == IN_VALUE || gates[in.value].has_value);
}

uint16_t eval(uint16_t idx);
uint16_t eval_input(input in) {
  if (in.type == IN_VALUE) {
    return in.value;
  } else {
    return eval(in.value);
  }
}


uint16_t eval(uint16_t idx) {
  gate g = gates[idx];
  if (g.has_value)
    return g.value;
  uint16_t l, r,out;
  l = eval_input(g.left);
  if (g.type != GATE_NOP && g.type != GATE_NOT)
    r = eval_input(g.right);
  switch (g.type) {
  case GATE_NOP:
    out = l;
    break;
  case GATE_NOT:
    out = ~l;
    break;
  case GATE_AND:
    out = l & r;
    break;
  case GATE_OR:
    out = l | r;
    break;
  case GATE_LSHIFT:
    out = l << r;
    break;
  case GATE_RSHIFT:
    out = l >> r;
    break;
  }
  gates[idx].has_value = true;
  gates[idx].value = out;
  return out;
}

void print_input(input in) {
  if (in.type == IN_VALUE) {
    printf("%u", in.value);
  } else {
    printf(GATE_IDX_FMT, GATE_IDX_ARG(in.value));
  }
}
void print_gate(uint16_t idx, gate g) {
  if (g.type == GATE_NOP) {
    print_input(g.left);
  } else if (g.type == GATE_NOT) {
    printf("NOT ");
    print_input(g.left);
  } else {
    print_input(g.left);
    switch (g.type) {
    case GATE_LSHIFT:
      printf(" LSHIFT ");
      break;
    case GATE_RSHIFT:
      printf(" RSHIFT ");
      break;
    case GATE_AND:
      printf(" AND ");
      break;
    case GATE_OR:
      printf(" OR ");
      break;
    default:
      printf("¯\\_(ツ)_/¯");
    }
    print_input(g.right);
  }
  printf(" -> "GATE_IDX_FMT"\n", GATE_IDX_ARG(idx));
}

void day7(Day *day) {
  str lines = day->input;
  str line;
  str orig_line;
  while (str_each_line(&lines, &line)) {
    orig_line = line;
    gate g = (gate){.used = true};
    if (str_startswith(line, str_constant("NOT "))) {
      g.type = GATE_NOT;
      line = str_drop(line, 4);
      if (!read_input(&g.left, &line))
        goto badline;
    } else {
      if (!read_input(&g.left, &line))
        goto badline;
      if (str_startswith(line, str_constant("-> "))) {
        // no operation, just direct output
        g.type = GATE_NOP;
      } else if (str_startswith(line, str_constant("AND "))) {
        g.type = GATE_AND;
        line = str_drop(line, 4);
        if (!read_input(&g.right, &line))
          goto badline;
      } else if (str_startswith(line, str_constant("LSHIFT "))) {
        g.type = GATE_LSHIFT;
        line = str_drop(line, 7);
        if (!read_input(&g.right, &line))
          goto badline;
      } else if (str_startswith(line, str_constant("RSHIFT "))) {
        g.type = GATE_RSHIFT;
        line = str_drop(line, 7);
        if (!read_input(&g.right, &line))
          goto badline;
      } else if (str_startswith(line, str_constant("OR "))) {
        g.type = GATE_OR;
        line = str_drop(line, 3);
        if (!read_input(&g.right, &line))
          goto badline;
      } else {
        goto badline;
      }
    }
    if (!str_startswith(line, str_constant("-> ")))
      goto badline;
    line = str_drop(line, 3);
    uint16_t idx = GATE_IDX(line);

    gates[idx] = g;
    //print_gate(idx, g);

  }

  str a = str_constant("a");
  str b = str_constant("b");
  str zz = str_constant("zz");
  uint16_t gate_a_idx = GATE_IDX(a);
  uint16_t gate_b_idx = GATE_IDX(b);
  uint16_t gate_zz_idx = GATE_IDX(zz);
  long v = eval(gate_a_idx);
  PART1(v);

  for (uint16_t i = gate_a_idx; i < gate_zz_idx; i++) {
    gates[i].has_value = false;
  }
  gates[gate_b_idx].type = GATE_NOP;
  gates[gate_b_idx].left = (input){.type = IN_VALUE, .value = (uint16_t) v};
  v = eval(gate_a_idx);
  PART2(v);

  return;
badline:
  fprintf(stderr, "Bad line: %.*s\n     pos: %.*s\n", (int)orig_line.len,
          orig_line.data,
          (int) line.len, line.data);
}
