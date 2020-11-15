#include "memory.h"

struct memory *new_memory()
{
  struct memory *mem = malloc(sizeof(struct memory));
  mem->bytes = malloc(sizeof(char) * MEM_SIZE);
  mem->s = sizeof(char) * MEM_SIZE;

  return mem;
}
int dump(struct memory *mem, enum print_mode m)
{
  printf("\n=== BEGIN DUMP ===\n");
  for (int i = 0; i < mem->s; i++) {
    if (mem->bytes[i] == 0) {
      printf("%s", (m == HEX) ? "00 " : " ");
    }
    else if (m == HEX) {
      printf("%2x ", mem->bytes[i]);
    }
    else if (m == ASCII) {
      printf("%c", mem->bytes[i]);
    }
  }
  printf("\n=== END DUMP ===\n");
  return 0;
}

int write(
  struct memory *mem,
  char *b,
  size_t s,
  size_t offset
)
{
  if (s + offset >= mem->s) { printf("buffer is too big.\n");
    return 1;
  }
  for (size_t i = 0; i < s; i++) {
    mem->bytes[i + offset] = b[i];
  }
  return 0;
}

char *read(struct memory *mem, size_t s, size_t offset)
{
    char *buf = calloc(s, 1);

    for (int i = 0; i < s; i++) {
        size_t mem_index = offset + i;

        if (mem_index < mem->s) {
            buf[i] = mem->bytes[mem_index];
        } else {
            printf("error reading from memory.\n");
            return buf;
        }
    }

    return buf;
}
