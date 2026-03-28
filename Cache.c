#ifndef CACHE_SIMULATION_CACHE
#define CACHE_SIMULATION_CACHE

#include "MainMemory.c"

// Cache Mapping: N-way Set Associative Mapping
#define CACHE_LINE_DATA_SIZE 64

typedef struct
{
  uint8_t dataCells[CACHE_LINE_DATA_SIZE];
  uint32_t tag;
  bool valid;
  bool dirty;
}
CacheLine;

typedef struct
{
  CacheLine* cacheLines;

  int cacheMemoryCapacity;
  uint8_t numberOfSets;
  uint8_t numberOfWays;
  uint8_t setBits;
  uint8_t offsetBits;

  // For statistic purpose
  int cacheHit, cacheMiss;
}
CacheMemory;

void
InitCacheMemory(CacheMemory* cacheMemory,
                uint8_t numberOfSets, uint8_t numberOfWays,
                uint8_t setBits, uint8_t offsetBits)
{
  cacheMemory->cacheLines = (CacheLine*) malloc(numberOfSets * numberOfWays * sizeof(CacheLine));

  cacheMemory->cacheMemoryCapacity = numberOfSets * numberOfWays * CACHE_LINE_DATA_SIZE;
  cacheMemory->numberOfSets = numberOfSets;
  cacheMemory->numberOfWays = numberOfWays;
  cacheMemory->setBits = setBits;
  cacheMemory->offsetBits = offsetBits;

  cacheMemory->cacheHit = 0;
  cacheMemory->cacheMiss = 0;
}

void
FreeCacheMemory(CacheMemory* cacheMemory)
{
  free(cacheMemory->cacheLines);
}

static void
DivideAddress(CacheMemory* cacheMemory, uint32_t address,
              uint32_t &tag, uint8_t &set, uint8_t &offset, int sizeOfDestination)
{
  uint8_t offset = mainMemoryAddress & ((1 << cacheMemory->offsetBits) - 1);
  if(offset + sizeOfDestination >= CACHE_LINE_DATA_SIZE)
  {
    Error("[Cache] Unable to handle cross memory chunk!", 47);
  }

  uint8_t set = (mainMemoryAddress >> cacheMemory->offsetBits) & ((1 << cacheMemory->setBits) - 1);
  uint32_t tag = mainMemoryAddress >> (cacheMemory->offsetBits + cacheMemory->setBits);
}

static uint8_t
RandomReplacement(CacheMemory* cacheMemory)
{
  srand(time(NULL));
  // Expect numberOfWays is a Power of 2
  return (rand() * rand()) & (cacheMemory->numberOfWays - 1);
}

// TODO: Will apply MSI/MESI policy IN THE FUTURE
// NOTE: This function cannot handle to take integer number from offset 61+
void
LookupAndUpdateSet(CacheMemory* cacheMemory, uint32_t mainMemoryAddress,
                    void* dest, int destSize)
{
  uint32_t tag;
  uint8_t set, offset;
  DivideAddress(cacheMemory, mainMemoryAddress,
                tag, set, offset, destSize);

  // LOOKUP
  for(uint8_t way = 0; way < cacheMemory->numberOfWays; way++)
  {
    // Cache HIT
    if(tag == cacheMemory->cacheLines[set][way].tag)
    {
      memcpy(dest,
             cacheMemory->cacheLines[set][way].dataCells + offset,
             sizeOfDestination);
      cacheMemory->cacheHit++;
      return;
    }
  }

  // Cache MISS -> UPDATE
  uint8_t victim = RandomReplacement(cacheMemory);
  AccessMainMemory(mainMemory, mainMemoryAddress,
                   cacheMemory->cacheLines[set][way].dataCells + offset,
                   destSize);
  cacheMemory->cacheMiss++;
}

#endif
