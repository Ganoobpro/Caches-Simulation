#ifndef CACHE_SIMULATION_REPLACEMENT_POLICIES
#define CACHE_SIMULATION_REPLACEMENT_POLICIES
#include "ReplacementPolicies.h"

uint8_t
RandomReplacement(CacheMemory* cacheMemory,
                  uint8_t set)
{
  srand(time(NULL));
  return (rand() * rand()) & (cacheMemory->numberOfWays - 1);
}

uint8_t
FIFO_Replacement(CacheMemory* cacheMemory,
                 uint8_t set)
{
  uint8_t victim = cacheMemory->setOther[set]++;
  cacheMemory->setOther[set] &= (cacheMemory->numberOfSets - 1); // Expect: Number Of Set is power of 2

  return victim;
}

uint8_t
TreeBasedPseudoLRU(CacheMemory* cacheMemory)
{

  return 0;
}



void EmptyReplacementUpdate(CacheMemory* cacheMemory,
                            uint8_t set, uint8_t way) {}

#endif /* ifdef CACHE_SIMULATION_REPLACEMNT_POLICIES */
