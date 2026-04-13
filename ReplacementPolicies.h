#pragma once

#include "Cache.h"
#include "Setup.h"

typedef union {
  struct {
    uint64_t uselessBits : 48;
    uint8_t recentUsedWay : 8;
    uint8_t leastUsedWay : 8;
  };
  uint64_t data;
} LMRU_SET_BITS;

typedef union {
  struct {
    uint8_t prevWay : 8;
    uint8_t nextWay : 8;
  };
  uint16_t data;
} LMRU_WAY_BITS;

uint8_t RandomReplacement(CacheMemory *cacheMemory, uint8_t set);
uint8_t FIFO_Replacement(CacheMemory *cacheMemory, uint8_t set);

void EmptyReplacementUpdate(CacheMemory *cacheMemory, uint8_t set, uint8_t way);
