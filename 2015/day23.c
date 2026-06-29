#include "../aoc.h"

typedef enum InsType { HLF, TPL, INC, JMP, JIE, JIO } InsType;

typedef struct Ins {
  InsType type;
  uint8_t reg; // 0 = a, 1 = b
  int val; // number value
} Ins;

Ins code[64] = {0};
int last_ins = 0;

int reg(str in) {
  if(in.len == 1 && in.data[0]=='a') return 0;
  else if(in.len == 1 && in.data[0] == 'b') return 1;
  else {
    printf("bad register: %.*s\n", (int)in.len, in.data);
    exit(1);
  }
}

void run(int *reg) {
  int pc = 0;
  while (pc >= 0 && pc < last_ins) {
    Ins c = code[pc];
    switch (c.type) {
    case HLF: {
      reg[c.reg] /= 2;
      pc++;
      break;
    }
    case TPL: {
      reg[c.reg] *= 3;
      pc++;
      break;
    }
    case INC: {
      reg[c.reg]++;
      pc++;
      break;
    }
    case JMP: {
      pc += c.val;
      break;
    }
    case JIE: {
      pc += (reg[c.reg] % 2 == 0) ? c.val : 1;
      break;
    }
    case JIO: {
      pc += (reg[c.reg] == 1) ? c.val : 1;
      break;
    }
    }
  }
}
void parse(str lines) {
  str line;
  while(str_each_line(&lines, &line)) {
    int i = last_ins++;
    str ins, args;
    if(!str_scan("%s %s", line, &ins, &args)) goto badline;
    if(str_eq(ins, str_constant("hlf"))) {
      code[i].type = HLF;
      code[i].reg = reg(args);
    } else if(str_eq(ins, str_constant("tpl"))) {
      code[i].type = TPL;
      code[i].reg = reg(args);
    } else if(str_eq(ins, str_constant("inc"))) {
      code[i].type = INC;
      code[i].reg = reg(args);
    } else if(str_eq(ins, str_constant("jmp"))) {
      code[i].type = JMP;
      code[i].val = (int)str_to_long(args);
    } else if(str_eq(ins, str_constant("jie"))) {
      str rarg, narg;
      if(!str_splitat(args, ",", &rarg, &narg)) goto badline;
      code[i].type = JIE;
      code[i].reg = reg(rarg);
      code[i].val = (int)str_to_long(str_trim(narg));
    } else if(str_eq(ins, str_constant("jio"))) {
      str rarg, narg;
      if(!str_splitat(args, ",", &rarg, &narg)) goto badline;
      code[i].type = JIO;
      code[i].reg = reg(rarg);
      code[i].val = (int)str_to_long(str_trim(narg));
    } else {
      goto badline;
    }
  }

  return;
 badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}

void day23(Day *day) {
  parse(day->input);
  int reg[2] = {0, 0};
  run(reg);
  PART1(reg[1]);
  reg[0] = 1;
  reg[1] = 0;
  run(reg);
  PART2(reg[1]);
}
