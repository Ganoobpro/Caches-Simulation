#pragma once

#include "Cache.h"
#include "Setup.h"

uint8_t RandomReplacement(CacheMemory *cacheMemory, uint8_t set);
uint8_t FIFO_Replacement(CacheMemory *cacheMemory, uint8_t set);

void EmptyReplacementUpdate(CacheMemory *cacheMemory, uint8_t set, uint8_t way);
