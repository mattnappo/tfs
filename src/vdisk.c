#include "vdisk.h"

struct vdisk *new_vdisk()
{
    struct vdisk *disk = malloc(sizeof(struct vdisk));
    disk->bytes = calloc(DISK_SIZE, 1);
    disk->s = DISK_SIZE;
    return disk;
}

void destroy_vdisk(struct vdisk *disk)
{
    free(disk->bytes);
    free(disk);
}

int vdisk_dump(struct vdisk *disk, enum print_mode m, int endpoint)
{
    if (endpoint >= disk->s) return 1;
    printf("\n=== BEGIN DUMP ===\n");
    for (int i = 0; i < endpoint; i++) {
        if (disk->bytes[i] == 0)
            printf("%s", (m == HEX) ? "00 " : " ");
        else if (m == HEX)
            printf("%2x ", disk->bytes[i]);
        else if (m == ASCII)
            printf("%c", disk->bytes[i]);
    }
    printf("\n=== END DUMP ===\n");
    return 0;
}

int vdisk_write(
    struct vdisk *disk,
    uint8_t *b,
    size_t s,
    size_t offset
) {
    if (s + offset >= disk->s) {
        printf("buffer is too big.\n");
        return 1;
    }
    memcpy(disk->bytes+offset, b, s);
    //for (size_t i = 0; i < s; i++)
    //    disk->bytes[i + offset] = b[i];
    return 0;
}

uint8_t *vdisk_read(struct vdisk *disk, size_t s, size_t offset)
{
    uint8_t *buf = calloc(s, 1);
    for (int i = 0; i < s; i++) {
        size_t index = offset + i;
        if (index < disk->s) {
            buf[i] = disk->bytes[index];
        } else {
            printf("error reading from vdisk.\n");
            return buf;
        }
    }
    return buf;
}
