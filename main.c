#include "Setup.h"
#include "Debug.c"
#include "MainMemory.c"

int main()
{
  MainMemory mainMemory;

  int value = 9485;
  WriteToMainMemory(&mainMemory, 7, &value, sizeof(int));
  PrintMainMemory(&mainMemory);

  int key;
  ReadFromMainMemory(&mainMemory, 9, &key, sizeof(int));
  printf("%d\n", key);
}
