#include "../aoc.h"

long boss_hp, boss_damage;

/* Magic Missile costs   53 mana. 4 damage.
   Drain         costs   73 mana. 2 damage, heals 2 hit points.
   Shield        costs  113 mana. effect 6 turns. armor is increased by 7.
   Poison        costs  173 mana. effect 6 turns. deals the boss 3 damage.
   Recharge      costs  229 mana. effect 5 turns. gives you 101 new mana.
*/


// the current game state
typedef struct Game {
  uint16_t mana, mana_used, hp, boss_hp;
  uint8_t shield_turns, poison_turns, recharge_turns;
} Game;

// Use DFS to search through all possible space, shortcutting if
// mana cost would exceed the found minimum
void play(Game g, bool difficult, uint16_t *min_mana_used);


#define MIN_MANA(g)                                                            \
  do {                                                                         \
    if ((g).mana_used < *min_mana_used)                                        \
      *min_mana_used = (g).mana_used;                                          \
  } while (false)


void play_boss(Game g, bool difficult, uint16_t *min_mana_used) {
  /* boss turn */
  int armor = 0;
  if (g.shield_turns) {
    armor = 7;
    g.shield_turns--;
  }
  if (g.poison_turns) {
    if (g.boss_hp <= 3) {
      MIN_MANA(g);
      return;
    }
    g.boss_hp -= 3;
    g.poison_turns--;
  }
  if (g.recharge_turns) {
    g.recharge_turns--;
    g.mana += 101;
  }

  int dmg = boss_damage - armor;
  if (dmg < 1)
    dmg = 1;
  if (g.hp > dmg) {
    // player survived, play next turn
    g.hp -= dmg;
    play(g, difficult, min_mana_used);
  }
}

void play(Game g, bool difficult, uint16_t *min_mana_used) {

  if (difficult) {
    if (g.hp <= 1)
      return;
    g.hp -= 1;
  }

  /* player turn */
  if (g.shield_turns) g.shield_turns--;
  if (g.poison_turns) {
    g.poison_turns--;
    if (g.boss_hp <= 3) {
      MIN_MANA(g);
      return;
    }
    g.boss_hp -= 3;
  }
  if (g.recharge_turns) {
    g.recharge_turns --;
    g.mana += 101;
  }

  // try each spell we can afford, doesn't have effect and doesn't exceed
  // minimum
  Game n;
  if (g.mana >= 53 && g.mana_used+53 < *min_mana_used) {
    n = g;
    n.mana -= 53;
    n.mana_used += 53;
    if (n.boss_hp <= 4) {
      MIN_MANA(n);
    } else {
      n.boss_hp -= 4;
      play_boss(n, difficult, min_mana_used);
    }
  }
  if (g.mana >= 73 && g.mana_used+73 < *min_mana_used) {
    n = g;
    n.mana -= 73;
    n.mana_used += 73;
    n.hp += 2;
    if (n.boss_hp <= 2) {
      MIN_MANA(n);
    } else {
      n.boss_hp -= 2;
      play_boss(n, difficult, min_mana_used);
    }
  }
  if (!g.shield_turns && g.mana >= 113 && g.mana_used+113 < *min_mana_used) {
    n = g;
    n.mana -= 113;
    n.mana_used += 113;
    n.shield_turns = 6;
    play_boss(n, difficult, min_mana_used);
  }
  if (!g.poison_turns && g.mana >= 173 && g.mana_used + 173 < *min_mana_used) {
    n = g;
    n.mana -= 173;
    n.mana_used += 173;
    n.poison_turns = 6;
    play_boss(n, difficult, min_mana_used);
  }
  if (!g.recharge_turns && g.mana >= 229 &&
      g.mana_used + 229 < *min_mana_used) {
    n = g;
    n.mana -= 229;
    n.mana_used += 229;
    n.recharge_turns = 5;
    play_boss(n, difficult, min_mana_used);
  }

}


void day22(Day *day) {
  str_scan("Hit Points: %ld\nDamage: %ld\n", day->input, &boss_hp,
           &boss_damage);
  uint16_t min_mana_p1 = 64000, min_mana_p2 = 64000;
  Game g = (Game){.mana_used = 0, .hp = 50, .mana = 500, .boss_hp = boss_hp};
  play(g, false, &min_mana_p1);
  play(g, true, &min_mana_p2);
  PART1((long)min_mana_p1);
  PART2((long)min_mana_p2);
}
