#ifndef CPL_UTILS_H_
#define CPL_UTILS_H_

#include <stddef.h>

void cplU_log_err(const char* format, ...);
char* cplU_read_file(const char* filename, size_t* size);

#endif
