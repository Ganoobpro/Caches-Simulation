#ifndef CACHE_SIMULATION_CACHE
#define CACHE_SIMULATION_CACHE
#include "Cache.h"

static void
DivideAddress(const CacheMemory* cacheMemory,
              const AddressType address,
              AddressParts* addressParts)
{
  addressParts->offset = address & ((1 << OFFSET_BITS) - 1);
  addressParts->setIndex = (address >> OFFSET_BITS) & ((1 << cacheMemory->setBits) - 1);
  addressParts->tag = address >> (cacheMemory->setBits + OFFSET_BITS);
}

static inline CacheLine*
GetCacheLine(const CacheMemory* cacheMemory,
             const uint8_t set, const uint8_t way)
{
  return cacheMemory->cacheLines + set * cacheMemory->numberOfWays + way;
}



void
InitCacheMemory(CacheMemory* cacheMemory, MainMemory* mainMemory,
                uint8_t numberOfSets, uint8_t numberOfWays,
                uint8_t setBits)
{
  cacheMemory->mainMemory = mainMemory;

  int totalLines = numberOfSets * numberOfWays;
  cacheMemory->cacheLines = malloc(totalLines* sizeof(CacheLine));
  for(int i=0; i < totalLines; i++)
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

CacheLine*
LookupAndUpdateSet(CacheMemory* cacheMemory,
                   const AddressParts* addressParts)
{
  // LOOKUP
  uint8_t way;
  for(way = 0; way < cacheMemory->numberOfWays; way++)
  {
    CacheLine* checkedCacheLine = GetCacheLine(cacheMemory, addressParts->setIndex, way);

    // This way is not currently in used => Cache MISS
    ifnot(checkedCacheLine->valid) break;

    // Cache HIT
    if(addressParts->tag == checkedCacheLine->tag)
    {
      cacheMemory->cacheHit++;
      return checkedCacheLine;
    }
  }

  // Cache MISS -> UPDATE
  AddressType startDataChunk = (addressParts->tag << (cacheMemory->setBits + OFFSET_BITS))
                               + (addressParts->setIndex << OFFSET_BITS);

  uint8_t victimWay = when (way < cacheMemory->numberOfWays)
                      then way
                      other RandomReplacement(cacheMemory); // Replacement Policy
  CacheLine* victim = GetCacheLine(cacheMemory, addressParts->setIndex, victimWay);

  ReadFromMainMemory(cacheMemory->mainMemory, startDataChunk,
                     victim->dataCells, CACHE_LINE_DATA_SIZE);
  victim->valid = true;
  cacheMemory->cacheMiss++;

  return victim;
}

// TODO: Will apply MSI/MESI policy IN THE FUTURE
void ReadFromCache(CacheMemory* cacheMemory,
                   const AddressType mainMemoryAddress,
                   void* dest, const int destSize)
{
  // Divide Address into Tag, Set, and Offset
  AddressParts addressParts;
  DivideAddress(cacheMemory, mainMemoryAddress, &addressParts);

  // Handle Misaligned Access situation
  if(addressParts.offset + destSize >= CACHE_LINE_DATA_SIZE)
    ErrorMessage("[Cache] Cannot handle data across many cache lines.");

  CacheLine* target = LookupAndUpdateSet(cacheMemory, &addressParts);
  memcpy(dest,
         target->dataCells + addressParts.offset,
         destSize);
  target->dirty = false;
}

void WriteToCache(CacheMemory* cacheMemory,
                  const AddressType mainMemoryAddress,
                  const void* dest, const int destSize)
{
  // Divide Address into Tag, Set, and Offset
  AddressParts addressParts;
  DivideAddress(cacheMemory, mainMemoryAddress, &addressParts);

  // Handle Misaligned Access situation
  if(addressParts.offset + destSize >= CACHE_LINE_DATA_SIZE)
    ErrorMessage("[Cache] Cannot handle data across many cache lines.");

  CacheLine* target = LookupAndUpdateSet(cacheMemory, &addressParts);
  memcpy(target->dataCells + addressParts.offset,
         dest,
         destSize);
  target->dirty = true;
}

#endif
