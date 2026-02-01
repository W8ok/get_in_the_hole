// maps.c
#include "include/types.h"
#include "include/defines.h"

static const uint8_t default_map[5*5] = {
  1, 1, 0, 1, 1,
  1, 0, 0, 0, 1,
  0, 0, 0, 0, 0,
  1, 0, 0, 0, 1,
  1, 1, 0, 1, 1,
};

const Map maps[MAP_COUNT] = {
  [DEFAULT] = {
    .w = 5,
    .h = 5,
    .tiles = default_map
  }
};
