#ifndef aoc_h
#define aoc_h

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint64_t u64;

typedef struct str {
  size_t len;
  char *data;
} str;

typedef enum AnswerType {
  NO_ANSWER = 0,
  LONG_ANSWER = 1,
  STR_ANSWER = 2
} AnswerType;

typedef struct Answer {
  AnswerType type;
  union {
    long long_answer;
    str str_answer;
  } answer;
} Answer;

typedef struct Day {
  int year;
  int day;
  str input;
  Answer part1;
  Answer part2;
  clock_t start;
  clock_t end;
} Day;

typedef enum dir4 {
  UP, RIGHT, DOWN, LEFT
} dir4;

void set_long_answer(Answer *a, long ans);
void set_str_answer(Answer *a, str ans);

#define ANSWER(part, x) _Generic((x), \
                                 long: set_long_answer(part, x),       \
                                 int: set_long_answer(part, (int)(x)))

#define PART1(x) ANSWER(&day->part1, (x))
#define PART2(x) ANSWER(&day->part2, (x))

#define str_constant(X) ((str){.len=sizeof(X)-1, .data=X})

bool str_splitat(str in, const char *chars, str *split, str *rest);
int str_indexof(str in, char ch);
bool str_each_line(str *lines, str *line);
bool is_space(char ch);
bool is_digit(char ch);
str str_ltrim(str in);
str str_rtrim(str in);
str str_trim(str in);
long str_to_long(str s);
long str_to_long_rest(str s, str *rest);
bool str_startswith(str haystack, str needle);
str str_drop(str haystack, size_t len);
str str_take(str big, size_t len);
str str_from_cstr(const char *in);
str str_dup(str in);
void str_reverse(str mut);
void str_free(str *s);
bool str_eq(str a, str b);
bool str_eq_cstr(str str, const char *cstring);
char str_char_at(str str, size_t idx);
char *str_to_cstr(str str);
str *str_lines(str in);
bool str_scan(const char *fmt, str in, ...);
#endif
