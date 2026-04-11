#ifndef CACHE_SIMULATION_DEBUG
#define CACHE_SIMULATION_DEBUG

#include "Debug.h"

#define MAX_ERROR_STR_LENGTH 1024

void ErrorMessage(const char *message) {
  printf("--------------------------------------------------\n");

  if (strlen(message) > MAX_ERROR_STR_LENGTH)
    fprintf(stderr, "[DEBUG] System cannot handle error message bigger than "
                    "1,024 characters.\n");
  else
    fprintf(stderr, "%.*s\n", strlen(message), message);

  exit(1);
}

#endif
