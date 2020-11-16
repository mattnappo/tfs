#include "ftable.h"

// Knuth's number
#define A ((sqrt(5) - 1) / 2)

// Hash function for a key k.
unsigned long fthash(char *k)
{
    unsigned long key = to_radix(k);
    return floor(NUM_BUCKETS * (key*A-floor(key*A)));
}

struct ftable_file *new_ftable_file(char name[], size_t s, size_t offset)
{
    struct ftable_file *file = malloc(sizeof(struct ftable_file));
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
    bucket->head = NULL;
    bucket->tail = NULL;
    bucket->n_entries = 0;
    return bucket;
}

struct ftable *new_ftable()
{
    struct ftable *ft = malloc(sizeof(struct ftable));
    ft->f_count = 0;
    for (int i = 0; i < NUM_BUCKETS; i++) {
        ft->buckets[i] = new_ftable_bucket();
    }
    return ft;
}

static void add_file_to_bucket(
    struct ftable_file *file, struct ftable_bucket *bucket
) {
    // TODO: Check that it is not already in the hashmap
    if (bucket->head == NULL) {
        bucket->head = file;
        bucket->n_entries++;
        return;
    }

    struct ftable_file *temp = bucket->head;
    while (temp->next != NULL) {
        printf("I AM RUNNING \n");
        temp = temp->next;
    }
    temp->next = file;
    temp->prev = temp;
    bucket->tail = file;
    bucket->n_entries++;
}

unsigned long ftable_add_file(
    struct ftable *ft, char name[],
    size_t s, size_t offset
) {
    struct ftable_file *f = new_ftable_file(name, s, offset);
    unsigned long key = fthash(name);

    struct ftable_bucket *target_bucket = ft->buckets[key % NUM_BUCKETS];
    add_file_to_bucket(f, target_bucket);

    return key;
}

