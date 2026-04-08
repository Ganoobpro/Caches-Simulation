#ifndef  CACHE_SIMULATION_MAIN_MEMORY
#define CACHE_SIMULATION_MAIN_MEMORY
#include "MainMemory.h"
#include "Debug.c"

void
ClearMainMemory(MainMemory* mainMemory)
{
  for(int i=0; i<MAIN_MEMORY_CAPACITY; i++)
  {
    mainMemory->memorycell[i] = 0;
  }
}

void
PrintMainMemory(const MainMemory* mainMemory)
{
  for(int i=0; i < VISIBLE_MEMORY_ROWS; i++)
  {
    for(int j=0; j < VISIBLE_MEMORY_COLS; j++)
    {
      printf("   0x%02X   ", mainMemory->memorycell[i * VISIBLE_MEMORY_COLS + j]);
    }
    printf("\n");
  }
}

void*
AccessMainMemory(const MainMemory* mainMemory,
                 const AddressType address)
{
  if(address >= MAIN_MEMORY_CAPACITY)
  {
    ErrorMessage("[Main Memory] Unable to read from invalid main memory address!!!");
  }

  return (void*)mainMemory->memorycell + address;
}

void
ReadFromMainMemory(const MainMemory* mainMemory,
                   const AddressType address,
                   void* dest, const AddressType destSize)
{
  memcpy(dest,
         AccessMainMemory(mainMemory, address),
         destSize);
}

void
WriteToMainMemory(MainMemory* mainMemory,
                  const AddressType address,
                  const void* value,
                  const AddressType valueSize)
{
  memcpy(AccessMainMemory(mainMemory, address),
         value, valueSize);
}

#endif
