#include "../aoc.h"

#include "../vendor/md5-c/md5.h"
#include "../vendor/md5-c/md5.c"

char hex(uint8_t d) {
  if (d < 10)
    return '0' + d;
  else
    return 'a' + (d-10);
}

void day5(Day *day) {
  char buf[64];
  str id = str_trim(day->input);
  size_t pos = snprintf(buf, 64, "%.*s", (int)id.len, id.data);
  int num = 0;

  char pw[8];
  int pw_idx = 0;

  char pw2[8] = {0};
  int pw2_set = 0;

  for (;;) {
    uint8_t result[16];
    snprintf(&buf[pos], 64 - pos, "%d", num++);
    md5String(buf, result);
    if(result[0] != 0 || result[1] != 0 || result[2] > 16) continue; // 5 first chars
    char six, seven;
    six = hex(result[2]);
    seven = hex(result[3] >> 4);

    if(pw_idx < 8)
      pw[pw_idx++] = six;

    if (six >= '0' && six <= '9') {
        int pidx = six - '0';
        if (pw2[pidx] == 0) {
          pw2[pidx] = seven;
          pw2_set++;
        }
    }
    if (pw_idx == 8 && pw2_set == 8)
      break;


  }
  set_str_answer(&day->part1, (str){.data = pw, .len = 8});
  set_str_answer(&day->part2, (str){.data = pw2, .len = 8});



}
