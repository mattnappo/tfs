#include "fs.h"

struct file new_file(const char *name)
{
    struct file *f = malloc(sizeof(struct file));
    size_t fsize, read_size;
    FILE *fp = fopen(name, "r");
  
    if (fp) {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        f->bytes = malloc(sizeof(char) * (fsize + 1));
    
        rewind(fp);
        read_size = fread(f->bytes, sizeof(char), fsize, fp);
    
        f->bytes[fsize] = '\0';
        f->s = read_size;
    
        if (fsize != read_size) {
            free(f->bytes);
            f->bytes = NULL;
            f->s = 0;
        }
    
        fclose(fp);
    }
    strcpy(f->name, name);
  
    return *f;
}

void destroy_file(struct file f)
{
    free(f.bytes);
}

void print_file(struct file f)
{
    printf("fs file:\n  name: %s\n  size: %ld\n  bytes: ", f.name, f.s
    );

    // TODO: make a temp memory and use dump here.
    for (int i = 0; i < f.s; i++) {
        printf("%2x ", f.bytes[i]);
    }
    printf("\n");
}

struct fs *new_fs() // TODO: size parameter
{
    struct fs *fs = malloc(sizeof(struct fs));
    fs->mem = new_memory();
    fs->ft  = new_ftable();

    return fs;
}

void destroy_fs(struct fs *fs)
{
    destroy_memory(fs->mem);
    destroy_ftable(fs->ft);
    free(fs);
}

void add_file(struct fs *fs, struct file f, size_t offset)
{
    int status = ftable_add_file(fs->ft, f.name, f.s, offset);
    if (status == -1)
        printf("'%s' is already in the fs\n", f.name);
    else
        write(fs->mem, f.bytes, f.s, offset);
}

int remove_file();

struct file get_file(struct fs *fs, char name[])
{
    struct ftable_file ftfile = ftable_get_file(fs->ft, name);
    char *memory = read(fs->mem, ftfile.s, ftfile.offset);
    struct file f;
    strcpy(f.name, name);
    f.bytes = malloc(sizeof(char) * strlen(memory));
    strcpy(f.bytes, memory);
    f.s = ftfile.s;
    return f;
}

