#include "Setup.h"
#include "Debug.c"
#include "MainMemory.c"
#include "ReplacementPolicies.c"
#include "Cache.c"

void TestMainMemory()
{
  MainMemory mainMemory;

  int value = 9485;
  WriteToMainMemory(&mainMemory, 0, &value, sizeof(int));
  PrintMainMemory(&mainMemory);

  int key;
  ReadFromMainMemory(&mainMemory, 1, &key, sizeof(int));
  printf("%d\n", key);
}

void TestCache()
{
  MainMemory mainMemory;
  CacheMemory L1Cache;
  InitCacheMemory(&L1Cache, &mainMemory, 64, 8, 6);

  int value = 9485;
  WriteToCache(&L1Cache, 0, &value, sizeof(int));

  int key;
  ReadFromCache(&L1Cache, 1, &key, sizeof(int));
  printf("%d\n", key);

  FreeCacheMemory(&L1Cache);
}

int main()
{
  TestMainMemory();
  TestCache();
  return 0;
}
