#include "cachelab.h"
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int s, E, b;
int S, B;
int hit_count = 0, miss_count = 0, eviction_count = 0;
int time_count = 0;
struct line{
  bool validate;
  int time;
  int tag;
};
typedef struct line *set;
set *cache;
FILE *file = NULL;
void phase_argv(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
    switch (opt) {
    case 'h':
      printf("Usage: %s [-h] [-v] -s <s> -E <E> -b <b> -t <tracefile>\n",
             argv[0]);
      exit(0);
    case 'v':
      break;
    case 's':
      s = atoi(optarg);
      break;
    case 'E':
      E = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    case 't':
      file = fopen(optarg, "r");
      if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", optarg);
        exit(1);
      }
      break;
    default:
      fprintf(stderr,
              "Usage: %s [-h] [-v] -s <s> -E <E> -b <b> -t <tracefile>\n",
              argv[0]);
      exit(1);
    }
    S = 1 << s;
    B = 1 << b;
  }
}
void init_cache() {
  cache = (set *)malloc(sizeof(set) * S);
  for (int i = 0; i < S; ++i) {
    cache[i] = (set)malloc(sizeof(struct line) * E);
    for (int j = 0; j < E; ++j) {
      cache[i][j].validate = false;
      cache[i][j].tag = -1;
      cache[i][j].time = -1;
    }
  }
}
void free_cache() {
  for (int i = 0; i < S; ++i) {
    free(cache[i]);
  }
  free(cache);
}
int find_least_use(set s){
  int index = 0;
  for (int i = 1; i < E; i++)
    index = s[index].time > s[i].time ? i : index;
  return index; 
}
void process_line(unsigned long addr) {
  ++time_count;
  unsigned long block_addr = addr >> b;
  unsigned long set_addr = block_addr & ((1 << s) - 1);
  unsigned long tag = block_addr >> s;
  int hit_idx = -1;
  set cur_set = cache[set_addr];
  for (int i = 0; i < E; ++i) {
    if (cur_set[i].validate && cur_set[i].tag == tag) {
      hit_idx = i;
      cur_set[i].time = time_count;
      ++hit_count;
      break;
    }
  }
  if (hit_idx == -1) {
    ++miss_count;
    bool all_loaded = true;
    int empty_idx = -1;
    for (int i = 0; i < E; i++)
      if(!(cur_set[i].validate)){
        empty_idx = i;
        all_loaded = false;
      }
    if (!all_loaded) {
      cur_set[empty_idx].tag = tag;
      cur_set[empty_idx].time = time_count;
      cur_set[empty_idx].validate = true;
    }else {
      int idx = find_least_use(cur_set);
      cur_set[idx].tag = tag;
      cur_set[idx].time = time_count;
      ++eviction_count;
    }
  }
}
void process_file() {
  char line[256];
  char op;
  unsigned long addr;
  int size;
  while (fgets(line, sizeof(line), file)) {
    if (line[0] == 'I')
      continue;
    sscanf(line, " %c %lx,%d", &op, &addr, &size);
    switch (op) {
    case 'L':
    case 'S':
      process_line(addr);
      break;
    case 'M':
      process_line(addr);
      process_line(addr);
      break;
    default:
      break;
    }
  }
}

int main(int argc, char **argv) {
  phase_argv(argc, argv);
  init_cache();
  process_file();
  free_cache();
  printSummary(hit_count, miss_count, eviction_count);
  fclose(file);
  return 0;
}
