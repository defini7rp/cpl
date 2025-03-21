#include "cpl_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// TODO: Printing the location of where an error has occured
void cplU_log_err(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

char* cplU_read_file(const char* filename, size_t* size)
{
    FILE* file = fopen(filename, "rb");

    if (!file)
    {
        fprintf(stderr, "%s: %s", filename, strerror(errno));
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    char* buffer = (char*)calloc(1, *size + 1);

    if (!buffer)
    {
        fprintf(stderr, "%s: %s", filename, strerror(errno));
        fclose(file);
        return NULL;
    }

    if (fread(buffer, *size, 1, file) != 1)
    {
        fprintf(stderr, "%s: %s", filename, strerror(errno));
        free(buffer);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return buffer;
}
