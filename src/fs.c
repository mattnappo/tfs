#include "fs.h"

struct file new_file(const char *name)
{
    struct file *f = malloc(sizeof(struct file));
    size_t fsize, read_size;
    FILE *fp = fopen(name, "r");
    if (fp == NULL) {
        printf("could not open file '%s'\n", name);
        exit(1);
    }
  
    if (fp) {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        f->bytes = malloc(sizeof(uint8_t) * (fsize + 1));
    
        rewind(fp);
        read_size = fread(f->bytes, sizeof(uint8_t), fsize, fp);
    
        f->bytes[fsize] = '\0';
        f->s = read_size;
    
        if (fsize != read_size) {
            free(f->bytes);
            f->bytes = NULL;
            f->s = 0;
            printf("error in reading file.");
            exit(1);
        }
    
        fclose(fp);
    }
    
    strcpy(f->name, name);

    struct file file = *f;
    free(f);
    return file;
}

int is_valid_filename(const char *name)
{
    if (strlen(name) >= FILENAME_SIZE) {
        return 0;
    }
    const char invalid_chars[] = { '\\' };
    for (int c = 0; c < sizeof(invalid_chars); c++) {
        if (strchr(name, invalid_chars[c]))
            return 0;
    }
    return 1;
}

void destroy_file(struct file f) { free(f.bytes); }

void print_file(struct file f, enum print_mode mode)
{
    printf("fs file:\n  name: %s\n  size: %ld\n  bytes: ", f.name, f.s);
    if (f.s < 0) return;
    if (mode == HEX) {
        for (int i = 0; i < f.s; i++) {
            printf("%2x ", f.bytes[i]);
        }
        printf("\n");
    } else if (mode == ASCII) {
        for (int i = 0; i < f.s; i++) {
            printf("%c", f.bytes[i]);
        }
        printf("\n");
    }
}

struct fs *new_fs()
{
    struct fs *fs = malloc(sizeof(struct fs));
    fs->disk = new_vdisk();
    fs->ft = new_ftable();
    return fs;
}

struct temp_fsid calc_fsid(struct fs *fs)
{
    uint8_t fsid[FSID_LEN];
    memset(fsid, 0, FSID_LEN);
    const time_t key = time(NULL);
    // Maybe serialize the fs to get better fsid calculation?
    calc_md5((uint8_t *) &key, sizeof(time_t), fsid);
    struct temp_fsid rfsid = {};
    memcpy(rfsid.fsid, fsid, FSID_LEN);
    return rfsid;
}

void print_fsid(uint8_t fsid[FSID_LEN])
{
    printf("0x");
    for (int b = 0; b < FSID_LEN; b++)
        printf("%02x", fsid[b]);
    printf("\n");
}

char *stringify_fsid(uint8_t fsid[FSID_LEN])
{
    char *str = calloc(FSID_LEN*2+2+1, 1);
    str[0] = '0';
    str[1] = 'x';
    for (int i = 0; i < FSID_LEN; i++) {
        char temp[3];
        snprintf(temp, 3, "%02x", fsid[i]);
        strncat(str, temp, 2);
    }
    return str;
}

// 1 if they are equal, 0 if they are not equal
int fsid_equal(uint8_t base[FSID_LEN], uint8_t other[FSID_LEN])
{
    return memcmp(base, other, FSID_LEN) == 0;
}

void destroy_fs(struct fs *fs)
{
    destroy_vdisk(fs->disk);
    destroy_ftable(fs->ft);
    free(fs);
}

int fs_add_file(struct fs *fs, struct file f, size_t offset)
{
    if (f.s >= MAX_FILE_LEN) {
        printf("file '%s' is too large (size %lu).\n", f.name, f.s);
        return 1;
    }
    if (!is_valid_filename(f.name)) {
        printf("file '%s' has an invalid name.\n", f.name);
        return 1;
    }
    int status = ftable_add_file(fs->ft, f.name, f.s, offset);
    if (status == -1)
        printf("'%s' is already in the fs.\n", f.name);
    else
        vdisk_write(fs->disk, f.bytes, f.s, offset);
    return 0;
}

int fs_remove_file();

struct file fs_get_file(struct fs *fs, char *name)
{
    struct ftable_file ftfile = ftable_get_file(fs->ft, name);
    if (ftfile.s == -1) {
        printf("'%s' not in fs.\n", name);
        return (struct file) {};
    }
    uint8_t *mem = vdisk_read(fs->disk, ftfile.s, ftfile.offset);

    struct file f;
    strcpy(f.name, ftfile.name);
    f.bytes = calloc(ftfile.s, 1);
    memcpy(f.bytes, mem, ftfile.s);
    f.s = ftfile.s;

    free(mem);
    return f;
}

void fs_list_files(struct fs fs)
{
    print_ftable(*fs.ft);
}

void fs_print_file(struct fs *fs, char *name, int show_mem);

