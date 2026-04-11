#ifndef CACHE_SIMULATION_CACHE
#define CACHE_SIMULATION_CACHE
#include "Cache.h"
#include "Debug.h"
#include <stdbool.h>

uint8_t (*ReplacementPolicy)(CacheMemory *CacheMemory,
                             uint8_t set) = RandomReplacement;
void (*UpdateReplacementPolicy)(CacheMemory *CacheMemory, uint8_t set,
                                uint8_t way) = EmptyReplacementUpdate;

static uint8_t SimpleLog2(uint8_t number) {
  uint8_t answer;
  for (answer = 0; (1 << answer) < number; answer++) {
  }

  ifnot((1 << answer) == answer)
      ErrorMessage("[Cache] Expect number of Set is a power of 2.");

  return answer;
}

static AddressParts DivideAddress(CacheMemory *cacheMemory,
                                  AddressType address) {
  AddressParts addressParts;
  addressParts.block = address >> OFFSET_BITS;

  addressParts.offset = address & ((1 << OFFSET_BITS) - 1);
  addressParts.set = addressParts.block & ((1 << cacheMemory->setBits) - 1);
  addressParts.tag = addressParts.block >> cacheMemory->setBits;

  return addressParts;
}

static void CheckedAddress(AddressParts *addressParts, int size) {
  if (addressParts->offset + size >= CACHE_LINE_DATA_SIZE)
    ErrorMessage("[Cache] Unable to handle cross cache line data.");
}

static inline CacheLine *GetCacheLine(CacheMemory *cacheMemory, uint8_t set,
                                      uint8_t way) {
  return cacheMemory->cacheLines + set * cacheMemory->numberOfWays + way;
}

static void ReplaceCacheLine(CacheMemory *cacheMemory,
                             AddressParts *addressParts,
                             CacheLine *cacheLineVictim) {
  AddressType startChunk = addressParts->block << OFFSET_BITS;
  ReadFromMainMemory(cacheMemory->mainMemory, startChunk, cacheLineVictim,
                     CACHE_LINE_DATA_SIZE);

  cacheLineVictim->valid = true;
  cacheLineVictim->policyVariable = 0;
  cacheLineVictim->tag = addressParts->tag;
}

void InitCacheMemory(CacheMemory *cacheMemory, MainMemory *mainMemory,
                     uint8_t numberOfSets, uint8_t numberOfWays) {
  // Init
  int numberOfCells = numberOfSets * numberOfWays;

  cacheMemory->mainMemory = mainMemory;
  cacheMemory->cacheMemoryCapacity = numberOfCells * CACHE_LINE_DATA_SIZE;

  // Malloc & Check
  cacheMemory->cacheLines = malloc(numberOfCells * sizeof(CacheLine));
  cacheMemory->dataCells = malloc(cacheMemory->cacheMemoryCapacity);
  cacheMemory->setReplacePolicy = malloc(numberOfSets * sizeof(int));

  if (!cacheMemory->cacheLines || !cacheMemory->dataCells ||
      !cacheMemory->setReplacePolicy)
    ErrorMessage("[Cache] Memory allocation failed.");

  // Set up all Cache Lines
  for (int i = 0; i < numberOfCells; i++) {
    cacheMemory->cacheLines[i].valid = false;
    cacheMemory->cacheLines[i].data =
        cacheMemory->dataCells + (i * CACHE_LINE_DATA_SIZE);
  }

  // Set up Replacement Policy's Variable
  for (int i = 0; i < numberOfSets; i++) {
    cacheMemory->setReplacePolicy[i] = 0;
  }

  // Constants & Stats
  cacheMemory->setBits = SimpleLog2(numberOfSets);
  cacheMemory->numberOfSets = numberOfSets;
  cacheMemory->numberOfWays = numberOfWays;
  cacheMemory->cacheHit = 0;
  cacheMemory->cacheMiss = 0;
}

void FreeCacheMemory(CacheMemory *cacheMemory) {
  free(cacheMemory->cacheLines);
  free(cacheMemory->dataCells);
  free(cacheMemory->setReplacePolicy);
}

CacheLine *LookupAndUpdateSet(CacheMemory *cacheMemory,
                              AddressParts *addressParts,
                              AddressType mainMemoryAddress) {
  for (uint8_t way = 0; way < cacheMemory->numberOfWays; way++) {
    CacheLine *checkedCacheLine =
        GetCacheLine(cacheMemory, addressParts->set, way);

    // Reach the end of the Set => Cache MISS
    // However this situation doesnt need Replacement Algorithm
    ifnot(checkedCacheLine->valid) {
      ReplaceCacheLine(cacheMemory, addressParts, checkedCacheLine);
      return checkedCacheLine;
    }

    // Cache HIT
    if (addressParts->tag == checkedCacheLine->tag)
      return checkedCacheLine;
  }

  // Cache MISS - Write-Back Policy
  uint8_t victim = ReplacementPolicy(cacheMemory, addressParts->set);
  CacheLine *cacheLineVictim =
      GetCacheLine(cacheMemory, addressParts->set, victim);
  ReplaceCacheLine(cacheMemory, addressParts, cacheLineVictim);
}

void ReadFromCache(CacheMemory *cacheMemory, AddressType mainMemoryAddress,
                   void *dest, int destSize) {
  AddressParts addressParts = DivideAddress(cacheMemory, mainMemoryAddress);
  CheckedAddress(&addressParts, destSize);
  CacheLine *target =
      LookupAndUpdateSet(cacheMemory, &addressParts, mainMemoryAddress);

  memcpy(dest, target, destSize);
  target->dirty = false;
}

void WriteToCache(CacheMemory *cacheMemory, AddressType mainMemoryAddress,
                  void *dest, int destSize) {
  AddressParts addressParts = DivideAddress(cacheMemory, mainMemoryAddress);
  CheckedAddress(&addressParts, destSize);
  CacheLine *target =
      LookupAndUpdateSet(cacheMemory, &addressParts, mainMemoryAddress);

  memcpy(target, dest, destSize);
  target->dirty = true;
}

#endif
