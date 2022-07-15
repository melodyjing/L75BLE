#include "cobra_sha256.h"
#if (COBRA_SHA_EN)
/* Calculate SHA256 hash. */
bool rdw_sha256_hash(volatile RDW_SHA256_T *sha256, const void *src, void *dst, unsigned int count, bool add_padding)
{
    unsigned flags;

    flags = RDW_SHA256_INIT_HASH | RDW_SHA256_ENABLE;
    if (add_padding)
        flags |= RDW_SHA256_FINALISE_HASH;

    /* Start hash calculation. */
    sha256->SRC_ADDRESS = src;
    if (dst != NULL)
    {
        sha256->DST_ADDRESS = dst;
        flags |= RDW_SHA256_WRITE_HASH;
    }
    sha256->COUNT = count;
    sha256->CONTROL = flags;

    /* Wait until operation is complete. */
    while(!(sha256->STATUS & (RDW_SHA256_HC | RDW_SHA256_ER)))
        ;
//	printf("sha256 status %x\n",sha256->STATUS);
    /* Return true if no errors occured. */
    return sha256->STATUS & RDW_SHA256_ER ? false : true;
}
/* Write out SHA256 hash. */
bool rdw_sha256_hash_final_write(volatile RDW_SHA256_T *sha256, void *dst)
{
    /* Write out hash. */
    sha256->DST_ADDRESS = dst;
    sha256->COUNT = 0;
    sha256->CONTROL = RDW_SHA256_ENABLE | RDW_SHA256_DO_NOT_INIT_HASH | RDW_SHA256_FINALISE_HASH | RDW_SHA256_WRITE_HASH;

    /* Wait until operation is complete. */
    while(!(sha256->STATUS & (RDW_SHA256_HC | RDW_SHA256_ER)))
        ;

    /* Return true if no errors occured. */
    return sha256->STATUS & RDW_SHA256_ER ? false : true;
}
/* Update SHA256 hash. */
bool rdw_sha256_hash_update(volatile RDW_SHA256_T *sha256, const void *src, unsigned int count)
{
    /* Start hash calculation. */
    sha256->SRC_ADDRESS = src;
    sha256->COUNT = count;
    sha256->CONTROL = RDW_SHA256_ENABLE | RDW_SHA256_DO_NOT_INIT_HASH | RDW_SHA256_DO_NOT_FINALISE_HASH | RDW_SHA256_DO_NOT_WRITE_HASH;

    /* Wait until operation is complete. */
    while(!(sha256->STATUS & (RDW_SHA256_HC | RDW_SHA256_ER)))
        ;

    /* Return true if no errors occured. */
    return sha256->STATUS & RDW_SHA256_ER ? false : true;
}

/* For SHA-256 blocks that do not support writing the hash to memory, copy the computed hash value from the hash registers to memory. */
void write_hash(volatile RDW_SHA256_T *sha256, unsigned char *mem)
{
    int i;
    unsigned long t;

    for(i = 0; i < 8; i++) {
        /* Store MSB at lowest address. */
        t = sha256->HASH[7-i];
        *mem++ = t >> 24;
        *mem++ = t >> 16;
        *mem++ = t >> 8;
        *mem++ = t;
    }
}
#endif
