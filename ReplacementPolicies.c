#ifndef CACHE_SIMULATION_REPLACEMENT_POLICIES
#define CACHE_SIMULATION_REPLACEMENT_POLICIES
#include "ReplacementPolicies.h"
#include <stdbool.h>
#include <stdint.h>

uint8_t RandomReplacement(CacheMemory *cacheMemory, uint8_t set) {
  srand(time(NULL));
  return (rand() * rand()) & (cacheMemory->numberOfWays - 1);
}

uint8_t FIFO_Replacement(CacheMemory *cacheMemory, uint8_t set) {
  uint8_t victim = cacheMemory->setReplacePolicy[set]++;
  cacheMemory->setReplacePolicy[set] &=
      (cacheMemory->numberOfSets - 1); // Expect: Number Of Set is power of 2

  return victim;
}

uint8_t TreeBasedPseudoLRU(CacheMemory *cacheMemory, uint8_t set) {
  uint64_t policyVariable = cacheMemory->setReplacePolicy[set];
  uint8_t victim = 0;

  for (uint8_t depthNodes = 1; depthNodes < cacheMemory->numberOfWays;
       depthNodes <<= 1) {
    uint64_t mask = 1 << victim;
    victim = (victim << 1) + (!!(policyVariable & mask));
    policyVariable >>= depthNodes;
  }

  return victim;
}

// Fuck this too hard to do
uint8_t CombinationLRU(CacheMemory *cacheMemory, uint8_t set) {
  uint8_t *setListOrder = malloc(cacheMemory->numberOfWays);

  free(setListOrder);
  return 0;
}

////////////////////////////// UPDATE FUNCTIONS //////////////////////////////
void EmptyReplacementUpdate(CacheMemory *cacheMemory, uint8_t set,
                            uint8_t recentAccessWay) {}

void TreeBasedPseudoLRU_Update(CacheMemory *cacheMemory, uint8_t set,
                               uint8_t recentAccessWay) {
  uint64_t *policyVariable = cacheMemory->setReplacePolicy + set;

  for (uint8_t depth = 0; depth < cacheMemory->wayBits; depth++) {
    uint64_t policyMask = 1 << (1 + depth),
             wayMask = 1 << (cacheMemory->wayBits - 1);

    uint64_t checkedPolicyBit = *policyVariable & policyMask,
             checkedWayBit = recentAccessWay & wayMask;

    if ((!!checkedPolicyBit) == (!!checkedWayBit)) {
      continue;
    }

    if (checkedWayBit) {
      *policyVariable += checkedPolicyBit;
      continue;
    }

    *policyVariable &= ~checkedPolicyBit;
  }
}

#endif /* ifdef CACHE_SIMULATION_REPLACEMNT_POLICIES */
