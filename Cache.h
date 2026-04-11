#pragma once

#include "Debug.h"
#include "MainMemory.h"
#include "Setup.h"

// Cache Mapping: N-way Set Associative Mapping
#define CACHE_LINE_DATA_SIZE 64
#define VISIBLE_CACHE_ROWS 16
#define VISIBLE_CACHE_COLS 8
#define OFFSET_BITS 6

typedef struct {
  byte *data;
  uint32_t tag;
  bool valid;
  bool dirty;

  // For replacement policies
  byte policyVariable;
} CacheLine;

typedef struct {
  MainMemory *mainMemory;
  CacheLine *cacheLines;
  byte *dataCells;

  int cacheMemoryCapacity;
  uint8_t numberOfSets;
  uint8_t numberOfWays;
  uint8_t setBits;
  uint8_t wayBits;

  // For statistic purpose
  int cacheHit, cacheMiss;

  // For replacement policies
  uint64_t *setReplacePolicy;
} CacheMemory;

typedef struct {
  uint32_t block;
  uint32_t tag;
  uint8_t set;
  uint8_t offset;
} AddressParts;

void InitCacheMemory(CacheMemory *cacheMemory, MainMemory *mainMemory,
                     uint8_t numberOfSets, uint8_t numberOfWays);
void FreeCacheMemory(CacheMemory *cacheMemory);
CacheLine *LookupAndUpdateSet(CacheMemory *cacheMemory,
                              AddressParts *addressParts,
                              AddressType mainMemoryAddress);
void ReadFromCache(CacheMemory *cacheMemory, AddressType mainMemoryAddress,
                   void *dest, int destSize);
void WriteToCache(CacheMemory *cacheMemory, AddressType mainMemoryAddress,
                  void *dest, int destSize);

#include "ReplacementPolicies.h"
