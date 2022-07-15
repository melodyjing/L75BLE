/******************************************************************************

	Coney Drivers

	File Name	:	cobra4_aes.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	11/16/2018

	Description :   abstraction the hardware, and support function to the user
					and modified from project of Cobra



	Changed Log	:

		Liujian  11/16/2018			- Creation
		Liujian  2019/8/10  		moved from coney


*****************************************************************************/

#ifndef __COBRA4_AES_H__
#define __COBRA4_AES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "cobra_config.h"
#include "ke_mem.h"         // Used to allocate memory environment
#include "co_list.h"        // List of AES execution to perform
#include "co_math.h"        // For rand functions

#if COBRA_AES_EN
#define __bits__ 32

/* Length of different fields */
#define RDW_AES_KEY_BITS                256                                      /**< To support up to 256-bit keys */
#define RDW_AES_DIN_BITS                128                                      /**< Input block is always 128 bits */
#define RDW_AES_DOUT_BITS               128                                      /**< Output block is always 128 bits */
#define RDW_AES_IV_BITS                 128                                      /**< Initial vector block is always 128 bits */
#define RDW_AES_KEY_BYTES               (RDW_AES_KEY_BITS  >> 3)                 /**< As above but in bytes */
#define RDW_AES_DIN_BYTES               (RDW_AES_DIN_BITS  >> 3)                 /**< As above but in bytes */
#define RDW_AES_DOUT_BYTES              (RDW_AES_DOUT_BITS >> 3)                 /**< As above but in bytes */
#define RDW_AES_IV_BYTES                (RDW_AES_IV_BITS   >> 3)                 /**< As above but in bytes */

/* Number of registers depending on 16 / 32 bit architecture  */
#define RDW_AES_KEY_WORDS               (RDW_AES_KEY_BITS  / __bits__)           /**< Number of h/w registers in which to store full key */
#define RDW_AES_DIN_WORDS               (RDW_AES_DIN_BITS  / __bits__)           /**< Number of h/w registers in which to store data in block */
#define RDW_AES_DOUT_WORDS              (RDW_AES_DOUT_BITS / __bits__)           /**< Number of h/w registers in which to present data out block */
#define RDW_AES_IV_WORDS                (RDW_AES_IV_BITS   / __bits__)           /**< Number of h/w registers in which to store the Initial Vector */

/* Modes */
#define RDW_AES_CHAIN_ECB_MODE          0x0                                      /**< ECB chain mode */
#define RDW_AES_CHAIN_CBC_MODE          0x1                                      /**< CBC chain mode */
#define RDW_AES_CHAIN_CTR_MODE          0x2                                      /**< CTR chain mode */
#define RDW_AES_CHAIN_GCM_MODE          0x3                                      /**< GCM chain mode */

#define RDW_AES_AAD_CIPHER_MODE         0x0                                      /**< CIPHER AAD mode */
#define RDW_AES_AAD_DATA_MODE           0x1                                      /**< DATA AAD mode */
#define RDW_AES_AAD_H_MODE              0x2                                      /**< H AAD mode */
#define RDW_AES_AAD_TAG_MODE            0x3                                      /**< TAD AAD mode */

/* Control register bit masks.  */
#define RDW_AES_EN_MASK                 0x0001                                   /**< Block enable control register bitmask */
#define RDW_AES_DMA_MASK                0x0002                                   /**< DMA flow control enable control register bitmask */
#define RDW_AES_MODE_MASK               0x000C                                   /**< Cipher/Decipher/Key_expansion control register bitmask */
#define RDW_AES_SIZE_MASK               0x0030                                   /**< Key size control register bitmask */
#define RDW_AES_SRC_MASK                0x0040                                   /**< Key source control register bitmask */
#define RDW_AES_IE_MASK                 0x0080                                   /**< Interrupt enable control register bitmask */
#define RDW_AES_CHAIN_MASK              0x0F00                                   /**< Chaining control register bitmask */
#define RDW_AES_AAD_MASK                0x3000                                   /**< AAD control register bitmask */

/* Status register bit masks.  */
#define RDW_AES_BUSY_MASK               0x0001                                   /**< Operation status register bitmask */
#define RDW_AES_DONE_MASK               0x0002                                   /**< Done status register bitmask */
/* Control register values.  */
#define RDW_AES_DISABLE                 (0x0 << 0)                               /**< AES disable */
#define RDW_AES_ENABLE                  (0x1 << 0)                               /**< AES enable */
#define RDW_AES_DMA_DISABLE             (0x0 << 1)                               /**< DMA flow control disable */
#define RDW_AES_DMA_ENABLE              (0x1 << 1)                               /**< DMA flow control enable */
#define RDW_AES_CIPHER                  (0x0 << 2)                               /**< Cipher operation */
#define RDW_AES_DECIPHER                (0x1 << 2)                               /**< Decipher operation */
#define RDW_AES_KEY_EXPANSION           (0x2 << 2)                               /**< Key expansion operation */
#define RDW_AES_KEY_128                 (0x0 << 4)                               /**< Key size 128 operation */
#define RDW_AES_KEY_192                 (0x1 << 4)                               /**< Key size 192 operation */
#define RDW_AES_KEY_256                 (0x2 << 4)                               /**< Key size 256 operation */
#define RDW_AES_KEY_SRC_APB             (0x0 << 6)                               /**< Key source is external interface */
#define RDW_AES_KEY_SRC_EXT             (0x1 << 6)                               /**< Key source is APB bus */
#define RDW_AES_INT_DISABLE             (0x0 << 7)                               /**< Interrupt request disable */
#define RDW_AES_INT_ENABLE              (0x1 << 7)                               /**< Interrupt request enable */
#define RDW_AES_CHAIN_ECB               (0x0 << 8)                               /**< ECB chaining mode */
#define RDW_AES_CHAIN_CBC               (0x1 << 8)                               /**< CBC chaining mode */
#define RDW_AES_CHAIN_CTR               (0x2 << 8)                               /**< CTR chaining mode */
#define RDW_AES_CHAIN_GCM               (0x3 << 8)                               /**< GCM chaining mode */
#define RDW_AES_AAD_CIPHER              (0x0 << 12)                              /**< AAD ciper mode */
#define RDW_AES_AAD_DATA                (0x1 << 12)                              /**< AAD data mode */
#define RDW_AES_AAD_H                   (0x2 << 12)                              /**< AAD H mode */
#define RDW_AES_AAD_TAG                 (0x3 << 12)                              /**< AAD Tag mode */

/* Status register values.  */
#define RDW_AES_NOT_BUSY                (0x0 << 0)                               /**< AES engine is idle */
#define RDW_AES_BUSY                    (0x1 << 0)                               /**< AES engine is processing block */
#define RDW_AES_NOT_DONE                (0x0 << 1)                               /**< IRQ not set */
#define RDW_AES_DONE                    (0x1 << 1)                               /**< IRQ set */

/** AES registers.  */
typedef struct rdw_aes_t
{
  __attribute__ ((aligned (4))) unsigned int   control;                        /**< Control fields  */
  __attribute__ ((aligned (4))) unsigned int   status;                         /**< Status fields */
  __attribute__ ((aligned (4))) unsigned int   data_in[RDW_AES_KEY_WORDS];     /**< Data and key input */
  __attribute__ ((aligned (4))) unsigned int   data_out[RDW_AES_DOUT_WORDS];   /**< Processed data output */
  __attribute__ ((aligned (4))) unsigned int   iv[RDW_AES_IV_WORDS];           /**< Initial vector  */
}RDW_AES_T;

/** Function prototypes */

/** Key setup
 * @param aes      - Pointer to the AES registers.
 * @param key      - Pointer to key
 * @param key_size - Key size 128, 192 or 256
 */
extern void
rdw_aes_key_setup(volatile RDW_AES_T *aes, const unsigned char key[], unsigned int key_size);

/** Encrypt main loop
 * @param aes      - Pointer to the AES registers.
 * @param plain    - Pointer to plaintext (zero padded to multiple of 128 bits)
 * @param length   - Length of plaintext in bytes
 * @param cipher   - Pointer to cipertext output
 */
extern void
rdw_aes_encrypt(volatile RDW_AES_T *aes,
                const unsigned char plain[],
                unsigned int length,
                unsigned char cipher[]);

/** Decrypt main loop
 * @param aes      - Pointer to the AES registers.
 * @param cipher   - Pointer to cipertext (zero padded to multiple of 128 bits)
 * @param length   - Length of ciphertext in bytes
 * @param plain    - Pointer to plaintext output
 */
extern void
rdw_aes_decrypt(volatile RDW_AES_T *aes,
                const unsigned char cipher[],
                unsigned int length,
                unsigned char plain[]);

/** Encrypt using ECB
 * @param aes      - Pointer to the AES registers.
 * @param plain    - Pointer to plaintext (zero padded to multiple of 128 bits)
 * @param length   - Length of plaintext in bytes
 * @param cipher   - Pointer to cipertext output
 */
extern void
rdw_aes_encrypt_ecb(volatile RDW_AES_T *aes,
                    const unsigned char plain[],
                    unsigned int length,
                    unsigned char cipher[]);

/** Decrypt using ECB
 * @param aes      - Pointer to the AES registers.
 * @param cipher   - Pointer to cipertext (zero padded to multiple of 128 bits)
 * @param length   - Length of ciphertext in bytes
 * @param plain    - Pointer to plaintext output
 */
extern void
rdw_aes_decrypt_ecb(volatile RDW_AES_T *aes,
                    const unsigned char cipher[],
                    unsigned int length,
                    unsigned char plain[]);

/** Encrypt using CBC
 * @param aes      - Pointer to the AES registers.
 * @param iv       - Pointer to initialisation vector
 * @param plain    - Pointer to plaintext (zero padded to multiple of 128 bits)
 * @param length   - Length of plaintext in bytes
 * @param cipher   - Pointer to cipertext output
 */
extern void
rdw_aes_encrypt_cbc(volatile RDW_AES_T *aes,
                    const unsigned char iv[],
                    const unsigned char plain[],
                    unsigned int length,
                    unsigned char cipher[]);

/** Decrypt using CBC
 * @param aes      - Pointer to the AES registers.
 * @param iv       - Pointer to initialisation vector
 * @param cipher   - Pointer to cipertext (zero padded to multiple of 128 bits)
 * @param length   - Length of ciphertext in bytes
 * @param plain    - Pointer to plaintext output
 */
extern void
rdw_aes_decrypt_cbc(volatile RDW_AES_T *aes,
                    const unsigned char iv[],
                    const unsigned char cipher[],
                    unsigned int length,
                    unsigned char plain[]);

/** Encrypt using CTR
 * @param aes      - Pointer to the AES registers.
 * @param iv       - Pointer to initialisation vector
 * @param plain    - Pointer to plaintext (zero padded to multiple of 128 bits)
 * @param length   - Length of plaintext in bytes
 * @param cipher   - Pointer to cipertext output
 */
extern void
rdw_aes_encrypt_ctr(volatile RDW_AES_T *aes,
                    const unsigned char iv[],
                    const unsigned char plain[],
                    unsigned int length,
                    unsigned char cipher[]);

/** Decrypt using CTR
 * @param aes      - Pointer to the AES registers.
 * @param iv       - Pointer to initialisation vector
 * @param cipher   - Pointer to cipertext (zero padded to multiple of 128 bits)
 * @param length   - Length of ciphertext in bytes
 * @param plain    - Pointer to plaintext output
 */
extern void
rdw_aes_decrypt_ctr(volatile RDW_AES_T *aes,
                    const unsigned char iv[],
                    const unsigned char cipher[],
                    unsigned int length,
                    unsigned char plain[]);

/** Encrypt using GCM
 * @param aes       - Pointer to the AES registers.
 * @param iv        - Pointer to initialisation vector
 * @param aad       - Pointer to AAD
 * @param plain     - Pointer to plaintext (zero padded to multiple of 128 bits)
 * @param lenA      - length of AAD in bytes
 * @param lenC      - Length of plaintext in bytes
 * @param cipher    - Pointer to cipertext output
 * @param tag       - Pointer to tag
 */
extern void
rdw_aes_encrypt_gcm(volatile RDW_AES_T *aes,
                    const unsigned char iv[],
                    const unsigned char aad[],
                    const unsigned char plain[],
                    unsigned int lenA,
                    unsigned int lenC,
                    unsigned char cipher[],
                    unsigned char tag[]);

/** Decrypt using GCM
 * @param aes       - Pointer to the AES registers.
 * @param iv        - Pointer to initialisation vector
 * @param aad       - Pointer to AAD
 * @param cipher    - Pointer to ciphertext (zero padded to multiple of 128 bits)
 * @param lenA      - length of AAD in bytes
 * @param lenC      - Length of ciphertext in bytes
 * @param plain     - Pointer to plaintext output
 * @param tag       - Pointer to tag
 */
extern void
rdw_aes_decrypt_gcm(volatile RDW_AES_T *aes,
                    const unsigned char iv[],
                    const unsigned char aad[],
                    const unsigned char cipher[],
                    unsigned int lenA,
                    unsigned int lenC,
                    unsigned char plain[],
                    unsigned char tag[]);

/** Write a 96-bit ICB for GCM
 * @param aes  - Pointer to the AES registers.
 * @param iv   - 96-bit IV vector
 */
extern void
rdw_aes_write_icb_gcm (volatile RDW_AES_T *aes, const unsigned char iv[]);

/** Write a 96-bit j0 for GCM
 * @param aes  - Pointer to the AES registers.
 * @param iv  - 96-bit IV vector
 */
extern void
rdw_aes_write_j0_gcm (volatile RDW_AES_T *aes, const unsigned char iv[]);

/** Write all zeros to the data_in registers
 * @param aes  - Pointer to the AES registers.
 */
extern void
rdw_aes_write_all_zero_data_in (volatile RDW_AES_T *aes);

/** Write the two 64-bit length vectors for GCM
 * @param aes   - Pointer to the AES registers.
 * @param lenA  - Length of AAD in bytes
 * @param lenC  - Length of plain or cipher text in bytes
 */
extern void
rdw_aes_write_length_data_in (volatile RDW_AES_T *aes, unsigned int lenA, unsigned int lenC);

/** Write 128-bit data in - safe for data[] stored on either big or little endian machine
 * @param aes - Pointer to the AES registers
 * @param data - Pointer to the input data
 */
extern void
rdw_aes_write_data_in (volatile RDW_AES_T *aes, const unsigned char data[]);

/** Read 128-bit data out - safe for data[] stored on either big or little endian machine
 * @param aes - Pointer to the AES registers
 * @param data - Pointer to the output data
 */
extern void
rdw_aes_read_data_out (volatile RDW_AES_T *aes, unsigned char data[]);

/** Write 128/192/256-bit key - safe for key[] stored on either big or little endian machine
 * @param aes - Pointer to the AES registers
 * @param key - Pointer to the key data
 * @param key_size - 128/192/256
 */
extern void
rdw_aes_write_key (volatile RDW_AES_T *aes, const unsigned char key[], unsigned int key_size);

/** Write IV in using aligned access - safe for iv[] stored on either big or little endian machine
 * @param aes - Pointer to the AES registers
 * @param iv - Pointer to the 128-bit IV
 */
extern void
rdw_aes_write_iv (volatile RDW_AES_T *aes, const unsigned char iv[]);

/** Select chaining mode
 * @param aes - Pointer to the AES registers
 * @param chain_mode - chain mode
 */
extern void
rdw_aes_chain_mode (volatile RDW_AES_T *aes, unsigned chain_mode);

/** Select AAD mode
 * @param aes - Pointer to the AES registers
 * @param aad_mode - aad mode
 */
extern void
rdw_aes_aad_mode (volatile RDW_AES_T *aes, unsigned aad_mode);

/** Wait until AES core has processed the block
 * Overload to model operation in software or map to rdw_aes_wait_complete() for real hardware
 * @param aes  - Pointer to the AES registers.
 */
extern void
rdw_aes_wait (volatile RDW_AES_T *aes);

/** Enable AES
 * This must be called to enable any operation in the block
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_enable (volatile RDW_AES_T *aes)
{
  /* Set the enable flag.  */
  aes->control |= RDW_AES_ENABLE;
}

/** Disable AES
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_disable (volatile RDW_AES_T *aes)
{
  /* Clear the enable flag.  */
  aes->control &= ~RDW_AES_ENABLE;
}

/** Enable DMA flow control
 * This must be called to enable any communication between the block and DMA engine
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_dma_enable (volatile RDW_AES_T *aes)
{
  /* Set the dma enable flag.  */
  aes->control |= RDW_AES_DMA_ENABLE;
}

/** Disable DMA flow control
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_dma_disable (volatile RDW_AES_T *aes)
{
  /* Clear the dma enable flag.  */
  aes->control &= ~RDW_AES_DMA_ENABLE;
}

/** Set operation mode to Cipher
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_cipher (volatile RDW_AES_T *aes)
{
  /* Set mode to cipher.  */
  aes->control = (aes->control & ~RDW_AES_MODE_MASK) | RDW_AES_CIPHER;
}

/** Set operation mode to Decipher
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_decipher (volatile RDW_AES_T *aes)
{
  /* Set mode to decipher.  */
  aes->control = (aes->control & ~RDW_AES_MODE_MASK) | RDW_AES_DECIPHER;
}

/** Set operation mode to Key expansion
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_key_expansion (volatile RDW_AES_T *aes)
{
  /* Set mode to key expansion.  */
  aes->control = (aes->control & ~RDW_AES_MODE_MASK) | RDW_AES_KEY_EXPANSION;
}

/** Set Key size to 128
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_key_size_128 (volatile RDW_AES_T *aes)
{
  /* Set key to 128 bits.  */
  aes->control = (aes->control & ~RDW_AES_SIZE_MASK) | RDW_AES_KEY_128;
}

/** Set Key size to 192
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_key_size_192 (volatile RDW_AES_T *aes)
{
  /* Set key to 192 bits.  */
  aes->control = (aes->control & ~RDW_AES_SIZE_MASK) | RDW_AES_KEY_192;
}

/** Set Key size to 256
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_key_size_256 (volatile RDW_AES_T *aes)
{
  /* Set key to 256 bits.  */
  aes->control = (aes->control & ~RDW_AES_SIZE_MASK) | RDW_AES_KEY_256;
}

/** Set Key size to ksize param value
 * @param aes   - Pointer to the AES registers
 * @param ksize - key size. One of 128, 192, 256
 */
static inline void
rdw_aes_key_size (volatile RDW_AES_T *aes, unsigned int ksize)
{
  /* Set key to ksize bits.  */
  switch (ksize)
    {
    case 128:
      rdw_aes_key_size_128 (aes);
      break;
    case 192:
      rdw_aes_key_size_192 (aes);
      break;
    case 256:
      rdw_aes_key_size_256 (aes);
      break;
    default:
      ;
    }
}

/** Set key source to internal APB
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_key_src_apb (volatile RDW_AES_T *aes)
{
  /* Key source is data_in registers written over APB interface */
  aes->control = (aes->control & ~RDW_AES_SRC_MASK) | RDW_AES_KEY_SRC_APB;
}

/** Set key source to external interface
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_key_src_ext (volatile RDW_AES_T *aes)
{
  /* Key source is the key_ext hardware port */
  aes->control = (aes->control & ~RDW_AES_SRC_MASK) | RDW_AES_KEY_SRC_EXT;
}

/** Enable Interrupt request
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_int_enable (volatile RDW_AES_T *aes)
{
  /* Set the interrupt enable flag.  */
  aes->control |= RDW_AES_INT_ENABLE;
}

/** Disable Interrupt request
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_int_disable (volatile RDW_AES_T *aes)
{
  /* Clear the interrupt enable flag.  */
  aes->control &= ~RDW_AES_INT_ENABLE;
}

/** Returns the AES busy status flag
 * @param aes     - Pointer to the AES registers
 * @return int - true if busy
 */
static inline int
rdw_aes_is_busy (volatile RDW_AES_T *aes)
{
  /* Return status.busy flag  */
  return ((aes->status & RDW_AES_BUSY_MASK) == RDW_AES_BUSY);
}

/** Returns status.done flag
 * @param aes - Pointer to the AES registers
 */
static inline int
rdw_aes_is_done (volatile RDW_AES_T *aes)
{
  return ((aes->status & RDW_AES_DONE_MASK) == RDW_AES_DONE);
}

/** Clear status.done flag
 * Clears done flag. Used to de-assert the IRQ from AES peripheral
 * @param aes - Pointer to the AES registers
 */
static inline void
rdw_aes_clear_int (volatile RDW_AES_T *aes)
{
  aes->status = RDW_AES_DONE;
}

/** Write a new 128-bit key to be expanded. Writing to last word of data_in will automatically trigger AES start
 * @param aes  - Pointer to the AES registers.
 * @param key  - 128-bit key vector
 */
static inline void
rdw_aes_write_key128 (volatile RDW_AES_T *aes, const unsigned char key[])
{
  rdw_aes_write_key (aes, key, 128);
}

/** Write a new 192-bit key to be expanded. Writing to last word of data_in will automatically trigger AES start
 * @param aes  - Pointer to the AES registers.
 * @param key  - 192-bit key vector
 */
static inline void
rdw_aes_write_key192 (volatile RDW_AES_T *aes, const unsigned char key[])
{
  rdw_aes_write_key (aes, key, 192);
}

/** Write a new 256-bit key to be expanded. Writing to last word of data_in will automatically trigger AES start
 * @param aes  - Pointer to the AES registers.
 * @param key  - 256-bit key vector
 */
static inline void
rdw_aes_write_key256 (volatile RDW_AES_T *aes, const unsigned char key[])
{
  rdw_aes_write_key (aes, key, 256);
}

/** Wait until AES core has processed the block
 * @param aes  - Pointer to the AES registers.
 */
static inline void
rdw_aes_wait_complete (volatile RDW_AES_T *aes)
{
  while (rdw_aes_is_busy(aes));
}

typedef enum {
	ENCRYPT_ECB_MODE,
	ENCRYPT_CBC_MODE,
	ENCRYPT_CTR_MODE,
	ENCRYPT_GCM_MODE,
	ENCRYPT_GCM_MODE_1,
	DECRYPT_ECB_MODE,
	DECRYPT_CBC_MODE,
	DECRYPT_CTR_MODE,
	DECRYPT_GCM_MODE
}AES_MODE;

typedef enum {
	CIPHER_DATA,
	AAD_DATA,
	H_CALC,
	TAG
}AAD_MODE;

typedef struct {
	AES_MODE mode;
	AAD_MODE aad_mode;
}aes_mode_param;

typedef enum{
	KEY_SIZE_128,
	KEY_SIZE_192,
	KEY_SIZE_256
}aes_key_size;


struct aes_func_env;


/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle result of an AES based algorithm
 *
 * @param[in] status       Execution status
 * @param[in] aes_res      16 bytes block result
 * @param[in] src_info     Information provided by requester
 ****************************************************************************************
 */
typedef void (*aes_func_result_cb) (uint8_t status, const uint8_t* aes_res, uint32_t src_info);

/**
 ****************************************************************************************
 * Callback used to continue execution of the AES based function.
 *
 * @param[in] aes_result Result of the AES (aes(key , val))
 *
 * @return True if function execution is over
 ****************************************************************************************
 */
typedef  bool (*aes_func_continue_cb) (struct aes_func_env* aes_env, uint8_t* aes_result);

/// Environment variable required for an AES based function
/// This structure must be Header of all function environment variables
struct aes_func_env
{
    /// used to put AES function in the AES execution queue
    struct co_list_hdr    hdr;
    /// AES continue callback
    aes_func_continue_cb  aes_continue_cb;
    /// AES End callback
    aes_func_result_cb    aes_res_cb;

    /// Key to use for the AES execution
    const uint8_t*       key;

    aes_key_size				key_size;

    uint8_t				mode;

    /// Value to use for AES Cypher/Decypher
    const uint8_t*       val;

    const uint8_t*       iv;
    /// Information put in source id message to retrieve requester
    uint32_t             src_info;
};



typedef struct {
	aes_mode_param  mode;
	unsigned char * data_in;
	unsigned short  data_in_lenght;
	unsigned char * iv_in;

	unsigned char    key_size;
	unsigned char  * key_in;





}aes_param;


#define AES_BASE       (0x400C0000UL + 0x0000UL)
void cobra4_aes_crypt_evt_handler(void);
void cobra4_aes_init(void);
void cobra4_aes_interrupt_function(void);
void cobra4_aes_encrypt_decryption(const uint8_t* key, aes_key_size key_size, AES_MODE mode, const uint8_t *val,  const uint8_t *iv, aes_func_result_cb res_cb, uint32_t src_info);

void app_cobra4_aes_event_callback_init(void);

#endif

#endif

