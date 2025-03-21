#ifndef CPL_HASHTABLE_H_
#define CPL_HASHTABLE_H_

#include <stddef.h>

// Use prime number for the number of buckets
// to reduce a number of hash collisions,
// and since this hash table is used only for storing
// variables then the maximum amount of variables is 199
#define BUCKETS_COUNT 199

typedef struct cplHT_Bucket
{
    char* key;
    double value;
    struct cplHT_Bucket* next;
} cplHT_Bucket;

typedef struct cplHT_Table
{
    cplHT_Bucket** buckets;
    size_t size;
    size_t count;
} cplHT_Table;

size_t cplHT_hash(const char* key);

cplHT_Bucket* cplHT_new_bucket(const char* key, double value);
void cplHT_init(cplHT_Table* table);

void cplHT_insert(cplHT_Table* table, const char* key, double value);
void cplHT_get(cplHT_Table* table, const char* key, double* value);

void cplHT_remove(cplHT_Table* table, const char* key);
void cplHT_free(cplHT_Table* table);

#endif
