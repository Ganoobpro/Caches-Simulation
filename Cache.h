#pragma once

#include "Setup.h"
#include "MainMemory.h"

// Cache Mapping: N-way Set Associative Mapping
#define CACHE_LINE_DATA_SIZE 64
#define VISIBLE_CACHE_ROWS 16
#define VISIBLE_CACHE_COLS 8
#define OFFSET_BITS 6

#define REPLACEMENT_POLICY RandomReplacement
#define UPDATE_REPLACEMENT_POLICY [](){} // Empty

typedef struct
{
  byte* data;
  uint32_t tag;
  bool valid;
  bool dirty;

  // For replacement policies
  byte other;
}
CacheLine;

typedef struct
{
  MainMemory* mainMemory;
  CacheLine* cacheLines;
  byte* dataCells;

  int cacheMemoryCapacity;
  uint8_t numberOfSets;
  uint8_t numberOfWays;
  uint8_t setBits;

  // For statistic purpose
  int cacheHit, cacheMiss;

  // For replacement policies
  int* setOther;
}
CacheMemory;

typedef struct
{
  uint32_t tag;
  uint8_t  set;
  uint8_t  offset;
}
AddressParts;

#include "ReplacementPolicies.h"

void InitCacheMemory(CacheMemory* cacheMemory, MainMemory* mainMemory,
                     uint8_t numberOfSets, uint8_t numberOfWays,
                     uint8_t setBits);
void FreeCacheMemory(CacheMemory* cacheMemory);
