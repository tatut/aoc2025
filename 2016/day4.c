#include "../aoc.h"

int compare_by_count(void *countsptr, const void *aptr, const void *bptr) {
  int *counts = (int *)countsptr;
  char a = *((char *)aptr);
  char b = *((char *)bptr);
  int d = (counts[b - 'a'] - counts[a - 'a']);
  return d ? d : a-b;
}

bool valid(str check, int *counts) {
  char letters[26];
  for(int i='a';i<='z';i++) letters[i-'a']=i;
  qsort_r(letters, 26, 1, counts, compare_by_count);
  return (strncmp(letters, check.data, 5)==0);
}

void decrypt(str in, long id) {
  while (!is_digit(in.data[0])) {
    char ch = in.data[0];
    if (ch == '-')
      ch = ' ';
    else
      ch = 'a' + ((ch-'a')+id)%26;
    in.data[0] = ch;
    in = str_drop(in, 1);
  }
}
void day4(Day *day){
  str line, lines = day->input;
  long sum = 0;
  while(str_each_line(&lines, &line)) {
    int counts[26] = {0};
    str orig = line;
    while(!is_digit(line.data[0])) {
      char ch = line.data[0];
      line = str_drop(line, 1);
      if(ch >= 'a' && ch <= 'z') {
        counts[ch-'a']++;
      }
    }
    long id = str_to_long_rest(line, &line);
    str check;
    if (!str_scan("[%s]", line, &check))
      goto badline;
    if (valid(check, counts)) {
      sum += id;
      decrypt(orig, id); // decrypt in place
      if (str_startswith(orig, str_constant("northpole")))
        PART2(id);
    }
  }

  PART1(sum);
  return;
 badline:
  printf("Bad line: %.*s\n", (int)line.len, line.data);
}
