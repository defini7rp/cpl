#ifndef CPL_UTILS_H_
#define CPL_UTILS_H_

#include <stddef.h>
#include <stdlib.h>

#define cplU_alloc(type) (type*)calloc(1, sizeof(type))

void cplU_log_err(const char* format, ...);
char* cplU_read_file(const char* filename, size_t* size);

#endif
