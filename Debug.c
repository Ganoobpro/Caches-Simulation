#ifndef  CACHE_SIMULATION_DEBUG
#define CACHE_SIMULATION_DEBUG

#include "Setup.h"

void Error(char* message, int length)
{
  printf("--------------------------------------------------\n");
  printf("%.*s\n", length, message);
  exit(1);
}

#endif
