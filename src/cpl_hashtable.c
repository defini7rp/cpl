#include "cpl_hashtable.h"

#include <string.h>
#include <stdlib.h>

size_t cplHT_hash(const char* key)
{
    size_t hash = 5381;
    size_t c;

    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;

    return hash % BUCKETS_COUNT;
}

cplHT_Bucket* cplHT_new_bucket(const char* key, double value)
{
    cplHT_Bucket* bucket = (cplHT_Bucket*)calloc(1, sizeof(cplHT_Bucket));

    bucket->key = strdup(key);
    bucket->value = value;

    return bucket;
}

void cplHT_init(cplHT_Table* table)
{
    table->size = BUCKETS_COUNT;
    table->buckets = (cplHT_Bucket**)calloc(BUCKETS_COUNT, sizeof(cplHT_Bucket*));
}

void cplHT_insert(cplHT_Table* table, const char* key, double value)
{
    cplHT_Bucket* bucket = cplHT_new_bucket(key, value);
    size_t index = cplHT_hash(key);

    if (table->buckets[index])
        bucket->next = table->buckets[index];

    table->buckets[index] = bucket;
    table->count++;
}

bool cplHT_get(cplHT_Table* table, const char* key, double* value)
{
    cplHT_Bucket* bucket = table->buckets[cplHT_hash(key)];

    while (bucket)
    {
        if (strcmp(bucket->key, key) == 0)
        {
            *value = bucket->value;
            return true;
        }

        bucket = bucket->next;
    }

    return false;
}

void cplHT_remove(cplHT_Table* table, const char* key)
{
    size_t index = cplHT_hash(key);

    cplHT_Bucket* bucket = table->buckets[index];
    cplHT_Bucket* prev = NULL;

    while (bucket)
    {
        if (strcmp(bucket->key, key) == 0)
        {
            if (bucket->next)
            {
                if (prev)
                    prev->next = bucket->next;
                else
                    table->buckets[index] = bucket->next;
            }

            free(bucket->key);
            free(bucket);

            return;
        }

        prev = bucket;
        bucket = bucket->next;
    }
}

void cplHT_free(cplHT_Table* table)
{
    for (size_t i = 0; i < table->count; i++)
    {
        cplHT_Bucket* bucket = table->buckets[i];

        while (bucket)
        {
            free(bucket->key);
            bucket = bucket->next;
        }
    }

    free(table->buckets);

    table->count = 0;
    table->size = 0;
}
