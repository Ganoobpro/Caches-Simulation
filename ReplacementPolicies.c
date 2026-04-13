#ifndef CACHE_SIMULATION_REPLACEMENT_POLICIES
#define CACHE_SIMULATION_REPLACEMENT_POLICIES
#include "ReplacementPolicies.h"
#include "Cache.h"
#include "Debug.h"
#include <stdint.h>
#include <sys/types.h>

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

uint8_t LRU_Replacement(CacheMemory *cacheMemory, uint8_t set) {
  LMRU_SET_BITS lru;
  lru.data = cacheMemory->setReplacePolicy[set];
  return lru.leastUsedWay;
}

uint8_t MRU_Replacement(CacheMemory *cacheMemory, uint8_t set) {
  LMRU_SET_BITS lru;
  lru.data = cacheMemory->setReplacePolicy[set];
  return lru.recentUsedWay;
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

// Quad-Age LRU
uint8_t QLRU(CacheMemory *cacheMemory, uint8_t set) {
  uint64_t *haveAge3 = cacheMemory->setReplacePolicy + set;
  bool setIsNotOld = !(*haveAge3);

  if (setIsNotOld) {
    return 0xFF;
  }

  for (uint8_t way = 0; way < cacheMemory->numberOfWays; way++) {
    uint16_t age = GetCacheLine(cacheMemory, set, way)->policyVariable;
    if (age == 3) {
      return way;
    }
  }

  ErrorMessage("[QLRU] Something wrongs with your fucking code!!!");
  return 0xFF;
}

// Not Recently Used
uint8_t NRU_Replacement(CacheMemory *cacheMemory, uint8_t set) {
  typedef enum { NOT_CURRENTLY_USED, CURRENTLY_USED } NRU_STAT;

  for (uint8_t way = 0; way < cacheMemory->numberOfWays; way++) {
    CacheLine *checkedCacheLine = GetCacheLine(cacheMemory, set, way);
    NRU_STAT nru = checkedCacheLine->policyVariable;

    if (nru == NOT_CURRENTLY_USED) {
      return way;
    }
  }

  // SPECIAL CASE: All ways are currently used
  // => Meaning that the set is used frequently
  // Reset
  for (uint8_t i = 0; i < cacheMemory->numberOfWays; i++) {
    GetCacheLine(cacheMemory, set, i)->policyVariable = NOT_CURRENTLY_USED;
  }
  return 0xFF;
}

// Fuck this too hard to do
uint8_t Full_LRU(CacheMemory *cacheMemory, uint8_t set) {
  uint8_t *setListOrder = malloc(cacheMemory->numberOfWays);

  free(setListOrder);
  return 0;
}

////////////////////////////// UPDATE FUNCTIONS //////////////////////////////
void EmptyReplacementUpdate(CacheMemory *cacheMemory, uint8_t set,
                            uint8_t recentAccessWay) {}

void LMRU_Replacement_Update(CacheMemory *cacheMemory, uint8_t set,
                             uint8_t recentAccessWay) {
  // INIT
  LMRU_SET_BITS lru;
  uint64_t *policyVariable = cacheMemory->setReplacePolicy + set;
  lru.data = *policyVariable;

  // EARLY EXIT: Have just accessed
  if (lru.recentUsedWay == recentAccessWay) {
    return;
  }

  CacheLine *cacheLine = GetCacheLine(cacheMemory, set, recentAccessWay);
  LMRU_WAY_BITS lru_way;
  lru_way.data = cacheLine->policyVariable;

  LMRU_WAY_BITS lru_recent_way;
  CacheLine *recentCacheLine =
      GetCacheLine(cacheMemory, set, lru.recentUsedWay);
  lru_recent_way.data = recentCacheLine->policyVariable;

  // If recent access way is in the middle of the list
  if (lru.leastUsedWay != recentAccessWay) {
    CacheLine *nextCacheLine = GetCacheLine(cacheMemory, set, lru_way.nextWay);
    CacheLine *prevCacheLine = GetCacheLine(cacheMemory, set, lru_way.prevWay);

    LMRU_WAY_BITS lru_next_way, lru_prev_way;
    lru_next_way.data = nextCacheLine->policyVariable;
    lru_prev_way.data = prevCacheLine->policyVariable;

    // Patch the hole
    lru_prev_way.nextWay = lru_way.nextWay;
    lru_next_way.prevWay = lru_way.prevWay;

    // Finish patching
    nextCacheLine->policyVariable = lru_next_way.data;
    prevCacheLine->policyVariable = lru_prev_way.data;
  } // Else, recent access way is in the end ot the list
  else {
    lru.leastUsedWay = lru_way.prevWay;
  }

  // "Rewrite" the linked list node
  lru_way.nextWay = lru.recentUsedWay;
  lru_recent_way.prevWay = recentAccessWay;
  lru.recentUsedWay = recentAccessWay;

  // Update the policy variable
  *policyVariable = lru.data;
  cacheLine->policyVariable = lru_way.data;
  recentCacheLine->policyVariable = lru_recent_way.data;
}

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

void QLRU_Update(CacheMemory *cacheMemory, uint8_t set,
                 uint8_t recentAccessWay) {
  uint64_t *numberOfWayAged3 = cacheMemory->setReplacePolicy + set;
  CacheLine *cacheLine = GetCacheLine(cacheMemory, set, recentAccessWay);
  uint16_t *cacheLineAge = &cacheLine->policyVariable;

  // Cache MISS
  if (*cacheLineAge != 3) {
    *cacheLineAge = 1;
    (*numberOfWayAged3)--;
    return;
  }

  // Cache HIT
  if (*numberOfWayAged3) {
    return;
  }

  switch (*cacheLineAge) {
  case 1:
    *cacheLineAge = 0;
    break;
  case 2:
  case 3:
    *cacheLineAge = 1;
    break;
  default:
    ErrorMessage("[QLRU] Expect age is in {0, 1, 2, 3}.");
  }

  for (uint8_t way = 0; way < cacheMemory->numberOfWays; way++) {
    uint16_t age = GetCacheLine(cacheMemory, set, way)->policyVariable++;

    if (age == 3) {
      (*numberOfWayAged3)++;
    }
  }
}

#endif /* ifdef CACHE_SIMULATION_REPLACEMNT_POLICIES */
