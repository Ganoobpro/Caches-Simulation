#ifndef  CACHE_SIMULATION_MAIN_MEMORY
#define CACHE_SIMULATION_MAIN_MEMORY

#include "Setup.h"
#include "MemorySetup.h"
#include "Debug.c"

#define MAIN_MEMORY_CAPACITY 4096

typedef struct
{
  uint8_t memorycell[MAIN_MEMORY_CAPACITY];
}
MainMemory;

MainMemory mainMemory;

void InitMainMemory(MainMemory* mainMemory)
{
  for(int i=0; i<MAIN_MEMORY_CAPACITY; i++)
  {
    mainMemory->memorycell[i] = 0;
  }
}

void PrintMainMemory(MainMemory* mainMemory)
{
  for(uint8_t i=0; i<(MAIN_MEMORY_CAPACITY>>3); i++)
  {
    for(uint8_t j=0; j<8; j++)
    {
      printf("   0x%02X   ", mainMemory->memorycell[(i<<3) + j]);
    }
    printf("\n");
  }
}

void
AccessMainMemory(MainMemory* mainMemory, AddressType address,
                 void* dest, AddressType destSize)
{
  if(address >= MAIN_MEMORY_CAPACITY)
  {
    Error("[Main Memory] Unable to read from invalid main memory address!!!", 64);
  }

  memcpy(dest, mainMemory->memorycell + address, destSize);
}

void WriteToMainMemory(MainMemory* mainMemory, AddressType address,
                       void* addressValue, AddressType addressSize)
{
  if(address + addressSize >= MAIN_MEMORY_CAPACITY)
  {
    Error("[Main Memory] Unable to write from invalid main memory address!!!", 65);
  }

  memcpy(mainMemory->memorycell + address, addressValue, addressSize);
}

/*
int main()
{
  MainMemory mainMemory;
  InitMainMemory(&mainMemory);

  int value = 9485;
  WriteToMainMemory(&mainMemory, 1025, &value, sizeof(int));
  PrintMainMemory(&mainMemory);

  int key;
  ReadFromMainMemory(&mainMemory, 1025, &key, sizeof(int));
  printf("%d", key);

  return 0;
}
*/

#endif
