#include "aoc.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint64_t u64;

// max input and output (observed from real input data)
#define MAX_OUT 21
#define MAX_IN 13

typedef struct device {
  u16 id; // id starting at 1 index for checking in bitsets
  str name;
  u16 out[MAX_OUT]; // outpus (0 = unused)
  u8 outs;          // actual output count
  u16 in[MAX_IN];   // inputs (0 = unused)
  u8 ins;           // actual input count
} device;

u16 device_id(device *ds, str name) {
  for (int i = 1; i < arrlen(ds); i++) {
    if(str_eq(ds[i].name, name)) return ds[i].id;
  }
  return 0; // unknown
}

int has_out(device d, u16 out) {
  for (int i = 0; i < d.outs; i++) {
    if (d.out[i] == out)
      return 1;
  }
  return 0;
}

/* Parse devices, returns devices, sets you and out ids */
device *parse_devices(str orig_input, u16 *you_id, u16 *out_id, u16 *svr_id, u16 *fft_id, u16 *dac_id) {
  device *ds = {0};
  device dummy = (device){0};
  device out = {.name = str_from_cstr("out"), .id = 1};
  arrput(ds, dummy); // 0th index is dummy, unused
  arrput(ds, out);   // 1 is out
  *out_id = 1;


  // first pass, create all devices, so we can refer to them by id
  str input = orig_input;
  str line;
  while (str_splitat(input, "\n", &line, &input)) {
    str name, outputs;
    str_splitat(line, ":", &name, &outputs);
    device d = (device) {
      .id = arrlen(ds),
      .name = name};
    arrput(ds, d);
    //printf("got device id=%d, name=%.*s\n", d.id, (int)d.name.len, d.name.data);
    if (str_eq_cstr(name, "you"))
      *you_id = d.id;
    else if (str_eq_cstr(name, "dac"))
      *dac_id = d.id;
    else if (str_eq_cstr(name, "fft"))
      *fft_id = d.id;
    else if (str_eq_cstr(name, "svr"))
      *svr_id = d.id;
  }

  // second pass, add references to outputs
  input = orig_input;
  while (str_splitat(input, "\n", &line, &input)) {
    str name, outputs;
    str_splitat(line, ":", &name, &outputs);
    int d_id = device_id(ds, name);
    str out;
    outputs = str_trim(outputs);
    int o=0;
    while (str_splitat(outputs, " ", &out, &outputs)) {
      u16 out_id = device_id(ds, out);
      if (out_id) {
        //printf("device %.*s has output %.*s\n", (int)ds[d_id].name.len, ds[d_id].name.data,
        //       (int)out.len, out.data);
        ds[d_id].out[o++] = out_id;
      }
    }
    if (outputs.len) {
      u16 out_id = device_id(ds, outputs);
      if (out_id) {
        //printf("device %.*s has output %.*s\n", (int)ds[d_id].name.len, ds[d_id].name.data,
        //       (int)outputs.len, outputs.data);
        ds[d_id].out[o++] = out_id;
      }
    }
    ds[d_id].outs = o;
  }

  // build inputs
  for (int i = 1; i < arrlen(ds); i++) {
    int ins = 0;
    for (int j = 1; j < arrlen(ds); j++) {
      if (i == j)
        break;
      if (has_out(ds[j], i)) {
        ds[i].in[ins++] = j;
        ds[i].ins = ins;
      }
    }
  }

  return ds;
}


typedef struct nodecount {
  bool calculated;
  long count;
} nodecount;

bool is_ignore(u8 n_ignore, u16 *ignore, u16 val) {
  for (int i = 0; i < n_ignore; i++) {
    if (ignore[i] == val) {
      return true;
    }
  }
  return false;
}

long traverse(nodecount *node_count, device *ds, u16 at, u16 target, u8 n_ignore, u16 *ignore) {
  // printf(" at %d, target %d\n", at, target);
  if (node_count[at].calculated) {
    return node_count[at].count;
  } else {
    if (at == target) {
      node_count[at] = (nodecount){.calculated = 1, .count = 1};
      return 1;
    } else {
      long count = 0;
      for (int i = 0; i < MAX_OUT; i++) {
        if (!ds[at].out[i] || is_ignore(n_ignore, ignore, ds[at].out[i]))
          continue;
        count += traverse(node_count, ds, ds[at].out[i], target, n_ignore, ignore);
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
  device *ds = parse_devices(input, &you, &out, &svr, &fft, &dac);
  printf("you = %d, dac = %d, fft= %d, svr=%d, max_id = %ld\n", you, dac, fft, svr, arrlen(ds));

  nodecount *node_count = malloc(arrlen(ds) * sizeof(nodecount));
  memset(node_count, 0, arrlen(ds) * sizeof(nodecount));
  long paths1 = traverse(node_count, ds, you, out, 0, NULL);
  printf("Part1: %ld\n", paths1);

  // paths from svr->dac * dac->fft * fft->out
  //   +        svr->fft * ffr->dac * dac->out

  memset(node_count, 0, arrlen(ds) * sizeof(nodecount));
  long svr_dac = traverse(node_count, ds, svr, dac, 2, (u16[]){fft,out});

  memset(node_count, 0, arrlen(ds) * sizeof(nodecount));
  long dac_fft = traverse(node_count, ds, dac, fft, 2, (u16[]){svr,out});

  memset(node_count, 0, arrlen(ds) * sizeof(nodecount));
  long fft_out = traverse(node_count, ds, fft, out, 2, (u16[]){dac,svr});

  memset(node_count, 0, arrlen(ds) * sizeof(nodecount));
  long svr_fft = traverse(node_count, ds, svr, fft, 2, (u16[]){dac,out});

  memset(node_count, 0, arrlen(ds) * sizeof(nodecount));
  long fft_dac = traverse(node_count, ds, fft, dac, 2, (u16[]){svr,out});

  memset(node_count, 0, arrlen(ds) * sizeof(nodecount));
  long dac_out = traverse(node_count, ds, dac, out, 2, (u16[]){svr,fft});


  printf("Part2: %ld\n",
         (svr_dac * dac_fft * fft_out) + (svr_fft * fft_dac * dac_out));

  diagram(ds);

}
