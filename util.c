#include "util.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>

char*
dupstr(char* str)
{
    size_t length = strlen(str) + 1;

    char* new_str = malloc(length);
    if (new_str == NULL)
    {
        // TODO: error returnen
        assert(false);
    }

    memcpy(new_str, str, length);

    return new_str;
}
