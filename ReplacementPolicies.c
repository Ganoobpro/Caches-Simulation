#ifndef CACHE_SIMULATION_REPLACEMENT_POLICIES
#define CACHE_SIMULATION_REPLACEMENT_POLICIES
#include "ReplacementPolicies.h"

uint8_t
RandomReplacement(CacheMemory* cacheMemory)
{
  srand(time(NULL));
  return (rand() * rand()) & (cacheMemory->numberOfWays - 1);
}

/*
uint8_t
FIFO_Replacement(CacheMemory* cacheMemory)
{

}
*/

#endif /* ifdef CACHE_SIMULATION_REPLACEMNT_POLICIES */
