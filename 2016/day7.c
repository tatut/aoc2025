#include "../aoc.h"

bool abba_enabled(str ip) {
  bool in_hypernet = false;
  bool has_abba = false;
  for(int i=0;i<ip.len-3;i++) {
    if (ip.data[i] == '[') {
      in_hypernet = true;
      continue;
    } else if (ip.data[i] == ']') {
      in_hypernet = false;
      continue;
    } else if (ip.data[i] != ip.data[i + 1] &&
               ip.data[i + 2] == ip.data[i + 1] &&
               ip.data[i + 3] == ip.data[i]) {
      if (in_hypernet)
        return false;
      else
        has_abba = true;
    }
  }
  return has_abba;
}

bool ABs[2 * 26 * 26];

bool ssl_enabled(str ip) {
  // find ABA outside hypernet and corresponding BAB
  // keep flags BAB / ABA, as key of AB (A*26 + B)
  // aba is [2*(a*26 + b)+0]
  // bab is [2*(a*26 + b)+1]
  // we can in the end just check if 2 continuous places
  // are both 1, with stride 2
  bzero(ABs, 2*26*26*sizeof(bool));
  // should use bits to consume less memory, but this is such a small data ¯\_(ツ)_/¯

  bool in_hypernet = false;
  for (int i = 0; i < ip.len - 2; i++) {
    if (ip.data[i] == '[') {
      in_hypernet = true;
      continue;
    } else if (ip.data[i] == ']') {
      in_hypernet = false;
      continue;
    }

    if (ip.data[i] == ip.data[i + 2] && ip.data[i] != ip.data[i + 1] &&
        ip.data[i + 1] != '[') {
      // found ABA/BAB
      int k = in_hypernet ? 1 : 0;
      char A, B;
      if (in_hypernet) {
        A = ip.data[i + 1];
        B = ip.data[i];
      } else {
        A = ip.data[i];
        B = ip.data[i+1];
      }
      ABs[2 * ((A - 'a') * 26 + (B - 'a')) + k] = true;
    }
  }

  for (int i = 0; i < (2 * 26 * 26) - 1; i += 2) {
    if(ABs[i+0] && ABs[i+1]) return true;
  }

  return false;
}
void day7(Day *day) {
  str line, lines = day->input;
  long abba = 0, ssl = 0;
  while (str_each_line(&lines, &line)) {
    if (abba_enabled(line))
      abba++;
    if (ssl_enabled(line))
      ssl++;
  }
  PART1(abba);
  PART2(ssl);
}
