#include "common/dream.h"
#include <stdio.h>

static uint8_t vec1[] = "Yet not so many, not so fair, as when Thangorodrim was broken";
static uint8_t vec2[] = "The quick brown fox jumps over the lazy brown dog.";
static uint8_t vec3[] = "Hello, world ";

void print_hex(const uint8_t *buf, size_t bl)
{
        for(size_t i = 0; i < bl; i++)
                printf("%02x", buf[i]);
        putchar('\n');
}

int main(void)
{
        /* hash tests */
        uint8_t digest128[dream128_DIGEST];
        uint8_t digest256[dream256_DIGEST];

        dream128_hash(vec1, sizeof vec1, digest128);
        print_hex(digest128, sizeof digest128);

        dream128_hash(vec2, sizeof vec2, digest128);
        print_hex(digest128, sizeof digest128);

        dream128_hash(vec3, sizeof vec3, digest128);
        print_hex(digest128, sizeof digest128);

        printf("128 bit hash tests passed...\n");

        dream256_hash(vec1, sizeof vec1, digest256);
        print_hex(digest256, sizeof digest256);

        dream256_hash(vec2, sizeof vec2, digest256);
        print_hex(digest256, sizeof digest256);

        dream256_hash(vec3, sizeof vec3, digest256);
        print_hex(digest256, sizeof digest256);
        printf("256 bit hash tests passed...\n");

        /* aead tests */
        uint8_t key[dream128_KEY] = {0};
        uint8_t iv[26] = {0};

        uint8_t vec1_e[sizeof vec1];
        uint8_t vec2_e[sizeof vec2];
        uint8_t vec3_e[sizeof vec3];

        uint8_t tag1[dream128_DIGEST];
        uint8_t tag2[dream128_DIGEST];
        uint8_t tag3[dream128_DIGEST];

        dream128_wrap(key, iv, sizeof iv, vec1, sizeof vec1, vec1_e, tag1);
        dream128_wrap(key, iv, sizeof iv, vec2, sizeof vec2, vec2_e, tag2);
        dream128_wrap(key, iv, sizeof iv, vec3, sizeof vec3, vec3_e, tag3);

        int ret1 = dream128_unwrap(key, iv, sizeof iv, vec1_e, sizeof vec1_e, vec1, tag1); 
        int ret2 = dream128_unwrap(key, iv, sizeof iv, vec2_e, sizeof vec2_e, vec2, tag2); 
        int ret3 = dream128_unwrap(key, iv, sizeof iv, vec3_e, sizeof vec3_e, vec3, tag3); 

        if(ret1 == 1 && ret2 == 1 && ret3 == 1)
                printf("128 bit aead tests passed...\n");
        else
                printf("128 bit aead tests failed...\n");

        uint8_t key2[dream256_KEY] = {0};
        uint8_t iv2[55] = {0};

        uint8_t tag11[dream256_DIGEST];
        uint8_t tag22[dream256_DIGEST];
        uint8_t tag33[dream256_DIGEST];
        
        dream256_wrap(key2, iv2, sizeof iv2, vec1, sizeof vec1, vec1_e, tag11);
        dream256_wrap(key2, iv2, sizeof iv2, vec2, sizeof vec2, vec2_e, tag22);
        dream256_wrap(key2, iv2, sizeof iv2, vec3, sizeof vec3, vec3_e, tag33);

        ret1 = dream256_unwrap(key2, iv2, sizeof iv2, vec1_e, sizeof vec1_e, vec1, tag11); 
        ret2 = dream256_unwrap(key2, iv2, sizeof iv2, vec2_e, sizeof vec2_e, vec2, tag22); 
        ret3 = dream256_unwrap(key2, iv2, sizeof iv2, vec3_e, sizeof vec3_e, vec3, tag33); 

        if(ret1 == 1 && ret2 == 1 && ret3 == 1)
                printf("256 bit aead tests passed...\n");
        else
                printf("256 bit aead tests failed...\n");
}
