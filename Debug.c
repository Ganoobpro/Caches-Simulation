#ifndef  CACHE_SIMULATION_DEBUG
#define CACHE_SIMULATION_DEBUG

#include "Setup.h"

#define MAX_ERROR_STR_LENGTH 1024

void ErrorMessage(const char* message)
{
  printf("--------------------------------------------------\n");

  if(strlen(message) > MAX_ERROR_STR_LENGTH)
    printf("[DEBUG] System cannot handle error message bigger than 1,024 characters.\n");
  else
    printf("%.*s\n", strlen(message), message);

  exit(1);
}

#endif
