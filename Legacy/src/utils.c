// utils.c
#include "include/types.h"
#include "include/defines.h"

SDL_AppResult Panic(const char* msg)
{
  const char* error = SDL_GetError();

  // If theres an error message, then print it
  if (error && error[0] != '\0')
    printf("Panic! %s: \t%s\n", msg, error);

  else
    printf("Panic! %s: \t(No Error Message)\n", msg);

  return SDL_APP_FAILURE;
}
