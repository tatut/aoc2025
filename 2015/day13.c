#include "../aoc.h"

#define MAX_PEOPLE 8

typedef struct participant {
  str name;
  long happy[8];
} participant;

participant participants[MAX_PEOPLE] = {0};
int last_participant = 0;

int participant_id(str name) {
  for(int i=0;i<last_participant;i++) {
    if(str_eq(participants[i].name, name)) return i;
  }
  int id = last_participant++;
  participants[id] = (participant){.name = name};
  return id;
}

void parse(str in) {
  str line;
  while (str_each_line(&in, &line)) {
    str name;
    long mul = 1;
    long amount;
    if (!str_splitat(line, " ", &name, &line))
      goto badline;
    int id = participant_id(name);
    if (!str_startswith(line, str_constant("would")))
      goto badline;
    line = str_drop(line,6);
    if (str_startswith(line, str_constant("gain "))) {
      mul = 1;
      line = str_drop(line, 5);
    } else if (str_startswith(line, str_constant("lose "))) {
      mul = -1;
      line = str_drop(line, 5);
    } else {
      goto badline;
    }
    amount = mul * str_to_long_rest(line, &line);
    if (!str_startswith(line,
                        str_constant(" happiness units by sitting next to ")))
      goto badline;
    line = str_drop(line, 36);
    line.len--; // remove last '.'
    int next_to = participant_id(line);
    participants[id].happy[next_to] = amount;

  }

  return;
badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}

void print_participants() {
  for (int i = 0; i < last_participant; i++) {
    for (int j = 0; j < last_participant; j++) {
      if(i == j) continue;
      printf("%.*s %ld next to %.*s\n", (int)participants[i].name.len,
             participants[i].name.data, participants[i].happy[j],
             (int)participants[j].name.len, participants[j].name.data);
    }
  }
}

long score(int *order) {
  int me, n1, n2; // sitting next to
  long score = 0;
  for (int i = 0; i < 8; i++) {
    n1 = (i == 0 ? order[7] : order[i - 1]);
    n2 = (i == 7 ? order[0] : order[i + 1]);
    me = order[i];
    score += participants[me].happy[n1] + participants[me].happy[n2];
  }
  return score;
}

long score2(int *order) {
  int me, n1, n2; // sitting next to
  long score = 0;
  for (int i = 0; i < 8; i++) {
    n1 = (i == 0 ? -1 : order[i - 1]);
    n2 = (i == 7 ? -1 : order[i + 1]);
    me = order[i];
    score += (n1 == -1 ? 0 : participants[me].happy[n1]) +
      (n2 == -1 ? 0 : participants[me].happy[n2]);
  }
  return score;
}

void try(int *order, uint8_t used, int idx, long *optimal1, long *optimal2) {
  if (idx == 8) {
    long s1 = score(order);
    if (s1 > *optimal1)
      *optimal1 = s1;
    long s2 = score2(order);
    if (s2 > *optimal2)
      *optimal2 = s2;
  }
  for (int i = 0; i < 8; i++) {
    uint8_t f = (uint8_t)1 << i;
    if (!(used & f)) {
      order[idx] = i;
      try(order, used | f, idx+1, optimal1, optimal2);
    }
  }
}
void try_all(long *optimal1, long *optimal2) {
  // each selection is 3 bits (0-7) so full number is (3*8) 24 bits
  int order[8] = {0};
  try(order, 0, 0, optimal1, optimal2);
}

void day13(Day *day) {
  parse(day->input);
  //print_participants();
  long optimal1 = -9999999999;
  long optimal2 = -9999999999;
  try_all(&optimal1, &optimal2);
  PART1(optimal1);
  PART2(optimal2);
}
