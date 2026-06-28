#include "../aoc.h"

// create a trie of all replacements
typedef struct trie {
  struct trie *children;
  uint8_t children_count;
  uint8_t children_capacity;
  char letter;
  bool is_leaf; // if this node is a molecule
} trie;

long trie_leaf_count(trie *at) {
  long c = 0;
  if (at->is_leaf) {
    c++;
  }
  for (int i = 0; i < at->children_count; i++) {
    c += trie_leaf_count(&at->children[i]);
  }
  return c;
}

trie *trie_at(trie *at, str string) {
  if (!string.len)
    return at;
  char letter = string.data[0];
  // check if we have a child for the letter
  for (int i = 0; i < at->children_count; i++) {
    if (at->children[i].letter == letter) {
      return trie_at(&at->children[i], str_drop(string, 1));
    }
  }

  // no child, we need to add one
  if (at->children_capacity == at->children_count) {
    uint8_t new_capacity = (at->children_capacity ? at->children_capacity * 2 : 8);
     at->children_capacity = new_capacity;

    at->children =
      realloc(at->children, sizeof(struct trie) * at->children_capacity);
    if (!at->children) {
      fprintf(stderr, "No memory!\n");
      exit(1);
    }
  }
  int idx = at->children_count++;

  at->children[idx] = (trie){.letter = letter};
  return trie_at(&at->children[idx], str_drop(string, 1));
}


typedef struct Replacement {
  str from;
  str to;
} Replacement;

Replacement replace[64];
int last_replacement = 0;

str molecule;

void parse(str lines) {
  str line;
  while (str_each_line(&lines, &line)) {
    if (line.len == 0)
      break;
    int idx = last_replacement++;
    str from, to;
    if (!str_scan("%s => %s", line, &from, &to))
      goto badline;
    from = str_trim(from);
    to = str_trim(to);
    replace[idx] = (Replacement){.from = from, .to = to};
  }
  molecule = str_trim(lines);
  return;
badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}

long solve() {
  long choices = 0;
  trie root = {0};
  trie *pfx = &root;
  for (int i = 0; i < molecule.len; i++) {
    str prefix = str_take(molecule, i);
    str suffix = str_drop(molecule, i);
    for (int r = 0; r < last_replacement; r++) {
      Replacement rpl = replace[r];
      if (str_startswith(suffix, rpl.from)) {
        trie *at = pfx;
        at = trie_at(at, rpl.to);
        at = trie_at(at, str_drop(suffix, rpl.from.len));
        at->is_leaf = true;
      }
    }
    pfx = trie_at(pfx, str_take(suffix, 1));
  }

  // count leaves
  return trie_leaf_count(&root);
}

/* part2, borrowed from:
 *  https://www.reddit.com/r/adventofcode/comments/3xflz8/comment/cy4h7ji/
 */
long reductions() {
  long uppers = 0, Rn = 0, Ar = 0, Y = 0;
  char *d = molecule.data;
  size_t l = molecule.len;
  for (int i = 0; i < l; i++) {
    if (d[i] == 'A' && i < l - 1 && d[i+1] == 'r')
      Ar++;
    if (d[i] == 'R' && i < l - 1 && d[i+1] == 'n')
      Rn++;
    if (d[i] >= 'A' && d[i] <= 'Z')
      uppers++;
    if(d[i] == 'Y') Y++;
  }
  return uppers - Rn - Ar - 2*Y - 1;
}

void day19(Day *day) {
  parse(day->input);
  PART1(solve());
  PART2(reductions());
}
