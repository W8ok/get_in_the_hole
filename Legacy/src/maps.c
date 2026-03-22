// maps.c
#include "include/types.h"
#include "include/defines.h"

static const uint8_t tutorial_map[5*5] = {
  1, 1, 1, 1, 1,
  1, 0, 0, 0, 1,
  1, 0, 0, 0, 1,
  1, 0, 0, 0, 1,
  1, 1, 1, 1, 1,
};

static const uint8_t default_map[7*7] = {
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 0, 1, 1, 1,
  1, 1, 0, 0, 0, 1, 1,
  1, 0, 0, 0, 0, 0, 1,
  1, 1, 0, 0, 0, 1, 1,
  1, 1, 1, 0, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
};

static const uint8_t circling_map[9*9] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 1, 0, 0, 0, 1,
  1, 0, 1, 0, 0, 0, 1, 0, 1,
  1, 0, 0, 0, 1, 0, 0, 0, 1,
  1, 1, 0, 1, 1, 1, 0, 1, 1,
  1, 0, 0, 0, 1, 0, 0, 0, 1,
  1, 0, 1, 0, 0, 0, 1, 0, 1,
  1, 0, 0, 0, 1, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1,
};

const Map maps[MAP_COUNT] = {
  [TUTORIAL] = {
    .w = 5,
    .h = 5,
    .tiles = tutorial_map
  },
  [DEFAULT] = {
    .w = 7,
    .h = 7,
    .tiles = default_map
  },
  [CIRCLING] = {
    .w = 9,
    .h = 9,
    .tiles = circling_map
  }
};

