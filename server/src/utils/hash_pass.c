#include <openssl/evp.h>
#include <openssl/crypto.h>
#include <openssl/bio.h>

static unsigned char* hash_password(const char* password) {
    unsigned char* hash = malloc(EVP_MAX_MD_SIZE);
    size_t len;
    EVP_Q_digest(NULL, "SHA-256", NULL, password, strlen(password), hash, &len);
    size_t n = (len/3 + 1) * 4 + 1;
    unsigned char *base64_hash = malloc(n);
    EVP_EncodeBlock(base64_hash, hash, len);
    free(hash);
    return base64_hash;
}
