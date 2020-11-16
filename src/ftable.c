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
    ft->n_files = 0;
    for (int i = 0; i < NUM_BUCKETS; i++) {
        ft->buckets[i] = new_ftable_bucket();
    }
    return ft;
}

static struct ftable_bucket *get_bucket_from_key(
    struct ftable *ft, char key[]
) {
    return ft->buckets[fthash(key) % NUM_BUCKETS];
}

static void add_file_to_bucket(
    struct ftable_file *file, struct ftable_bucket *bucket
) {
    if (file_in_ftable(file->name) == 1) {
        printf("'%s' already in the ftable.", file->name);
        return;
    }

    if (bucket->head == NULL) {
        bucket->head = file;
        bucket->n_entries++;
        return;
    }

    struct ftable_file *temp = bucket->head;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = file;
    temp->prev = temp;
    bucket->tail = file;
    bucket->n_entries++;
}

unsigned int ftable_add_file(
    struct ftable *ft, char name[],
    size_t s, size_t offset
) {
    struct ftable_file *f = new_ftable_file(name, s, offset);    
    unsigned int key = fthash(name) % NUM_BUCKETS;
    struct ftable_bucket *target_bucket = ft->buckets[key];
    add_file_to_bucket(f, target_bucket);
    ft->n_files++;
    return key;
}

// Return 1 if the file is in the ftable. Return 0 if not.
int file_in_ftable(struct ftable *ft, char name[])
{
    struct ftable_bucket *bucket = get_bucket_from_key(ft, name);
    struct ftable_file *temp = bucket->head;
    while (temp != NULL)
        if (strcmp(name, temp->name) == 0)
            return 1;
    return 0;
}

struct ftable_file ftable_get_file(struct ftable *ft, char name[])
{
    struct ftable_bucket *bucket = get_bucket_from_key(ft, name);
    struct ftable_file *temp = bucket->head;
    while (temp != NULL)
        if (strcmp(name, temp->name) == 0)
            return *temp;
    printf("'%s' not in ftable.\n", name);
    return (struct ftable_file){};
}

void destroy_ftable(struct ftable *ft)
{
    for (int i = 0; i < NUM_BUCKETS; i++)
        destroy_ftable_bucket(ft->buckets[i]);
    free(ft);
}

void destroy_ftable_file(struct ftable_file *file)
{
    free(file);
}

void destroy_ftable_bucket(struct ftable_bucket *bucket)
{
    struct ftable_file *temp;
    while (bucket->head != NULL) {
        temp = bucket->head;
        bucket->head = bucket->head->next;
        destroy_ftable_file(temp);
    }
    free(bucket);
}

