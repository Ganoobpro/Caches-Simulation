#pragma once

#include "Setup.h"
#include "MainMemory.h"
#include "ReplacementPolicies.h"

// Cache Mapping: N-way Set Associative Mapping
#define CACHE_LINE_DATA_SIZE 64
#define OFFSET_BITS 6

typedef struct
{
  byte dataCells[CACHE_LINE_DATA_SIZE];
  uint32_t tag;
  uint8_t other;
  bool valid;
  bool dirty;
}
CacheLine;

typedef struct
{
  MainMemory* mainMemory;
  CacheLine* cacheLines;

  int cacheMemoryCapacity;
  uint8_t numberOfSets;
  uint8_t numberOfWays;
  uint8_t setBits;

  // For replacement policies


  // For statistic purpose
  int cacheHit, cacheMiss;
}
CacheMemory;

typedef struct
{
  uint32_t tag;
  uint8_t  setIndex;
  uint8_t  offset;
}
AddressParts;
