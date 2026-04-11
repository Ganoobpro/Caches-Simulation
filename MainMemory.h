#pragma once

#include "Debug.h"
#include "Setup.h"

#define MAIN_MEMORY_CAPACITY 1048576 // 1MB RAM
#define VISIBLE_MEMORY_ROWS 16
#define VISIBLE_MEMORY_COLS 8

typedef struct {
  byte memorycell[MAIN_MEMORY_CAPACITY];
} MainMemory;

void ClearMainMemory(MainMemory *mainMemory);
void PrintMainMemory(const MainMemory *mainMemory);
void *AccessMainMemory(const MainMemory *mainMemory, const AddressType address);
void ReadFromMainMemory(const MainMemory *mainMemory, const AddressType address,
                        void *dest, const AddressType destSize);
void WriteToMainMemory(MainMemory *mainMemory, const AddressType address,
                       const void *value, const AddressType valueSize);
