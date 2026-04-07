#ifndef  CACHE_SIMULATION_DEBUG
#define CACHE_SIMULATION_DEBUG

#include "Setup.h"

void ErrorMessage(const char* message)
{
  printf("--------------------------------------------------\n");
  printf("%.*s\n", strlen(message), message);
  exit(1);
}

#endif
