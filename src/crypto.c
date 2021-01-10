#include "crypto.h"

void calc_md5 (const uint8_t *b, size_t l, uint8_t digest[HASH_LEN])
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, b, l);
    MD5_Final(digest, &ctx);
}
