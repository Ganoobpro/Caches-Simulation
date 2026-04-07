#ifndef CACHE_SIMULATION_CACHE
#define CACHE_SIMULATION_CACHE

static void
DivideAddress(CacheMemory* cacheMemory, const uint32_t& address,
              AddressParts* addressParts)
{
  addressParts->offset = mainMemoryAddress & ((1 << OFFSET_BITS) - 1);
  addressParts->set = (mainMemoryAddress >> cacheMemory->offsetBits) & ((1 << cacheMemory->setBits) - 1);
  addressParts->tag = mainMemoryAddress >> (cacheMemory->offsetBits + OFFSET_BITS);
}

static uint8_t
RandomReplacement(CacheMemory* cacheMemory)
{
  srand(time(NULL));
  // Expect numberOfWays is a Power of 2
  return (rand() * rand()) & (cacheMemory->numberOfWays - 1);
}



void
InitCacheMemory(mainMemory* mainMemory, CacheMemory* cacheMemory,
                uint8_t numberOfSets, uint8_t numberOfWays,
                uint8_t setBits)
{
  cacheMemory->cacheLines = (CacheLine*) malloc(numberOfSets * numberOfWays * sizeof(CacheLine));

  cacheMemory->cacheMemoryCapacity = numberOfSets * numberOfWays * CACHE_LINE_DATA_SIZE;
  cacheMemory->numberOfSets = numberOfSets;
  cacheMemory->numberOfWays = numberOfWays;
  cacheMemory->offsetBits = offsetBits;

  cacheMemory->cacheHit = 0;
  cacheMemory->cacheMiss = 0;
}

void
FreeCacheMemory(CacheMemory* cacheMemory)
{
  free(cacheMemory->cacheLines);
}

// TODO: Will apply MSI/MESI policy IN THE FUTURE
// NOTE: This function cannot handle to take integer number from offset 61+
void
LookupAndUpdateSet(CacheMemory* cacheMemory, const uint32_t& mainMemoryAddress,
                   void* dest, int destSize)
{
  // Divide Address into Tag, Set, and Offset
  AddressParts addressParts;
  DivideAddress(cacheMemory, mainMemoryAddress, &addressParts);
  if(addressParts->offset + destSize >= CACHE_LINE_DATA_SIZE)
  {
    Error("[Cache] Cannot handle data across many cache lines.");
  }

  // LOOKUP
  for(uint8_t way = 0; way < cacheMemory->numberOfWays; way++)
  {
    // Cache HIT
    if(tag == cacheMemory->cacheLines[addressParts->setIndex][way].tag)
    {
      memcpy(dest,
             cacheMemory->cacheLines[addressParts->setIndex][way].dataCells + addressParts->offset,
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
