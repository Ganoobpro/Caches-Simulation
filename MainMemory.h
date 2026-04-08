#pragma once

#include "Setup.h"
#include "Debug.h"

#define MAIN_MEMORY_CAPACITY 4096
#define VISIBLE_MEMORY_ROWS  16
#define VISIBLE_MEMORY_COLS  8

typedef struct
{
  byte memorycell[MAIN_MEMORY_CAPACITY];
}
MainMemory;

void ClearMainMemory(MainMemory* mainMemory);
void PrintMainMemory(MainMemory* mainMemory);
void* AccessMainMemory(MainMemory* mainMemory, AddressType address);
void ReadFromMainMemory(MainMemory* mainMemory, AddressType address,
                       void* dest, AddressType destSize);
void WriteToMainMemory(MainMemory* mainMemory, AddressType address,
                       void* value, AddressType valueSize);
