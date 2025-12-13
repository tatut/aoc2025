#include "aoc.h"

// max input and output (observed from real input data)
#define MAX_OUT 21

typedef struct device {
  str name;
  u16 out[MAX_OUT]; // outpus (0 = unused)
  u8 outs;          // actual output count
} device;

// id is a-z * 3 (so 25^3, 15bit id is enough)
// have array of devices statically
device ds[2<<15];

// turn C string into id
u16 ID(const char *x) {
  return (((x[0]-'a') << 10) + ((x[1]-'a') << 5) + (x[2]-'a'));
}


int has_out(device d, u16 out) {
  for (int i = 0; i < d.outs; i++) {
    if (d.out[i] == out)
      return 1;
  }
  return 0;
}

/* Parse devices, returns devices, sets you and out ids */
void parse_devices(str orig_input) {
  device out = {.name = str_from_cstr("out")};
  ds[ID("out")] = out;

  // first pass, create all devices, so we can refer to them by id
  str input = orig_input;
  str line;
  while (str_splitat(input, "\n", &line, &input)) {
    str name, outputs;
    str_splitat(line, ":", &name, &outputs);
    device d = (device) {.name = name};
    ds[ID(name.data)] = d;
  }

  // second pass, add references to outputs
  input = orig_input;
  while (str_splitat(input, "\n", &line, &input)) {
    str name, outputs;
    str_splitat(line, ":", &name, &outputs);
    u16 d_id = ID(name.data);
    str out;
    outputs = str_trim(outputs);
    int o=0;
    while (str_splitat(outputs, " ", &out, &outputs)) {
      u16 out_id = ID(out.data);
      ds[d_id].out[o++] = out_id;
    }
    if (outputs.len) {
      u16 out_id = ID(outputs.data);
      ds[d_id].out[o++] = out_id;
    }
    ds[d_id].outs = o;
  }
}


typedef struct nodecount {
  bool calculated;
  long count;
} nodecount;

nodecount node_count[2<<15];

void clear() { memset(node_count, 0, sizeof(node_count)); }

long traverse(u16 at, u16 target) {
  if (node_count[at].calculated) {
    return node_count[at].count;
  } else {
    if (at == target) {
      node_count[at] = (nodecount){.calculated = 1, .count = 1};
      return 1;
    } else {
      long count = 0;
      for (int i = 0; i < MAX_OUT; i++) {
        if (!ds[at].out[i])
          continue;
        count += traverse(ds[at].out[i], target);
      }
      node_count[at] = (nodecount){.calculated = 1, .count = count};
      return count;
    }
  }
}


void diagram(device *ds) {
  FILE *f = fopen("day11.dot", "w");
  if (!f) {
    fprintf(stderr, "Can't open day11.dot\n");
    exit(1);
  }
  fprintf(f, "digraph {\n");
  for (int i = 1; i < arrlen(ds); i++) {
    str from = ds[i].name;
    for (int j = 0; j < MAX_OUT; j++) {
      u16 out = ds[i].out[j];
      if (out) {
        fprintf(f, "%.*s -> %.*s\n", (int)from.len, from.data,
                (int)ds[out].name.len, ds[out].name.data);
      }
    }
  }
  fprintf(f, "}");
  fclose(f);
}

void day11(str input) {
  u16 you, out, svr, dac, fft;
  parse_devices(input);

  clear();
  long paths1 = traverse(ID("you"), ID("out"));
  printf("Part1: %ld\n", paths1);

  // paths from svr->dac * dac->fft * fft->out
  //   +        svr->fft * ffr->dac * dac->out

  clear();
  long svr_dac = traverse(ID("svr"), ID("dac"));

  clear();
  long dac_fft = traverse(ID("dac"), ID("fft"));

  clear();
  long fft_out = traverse(ID("fft"), ID("out"));

  clear();
  long svr_fft = traverse(ID("svr"), ID("fft"));

  clear();
  long fft_dac = traverse(ID("fft"), ID("dac"));

  clear();
  long dac_out = traverse(ID("dac"), ID("out"));

  printf("Part2: %ld\n",
         (svr_dac * dac_fft * fft_out) + (svr_fft * fft_dac * dac_out));

  //diagram(ds);

}
