#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/md5.h>
#include <string.h>

#define HASH_LEN 16

// md5 l bytes b into digest of length HASH_LEN
void calc_md5(const uint8_t *b, size_t l, uint8_t digest[HASH_LEN]);

#endif
