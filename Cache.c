#ifndef CACHE_SIMULATION_CACHE
#define CACHE_SIMULATION_CACHE

static void
DivideAddress(CacheMemory* cacheMemory, const AddressType& address,
              AddressParts* addressParts)
{
  addressParts->offset = mainMemoryAddress & ((1 << OFFSET_BITS) - 1);
  addressParts->set = (mainMemoryAddress >> cacheMemory->offsetBits) & ((1 << cacheMemory->setBits) - 1);
  addressParts->tag = mainMemoryAddress >> (cacheMemory->offsetBits + OFFSET_BITS);
}

static inline void*
GetCacheLine(CacheMemory* cacheMemory, const uint8_t& set, const uint8_t& way)
{
  return cacheMemory->cacheLines + set * cacheMemory->numberOfWays + way;
}



void
InitCacheMemory(CacheMemory* cacheMemory, MainMemory* mainMemory,
                uint8_t numberOfSets, uint8_t numberOfWays,
                uint8_t setBits)
{
  cacheMemory->mainMemory = mainMemory;
  cacheMemory->cacheLines = (CacheLine*) malloc(numberOfSets * numberOfWays * sizeof(CacheLine));
  for(int i=0; i < numberOfSets * numberOfWays * sizeof(CacheLine); i++)
    cacheMemory->cacheLines[i].valid = false;

  cacheMemory->cacheMemoryCapacity = numberOfSets * numberOfWays * CACHE_LINE_DATA_SIZE;
  cacheMemory->numberOfSets = numberOfSets;
  cacheMemory->numberOfWays = numberOfWays;
  cacheMemory->setBits = setBits;

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
void*
LookupAndUpdateSet(CacheMemory* cacheMemory, const AddressType& mainMemoryAddress,
                   const int& destSize)
{
  // Divide Address into Tag, Set, and Offset
  AddressParts addressParts;
  DivideAddress(cacheMemory, mainMemoryAddress, &addressParts);

  if(addressParts->offset + destSize >= CACHE_LINE_DATA_SIZE)
    Error("[Cache] Cannot handle data across many cache lines.");

  // LOOKUP
  uint8_t way;
  for(way = 0; way < cacheMemory->numberOfWays; way++)
  {
    CacheLine* checkedCacheLine = (CacheLine*)
                                  GetCacheLine(cacheMemory, addressParts->setIndex, way);

    // This way is not currently in used => Cache MISS
    ifnot(checkedCacheLine->valid) break;

    // Cache HIT
    if(tag == cacheMemory->cacheLines[addressParts->setIndex][way].tag)
    {
      cacheMemory->cacheHit++;
      return checkedCacheLine->dataCells + addressParts->offset;
    }
  }

  // Cache MISS -> UPDATE
  AddressType startDataChunk = mainMemoryAddress & (((1 << (sizeof(AddressType) - OFFSET_BITS)) - 1) << OFFSET_BITS);
  CacheLine* victim = when (way < cacheMemory->numberOfWays)
                      then (CacheLine*) GetCacheLine(cacheMemory, addressParts->setIndex, way);
                      only; // Replacement policy

  ReadFromMainMemory(cacheMemory->mainMemory, mainMemoryAddress,
                     victim->dataCells, CACHE_LINE_DATA_SIZE);
  victim->valid = true;
  cacheMemory->cacheMiss++;

  return victim + addressParts->offset;
}

void ReadFromCache(CacheMemory* cacheMemory, const AddressType& mainMemoryAddress,
                   const void* dest, const int& destSize)
{
  CacheLine* target = LookupAndUpdateSet(cacheMemory, mainMemoryAddress, destSize);
  memcpy(dest, target, destSize);
}

void WriteToCache(CacheMemory* cacheMemory, const AddressType& mainMemoryAddress,
                   const void* dest, const int& destSize)
{
  CacheLine* target = LookupAndUpdateSet(cacheMemory, mainMemoryAddress, destSize);
  memcpy(target, dest, destSize);
}

#endif
