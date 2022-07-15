#ifndef _SHA_256_H
#define _SHA_256_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cobra_config.h>

#if (COBRA_SHA_EN)
/* Status register bits.  */
#define RDW_SHA256_HC                   0x0001
#define RDW_SHA256_ER                   0x0002

/* Control register bit masks.  */
#define RDW_SHA256_ENABLE_MASK          0x0001
#define RDW_SHA256_HC_IE_MASK           0x0002
#define RDW_SHA256_ER_IE_MASK           0x0004

/* Control register values.  */
#define RDW_SHA256_DISABLE              (0x0 << 0)
#define RDW_SHA256_ENABLE               (0x1 << 0)
#define RDW_SHA256_TC_INT_DISABLE       (0x0 << 1)
#define RDW_SHA256_TC_INT_ENABLE        (0x1 << 1)
#define RDW_SHA256_ER_INT_DISABLE       (0x0 << 2)
#define RDW_SHA256_ER_INT_ENABLE        (0x1 << 2)
#define RDW_SHA256_DO_NOT_INIT_HASH     (0x0 << 3)
#define RDW_SHA256_INIT_HASH            (0x1 << 3)
#define RDW_SHA256_DO_NOT_FINALISE_HASH (0x0 << 4)
#define RDW_SHA256_FINALISE_HASH        (0x1 << 4)
#define RDW_SHA256_DO_NOT_WRITE_HASH    (0x0 << 5)
#define RDW_SHA256_WRITE_HASH           (0x1 << 5)

/** eSi-SHA256 registers.  */
typedef volatile struct RDW_SHA256 {
    __attribute__ ((aligned (4))) const void * volatile SRC_ADDRESS;              /**< Source address of data to calculate hash for.  */
    __attribute__ ((aligned (4))) void * volatile DST_ADDRESS;                    /**< Destination address to store computed hash value.  */
    __attribute__ ((aligned (4))) volatile unsigned int COUNT;                    /**< Number of bytes of data to calculate hash for.  */
    __attribute__ ((aligned (4))) volatile unsigned int STATUS;                   /**< Status register.  */
    __attribute__ ((aligned (4))) volatile unsigned int CONTROL;                  /**< Control register.  */
    __attribute__ ((aligned (4))) volatile unsigned char PAD[12];
    __attribute__ ((aligned (4))) volatile unsigned int HASH[8];                  /**< Calculated hash.  */
} RDW_SHA256_T;

/** Initialise, calculate and write out the SHA256 hash for a single block of data.
 * @param sha256 - Pointer to SHA256 registers.
 * @param src - Pointer to data to calculate hash for.
 * @param dst - Pointer to location to store 256-bit hash. Set to NULL if hash should not be written out.
 * @param count - Length of data in bytes pointed to by src.
 * @param add_padding - Whether to append padding to data.
 * @return true if hash succesfully written out, false if an error occurred.
 */
bool rdw_sha256_hash (volatile RDW_SHA256_T *sha256, const void *src, void *dst, unsigned int count, bool add_padding);
bool rdw_sha256_hash_final_write(volatile RDW_SHA256_T *sha256, void *dst);
bool rdw_sha256_hash_update(volatile RDW_SHA256_T *sha256, const void *src, unsigned int count);
void write_hash(volatile RDW_SHA256_T *sha256, unsigned char *mem);

/** Initliase hash accumulator, before starting a hash calculation for multiple blocks of data.
 * @param sha256 - Pointer to SHA256 registers.
 */
static inline void rdw_sha256_hash_init (volatile RDW_SHA256_T *sha256)
{
    sha256->COUNT = 0;
    sha256->CONTROL = RDW_SHA256_ENABLE | RDW_SHA256_INIT_HASH | RDW_SHA256_DO_NOT_FINALISE_HASH | RDW_SHA256_DO_NOT_WRITE_HASH;
}

/** Calculate and update the SHA256 hash for multiple blocks of data.
 * @param sha256 - Pointer to SHA256 registers.
 * @param src - Pointer to data to calculate hash for.
 * @param count - Length of data in bytes pointed to by src.
 * @return true if hash succesfully updated, false if an error occurred.
 */
extern bool rdw_sha256_hash_update (volatile RDW_SHA256_T *sha256, const void *src, unsigned int count);

/** Finalise hash (add padding and calculate) and write to memory.
 * @param sha256 - Pointer to SHA256 registers.
 * @param dst - Pointer to location to store 256-bit hash.
 * @return true if hash succesfully written, false if an error occurred.
 */
extern bool rdw_sha256_hash_final_write (volatile RDW_SHA256_T *sha256, void *dst);

/** Finialise hash (add padding and calculate), without writing to memory.
 * @param sha256 - Pointer to SHA256 registers.
 */
static inline void rdw_sha256_hash_final (volatile RDW_SHA256_T *sha256)
{
    sha256->CONTROL = RDW_SHA256_ENABLE | RDW_SHA256_DO_NOT_INIT_HASH | RDW_SHA256_FINALISE_HASH | RDW_SHA256_DO_NOT_WRITE_HASH;

    /* Wait until operation is complete. */
    while(!(sha256->STATUS & (RDW_SHA256_HC | RDW_SHA256_ER)))
        ;
}

/** Write hash to memory.
 * @param sha256 - Pointer to SHA256 registers.
 */
static inline void rdw_sha256_hash_write (volatile RDW_SHA256_T *sha256, void *dst)
{
    sha256->DST_ADDRESS = dst;
    sha256->CONTROL = RDW_SHA256_ENABLE | RDW_SHA256_DO_NOT_INIT_HASH | RDW_SHA256_DO_NOT_FINALISE_HASH | RDW_SHA256_WRITE_HASH;

    /* Wait until operation is complete. */
    while(!(sha256->STATUS & (RDW_SHA256_HC | RDW_SHA256_ER)))
        ;
}

#define SHA_256_BASE				(0x400F3000 + 0x000UL)
#define SHA_256          	  ((volatile RDW_SHA256_T   *) SHA_256_BASE )
    
#endif //COBRA_SHA_EN
#endif //_SHA_256_H

