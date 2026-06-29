#include "../aoc.h"

long boss_hp, boss_damage, boss_armor;

void parse(str input) {
  if (!str_scan("Hit Points: %ld\nDamage: %ld\nArmor: %ld\n", input, &boss_hp,
                &boss_damage, &boss_armor)) {
    exit(1);
  }
}

typedef struct Item {
  long cost, damage, armor;
} Item;

Item shop[] = {
  {0}, // empty item
  //                            Weapons:    Cost  Damage  Armor
  {.cost = 8, .damage = 4},  // Dagger        8     4       0
  {.cost = 10, .damage = 5}, // Shortsword   10     5       0
  {.cost = 25, .damage = 6}, // Warhammer    25     6       0
  {.cost = 40, .damage = 7}, // Longsword    40     7       0
  {.cost = 74, .damage = 8}, // Greataxe     74     8       0

  //                            Armor:      Cost  Damage  Armor
  {.cost = 13, .armor = 1},  // Leather      13     0       1
  {.cost = 31, .armor = 2},  // Chainmail    31     0       2
  {.cost = 53, .armor = 3},  // Splintmail   53     0       3
  {.cost = 75, .armor = 4},  // Bandedmail   75     0       4
  {.cost = 102, .armor = 5}, // Platemail   102     0       5

  //                             Rings:      Cost  Damage  Armor
  {.cost = 25, .damage = 1},  // Damage +1    25     1       0
  {.cost = 50, .damage = 2},  // Damage +2    50     2       0
  {.cost = 100, .damage = 3}, // Damage +3   100     3       0
  {.cost = 20, .armor = 1},   // Defense +1   20     0       1
  {.cost = 40, .armor = 2},   // Defense +2   40     0       2
  {.cost = 80, .armor = 3}    // Defense +3   80     0       3
};

/* indexes to what you bought and total cost */
typedef struct Buy {
  int weapon;
  int armor;
  int ring1;
  int ring2;
} Buy;

void play(Buy b, int *costs) {
  int damage =
      shop[b.weapon].damage + shop[b.ring1].damage + shop[b.ring2].damage;
  int armor = shop[b.armor].armor + shop[b.ring1].armor + shop[b.ring2].armor;
  int cost = shop[b.weapon].cost + shop[b.armor].cost + shop[b.ring1].cost +
             shop[b.ring2].cost;
  //printf("Play, W: %d, A: %d, R1: %d, R2: %d, cost: %d, dmg: %d, armor: %d => ", b.weapon, b.armor, b.ring1, b.ring2, cost, damage, armor);
  int round = 1;

  // no need for rounds, we know both start at 100hp
  int bdmg = damage - boss_armor;
  if (bdmg < 1)
    bdmg = 1;
  int mdmg = boss_damage - armor;
  if (mdmg < 1)
    mdmg = 1;

  if (bdmg >=  mdmg) {
    if (cost < costs[0])
      costs[0] = cost;
  } else {
      if (cost > costs[1])
        costs[1] = cost;
  }
}

void buy_rings(Buy b, int *costs) {
  play(b, costs); // try without rings
  for (int r1 = 11; r1 <= 16; r1++) {
    b.ring1 = r1;
    b.ring2 = 0;
    play(b, costs); // 1 ring only
    for (int r2 = 11; r2 <= 16; r2++) {
      if (r1 == r2) // only 1 in inventory of each item
        continue;
      b.ring2 = r2;
      play(b, costs);
    }
  }
}

void buy(int *min_cost) {
  // rules:
  // - must have exactly 1 weapon
  // - armor is optional, at most one
  // - 0-2 rings

  // Iterate through all possible states
  for(int w=1; w <= 5; w++) {
    Buy b = {0};
    b.weapon = w;
    play(b, min_cost); // no armor or rings

    // try with armor
    for (int a = 6; a <= 10; a++) {
      b.armor = a;
      buy_rings(b, min_cost);
    }

    // no armor, but maybe rings
    b.armor = 0;
    buy_rings(b, min_cost);
  }

}
void day21(Day *day) {
  parse(day->input);
  int costs[] = {99999999,0};
  buy(costs);
  PART1(costs[0]);
  PART2(costs[1]);
}
