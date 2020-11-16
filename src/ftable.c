#include "ftable.h"

// Knuth's number
double A = (sqrt(5) - 1) / 2;

// Hash function for a key k.
unsigned long fthash(char *k)
{
    unsigned long key = to_radix(k);
    return floor(NUM_BUCKETS * (key*A-floor(key*A)))
}

struct new_ftable_file(char[] name, size_t s, offset)
{
    struct ftable_file *file = malloc(sizeof(struct ftable file));
    strcpy(file->name, name);
    file->s = s;
    file->offset = offset;
    file->next = NULL;
    file->prev = NULL;
    return file;
}

struct ftable_bucket *new_ftable_bucket()
{
    struct ftable_bucket *bucket = malloc(sizeof(struct ftable_bucket));

}

struct ftable *new_ftable()
{
    struct ftable *ft = malloc(sizeof(struct ftable));
    ftable->f_count = 0;
    for (int i = 0; i < NUM_BUCKETS; i++) {
        ftable->buckets[i] = new_ftable_bucket();
    }
    return ft;
}


