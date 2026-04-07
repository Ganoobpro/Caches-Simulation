#ifndef  CACHE_SIMULATION_MAIN_MEMORY
#define CACHE_SIMULATION_MAIN_MEMORY
#include "MainMemory.h"



void
ClearMainMemory(MainMemory* mainMemory)
{
  for(int i=0; i<MAIN_MEMORY_CAPACITY; i++)
  {
    mainMemory->memorycell[i] = 0;
  }
}

void
PrintMainMemory(MainMemory* mainMemory)
{
  for(uint8_t i=0; i < VISIBLE_MEMORY_ROWS; i++)
  {
    for(uint8_t j=0; j < VISIBLE_MEMORY_COLS; j++)
    {
      printf("   0x%02X   ", mainMemory->memorycell[i * VISIBLE_MEMORY_ROWS + j]);
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
    ErrorMessage("[Main Memory] Unable to read from invalid main memory address!!!");
  }

  memcpy(dest, mainMemory->memorycell + address, destSize);
}

void WriteToMainMemory(MainMemory* mainMemory, AddressType address,
                       void* addressValue, AddressType addressSize)
{
  if(address + addressSize >= MAIN_MEMORY_CAPACITY)
  {
    ErrorMessage("[Main Memory] Unable to write from invalid main memory address!!!");
  }

  memcpy(mainMemory->memorycell + address, addressValue, addressSize);
}

int
main()
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

#endif
