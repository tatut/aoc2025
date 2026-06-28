#include "../aoc.h"

typedef enum Analyses {
  CHILDREN, CATS, SAMOYEDS, POMERANIANS, AKITAS, VIZSLAS, GOLDFISH, TREES, CARS, PERFUMES
} Analyses;
#define MAX_FIELDS 10

str analyses_name[] = {
  [CHILDREN] = str_constant("children"),
  [CATS] = str_constant("cats"),
  [SAMOYEDS] = str_constant("samoyeds"),
  [POMERANIANS] = str_constant("pomeranians"),
  [AKITAS] = str_constant("akitas"),
  [VIZSLAS] = str_constant("vizslas"),
  [GOLDFISH] = str_constant("goldfish"),
  [TREES] = str_constant("trees"),
  [CARS] = str_constant("cars"),
  [PERFUMES] = str_constant("perfumes")
};

typedef struct Value {
  long value;
  bool known;
} Value;

typedef struct Sue {
  Value value[10];
} Sue;

Sue sues[500] = {0};

bool parse_field(str field, Sue *sue) {
  field = str_trim(field);
  str name;
  long value;
  if (!str_scan("%s: %ld", field, &name, &value))
    return false;
  for (int i = 0; i < MAX_FIELDS; i++) {
    if (str_eq(name, analyses_name[i])) {
      sue->value[i] = (Value){.value = value, .known = true};
      return true;
    }
  }
  return false;
}

void parse(str lines) {
  str line;
  while (str_each_line(&lines, &line)) {
    long idx;
    str fields;
    if (!str_scan("Sue %ld: %s", line, &idx, &fields))
      goto badline;
    str field;
    while (str_splitat(fields, ",", &field, &fields)) {
      if(!parse_field(field, &sues[idx-1])) goto badline;
    }
    if(!parse_field(fields, &sues[idx-1])) goto badline;

  }

  return;
badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}

bool match(Sue *a, Sue *b) {
  for (int i = 0; i < MAX_FIELDS; i++) {
    // if both values are known, but not the same => no match
    if (a->value[i].known && b->value[i].known &&
        a->value[i].value != b->value[i].value)
      return false;
  }
  return true;
}

bool match2(Sue *a, Sue *b) {
  for (int i = 0; i < MAX_FIELDS; i++) {
    if (i == CATS || i == TREES) {
      if (b->value[i].known && b->value[i].value <= a->value[i].value)
        return false;
    } else if (i == POMERANIANS || i == GOLDFISH) {
      if (b->value[i].known && b->value[i].value >= a->value[i].value)
        return false;
    } else {
      if (a->value[i].known && b->value[i].known &&
          a->value[i].value != b->value[i].value)
        return false;
    }
  }
  return true;
}
void day16(Day *day) {
  parse(day->input);

  Sue sender = (Sue){.value = {
      [CHILDREN]     = (Value){.known = true, .value = 3},
      [CATS]         = (Value){.known = true, .value = 7},
      [SAMOYEDS]     = (Value){.known = true, .value = 2},
      [POMERANIANS]  = (Value){.known = true, .value = 3},
      [AKITAS]       = (Value){.known = true, .value = 0},
      [VIZSLAS]      = (Value){.known = true, .value = 0},
      [GOLDFISH]     = (Value){.known = true, .value = 5},
      [TREES]        = (Value){.known = true, .value = 3},
      [CARS]         = (Value){.known = true, .value = 2},
      [PERFUMES]     = (Value){.known = true, .value = 1},
    }};

  for (int i = 0; i < 500; i++) {
    if (match(&sender, &sues[i])) {
      PART1(i + 1);
    }
    if (match2(&sender, &sues[i])) {
      PART2(i + 1);
    }
  }
}
