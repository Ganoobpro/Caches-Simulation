#ifndef CACHE_SIMULATION_REPLACEMENT_POLICIES
#define CACHE_SIMULATION_REPLACEMENT_POLICIES
#include "ReplacementPolicies.h"

uint8_t
RandomReplacement(CacheMemory* cacheMemory)
{
  srand(time(NULL));
  return (rand() * rand()) & (cacheMemory->numberOfWays - 1);
}

uint8_t
FIFO_Replacement(CacheMemory* cacheMemory,
                 AddressParts addressParts)
{
  uint8_t victim = cacheMemory->setOther[addressParts->setIndex]++;
  cacheMemory->setOther[addressParts->setIndex] &= (cacheMemory->numberOfSets - 1); // Expect: Number Of Set is power of 2

  return victim;
}

uint8_t
TreeBasedPseudoLRU(CacheMemory* cacheMemory)
{

  return 0;
}

#endif /* ifdef CACHE_SIMULATION_REPLACEMNT_POLICIES */
