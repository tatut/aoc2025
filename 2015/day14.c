#include "../aoc.h"

typedef struct Reindeer {
  str name;
  long speed;
  long duration;
  long rest;
} Reindeer;

#define MAX_REINDEER 12
Reindeer reindeer[MAX_REINDEER];
int last_reindeer = 0;

void parse(str lines) {
  str line;
  while (str_each_line(&lines, &line)) {
    Reindeer r = {0};
    if(!str_scan("%s can fly %ld km/s for %ld seconds, but then must rest for %ld seconds.", line, &r.name, &r.speed, &r.duration, &r.rest))
      goto badline;
    reindeer[last_reindeer++] = r;
    //printf("%.*s, speed: %ld, dur: %ld, rest: %ld\n", (int)r.name.len, r.name.data, r.speed, r.duration, r.rest);
  }

  return;
badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}

long distance(Reindeer r, long seconds) {
  // calculate how many full travel+rest periods there are
  long travel_and_rest = r.duration + r.rest;
  long full_periods = seconds / travel_and_rest;
  long spare_seconds = seconds % travel_and_rest;

  long dist = full_periods * r.duration * r.speed;
  // add the last period travel seconds
  if (spare_seconds >= r.duration)
    dist += r.duration * r.speed;
  else
    dist += spare_seconds * r.speed;
  return dist;
}

void day14(Day *day) {
  parse(day->input);

  long max = -1;
  for (int i = 0; i < last_reindeer; i++) {
    long d = distance(reindeer[i], 2503);
    if(d > max) max = d;
  }
  PART1(max);

  long points[MAX_REINDEER] = {0};
  long dist[MAX_REINDEER] = {0};
  for (int s = 1; s <= 2503; s++) {
    max = -1;
    for (int i = 0; i < last_reindeer; i++) {
      dist[i] = distance(reindeer[i], s);
      if(dist[i] > max) max = dist[i];
    }

    // give points to everyone in the lead
    for (int i = 0; i < last_reindeer; i++) {
      if (dist[i] == max)
        points[i]++;
    }
  }
  max = -1;
  for (int i = 0; i < last_reindeer; i++) {
    if(points[i] > max) max = points[i];
  }
  PART2(max);
}
