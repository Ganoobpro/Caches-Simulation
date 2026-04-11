#ifndef CACHE_SIMULATION_CACHE
#define CACHE_SIMULATION_CACHE
#include "Cache.h"

static uint8_t
SimpleLog2(uint8_t number)
{
  uint8_t answer;
  for(answer = 0; (1 << answer) < number; answer++){}

  ifnot((1 << answer) == answer)
    ErrorMessage("[Cache] Expect number of Set is a power of 2.")

  return answer;
}

static void
DivideAddress(CacheMemory* cacheMemory,
              AddressType address,
              AddressParts* addressParts)
{
  addressParts->offset = address & ((1 << OFFSET_BITS) - 1);
  addressParts->setIndex = (address >> OFFSET_BITS) & ((1 << cacheMemory->setBits) - 1);
  addressParts->tag = address >> (cacheMemory->setBits + OFFSET_BITS);
}

static inline bool
ValidAddress(AddressParts* addressParts, int size)
{
  return addressParts->offset + size < CACHE_LINE_DATA_SIZE;
}

static inline CacheLine*
GetCacheLine(CacheMemory* cacheMemory,
             uint8_t set, uint8_t way)
{
  return cacheMemory->cacheLines + set * cacheMemory->numberOfWays + way;
}

static void
ReplaceCacheLine(CacheMemory* cacheMemory,
                 CacheLine* victim,
                 AddressType mainMemoryAddress)
{
  AddressType startChunk = mainMemoryAddress & (~((AddressType) OFFSET_BITS));
  ReadFromMainMemory(cacheMemory->mainMemory,
                     startChunk,
                     cacheLine->data,
                     CACHE_LINE_DATA_SIZE);
}



void
InitCacheMemory(CacheMemory* cacheMemory, MainMemory* mainMemory,
                uint8_t numberOfSets, uint8_t numberOfWays)
{
  int numberOfCells = numberOfSets * numberOfWays;

  cacheMemory->mainMemory = mainMemory;
  cacheMemory->cacheLines = malloc(numberOfCells * CacheLine);

  cacheMemory->setBits      = SimpleLog2(numberOfSets);
  cacheMemory->numberOfSets = numberOfSets;
  cacheMemory->numberOfWays = numberOfWays;

  cacheMemory->cacheMemoryCapacity = numberOfCells * CACHE_LINE_DATA_SIZE;
  cacheMemory->dataCells = malloc(cacheMemory->cacheMemoryCapacity);
  for(int i=0; i<numberOfCells; i++)
    cacheMemory->cacheLines[i] = cacheMemory->dataCells + i * CACHE_LINE_DATA_SIZE;

  cacheMemory->cacheHit  = 0;
  cacheMemory->cacheMiss = 0;

  cacheMemory->setOther = malloc(numberOfSets * sizeof(int));
}

void
FreeCacheMemory(CacheMemory* cacheMemory)
{
  free(cacheMemory->cacheLines);
  free(cacheMemory->dataCells);
  free(cacheMemory->setOther);
}

CacheLine*
LookupAndUpdateSet(CacheMemory* cacheMemory,
                   AddressParts* addressParts,
                   AddressType mainMemoryAddress)
{
  for(uint8_t way = 0; way < cacheMemory->numberOfWays; way++)
  {
    CacheLine* checkedCacheLine = GetCacheLine(cacheMemory,
                                               addressParts->set,
                                               way);

    // Reach the end of the Set => Cache MISS
    // However this situation doesnt need Replacemnt Algorithm
    ifnot(checkedCacheLine->valid)
    {
      ReplaceCacheLine(cacheMemory, checkedCacheLine, mainMemoryAddress);
      return checkedCacheLine;
    }

    // Cache HIT
    if(addressParts->tag == checkedCacheLine->tag)
      return checkedCacheLine;
  }


}

void
ReadFromCache(CacheMemory* cacheMemory,
              AddressType mainMemoryAddress,
              void* dest, int destSize)
{
  AddressParts addressParts = DivideAddress(mainMemoryAddress);
  ifnot(ValidAddress(addressParts))
    ErrorMessage("[Cache] Unable to handle cross cache line data.");

  memcpy(dest,
         LookupAndUpdateSet(cacheMemory, addressParts, mainMemoryAddress),
         destSize);
}

void
WriteToCache(CacheMemory* cacheMemory,
             AddressType mainMemoryAddress,
             void* dest, int destSize)
{
  AddressParts addressParts = DivideAddress(mainMemoryAddress);
  ifnot(ValidAddress(addressParts))
    ErrorMessage("[Cache] Unable to handle cross cache line data.");

  memcpy(LookupAndUpdateSet(cacheMemory, addressParts, mainMemoryAddress),
         dest,
         destSize);
}

#endif
