#ifndef aoc_h
#define aoc_h

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct str {
  size_t len;
  char *data;
} str;

#endif

#ifndef aoc_h_impl

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

bool str_splitat(str in, const char *chars, str *split, str *rest) {
  if(in.len == 0) return false;
  size_t at = 0;
  int chs = strlen(chars);
  while(at < in.len) {
    for(int i=0;i<chs;i++) {
      if(in.data[at] == chars[i]) {
        // found split here
        split->len = at;
        split->data = in.data;
        rest->len = in.len - at - 1;
        rest->data = in.data + at + 1;
        return true;
      }
    }
    at++;
  }
  return false;
}


int str_indexof(str in, char ch) {
  for(int i=0;i<in.len;i++) {
    if(in.data[i] == ch) return i;
  }
  return -1;
}

bool str_each_line(str *lines, str *line) {
  if(lines->len == 0) return false;
  if(str_splitat(*lines, "\n", line, lines))
    return true;
  if(lines->len) {
    // last line
    line->len = lines->len;
    line->data = lines->data;
    return true;
  }
  return false;
}

bool is_space(char ch) {
  return ch == ' ' || ch == '\t';
}

str str_ltrim(str in) {
  while(in.len && is_space(in.data[0])) {
    in.len--;
    in.data = &in.data[1];
  }
  return in;
}

str str_rtrim(str in) {
  while(in.len && is_space(in.data[in.len-1])) in.len--;
  return in;
}

str str_trim(str in) {
  return str_ltrim(str_rtrim(in));
}

long str_to_long(str s) {
  long l=0;
  for(size_t i=0;i<s.len;i++) {
    l = (l*10) + (s.data[i]-48);
  }
  return l;
}
bool str_startswith(str haystack, str needle) {
  if(needle.len > haystack.len) return false;
  return strncmp(haystack.data,needle.data,needle.len) == 0;
}

str str_drop(str haystack, size_t len) {
  if(len > haystack.len) return (str) {.len = 0, .data=NULL};
  return (str) {.len = haystack.len - len,
                .data = &haystack.data[len]};
}

str str_from_cstr(const char *in) {
  size_t len = strlen(in);
  char *data = malloc(len);
  if(!data) fprintf(stderr, "Malloc failed!");
  memcpy(data, in, len);
  return (str){.data = data, .len = len};
}

str str_dup(str in) {
  char *data = malloc(in.len);
  if(!data) fprintf(stderr, "Malloc failed!");
  memcpy(data, in.data, in.len);
  return (str) {.len = in.len, .data = data};
}

void str_reverse(str mut) {
  size_t l=0,r=mut.len-1;
  while(l<r) {
    char tmp = mut.data[l];
    mut.data[l] = mut.data[r];
    mut.data[r] = tmp;
    l++; r--;
  }
}

void str_free(str *s) {
  free(s->data);
  s->len = 0;
  s->data = NULL;
}

bool str_eq(str a, str b) {
  if(a.len != b.len) return false;
  return strncmp(a.data, b.data, a.len) == 0;
}

bool str_eq_cstr(str str, const char *cstring) {
  int len = strlen(cstring);
  if(len != str.len) return false;
  return strncmp(str.data, cstring, len) == 0;
}

char str_char_at(str str, size_t idx) {
  if(idx < 0 || idx >= str.len) return 0;
  return str.data[idx];
}

char *str_to_cstr(str str) {
  char *cstr = malloc(str.len+1);
  if(!cstr) {
    printf("malloc failed");
    exit(1);
  }
  memcpy(cstr, str.data, str.len); // FIXME: check
  cstr[str.len] = 0;
  return cstr;
}

/* Allocate new dynamic array of lines extracted */
str *str_lines(str in) {
  str *lines = {0};
  str line;
  str rest = in;
  while(str_splitat(rest, "\n", &line, &rest)) {
    arrput(lines, line);
  }
  if(rest.len > 0)
    arrput(lines, rest);
  return lines;
}
#endif
