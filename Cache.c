#ifndef CACHE_SIMULATION_CACHE
#define CACHE_SIMULATION_CACHE
#include "Cache.h"
#include "MainMemory.h"

uint8_t (*ReplacementPolicy)(CacheMemory *CacheMemory,
                             uint8_t set) = RandomReplacement;
void (*UpdateReplacementPolicy)(CacheMemory *CacheMemory, uint8_t set,
                                uint8_t recentAccessWay) =
    EmptyReplacementUpdate;

static uint8_t SimpleLog2(uint8_t number) {
  uint8_t answer;
  for (answer = 0; (1 << answer) < number; answer++) {
  }

  ifnot((1 << answer) == number)
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
  cacheMemory->setReplacePolicy = malloc(numberOfSets * sizeof(uint64_t));

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
  cacheMemory->wayBits = SimpleLog2(numberOfWays);
  cacheMemory->numberOfSets = numberOfSets;
  cacheMemory->numberOfWays = numberOfWays;
  cacheMemory->cacheHit = 0;
  cacheMemory->cacheMiss = 0;
}

CacheLine *GetCacheLine(CacheMemory *cacheMemory, uint8_t set, uint8_t way) {
  return cacheMemory->cacheLines + set * cacheMemory->numberOfWays + way;
}

void FreeCacheMemory(CacheMemory *cacheMemory) {
  free(cacheMemory->cacheLines);
  free(cacheMemory->dataCells);
  free(cacheMemory->setReplacePolicy);
}

uint8_t LookupAndUpdateSet(CacheMemory *cacheMemory, AddressParts *addressParts,
                           AddressType mainMemoryAddress) {
  for (uint8_t way = 0; way < cacheMemory->numberOfWays; way++) {
    CacheLine *checkedCacheLine =
        GetCacheLine(cacheMemory, addressParts->set, way);

    // Reach the end of the Set => Cache MISS
    // However this situation doesnt need Replacement Algorithm
    ifnot(checkedCacheLine->valid) {
      ReplaceCacheLine(cacheMemory, addressParts, checkedCacheLine);
      return way;
    }

    // Cache HIT
    if (addressParts->tag == checkedCacheLine->tag)
      return way;
  }

  // Cache MISS - Write-Back Policy
  uint8_t victim = ReplacementPolicy(cacheMemory, addressParts->set);

  // SPECIAL: Some algorithms in some case don't choose a victim
  if (victim == 0xFF) {
    return 0xFF;
  }

  CacheLine *cacheLineVictim =
      GetCacheLine(cacheMemory, addressParts->set, victim);
  ReplaceCacheLine(cacheMemory, addressParts, cacheLineVictim);

  return victim;
}

static byte *GetMemoryTarget(CacheMemory *cacheMemory,
                             AddressType mainMemoryAddress, void *dest,
                             int destSize, bool dirty) {
  AddressParts addressParts = DivideAddress(cacheMemory, mainMemoryAddress);
  CheckedAddress(&addressParts, destSize);
  uint8_t victim =
      LookupAndUpdateSet(cacheMemory, &addressParts, mainMemoryAddress);

  if (victim == 0xFF) {
    return AccessMainMemory(cacheMemory->mainMemory, mainMemoryAddress);
  }

  UpdateReplacementPolicy(cacheMemory, addressParts.set, victim);
  CacheLine *target = GetCacheLine(cacheMemory, addressParts.set, victim);
  target->dirty = dirty;
  return target->data + addressParts.offset;
}

void ReadFromCache(CacheMemory *cacheMemory, AddressType mainMemoryAddress,
                   void *dest, int destSize) {
  byte *target =
      GetMemoryTarget(cacheMemory, mainMemoryAddress, dest, destSize, false);
  memcpy(dest, target, destSize);
}

void WriteToCache(CacheMemory *cacheMemory, AddressType mainMemoryAddress,
                  void *dest, int destSize) {
  byte *target =
      GetMemoryTarget(cacheMemory, mainMemoryAddress, dest, destSize, true);
  memcpy(dest, target, destSize);
}

#endif
