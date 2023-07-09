#ifndef UTIL_H
#define UTIL_H
#include <stdlib.h>

#define SAFE_FREE(x) do { free(x); (x) = NULL; } while (0)

char* dupstr(const char* str);

#endif
