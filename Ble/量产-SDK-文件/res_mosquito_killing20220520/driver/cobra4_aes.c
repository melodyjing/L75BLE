/******************************************************************************

	Coney Drivers

	File Name	:	cobra4_aes.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	11/16/2018

	Description :   abstraction the hardware, and support function to the user
					and modified from project of Cobra



	Changed Log	:

		Liujian  11/16/2018			- Creation
		Liujian  2019/8/10     moved from coney



*****************************************************************************/


#include <stdio.h>

#include "cobra_uart.h"
#include "rwip_config.h"
#include "cobra4_aes.h"
#include "ke_event.h"
#include "rdw_mcu.h"
#if COBRA_AES_EN

#ifdef COBRA_ASE_NEW
/** Key setup
 * @param aes      - Pointer to the AES registers.
 * @param key      - Pointer to key
 * @param key_size - Key size 128, 192 or 256
 */
void
rdw_aes_key_setup(volatile RDW_AES_T *aes, const unsigned char key[], unsigned int key_size)
{
    /* Configure AES key size in control.size field */
    rdw_aes_key_size(aes, key_size);
    /* Configure AES to work in key expansion mode */
    rdw_aes_key_expansion(aes);
    /* Write key value. A write to the last data_in location will trigger an AES start */
    rdw_aes_write_key(aes, key, key_size);
    /* Wait */
    rdw_aes_wait(aes);
}

/** Encrypt main loop
 * @param aes      - Pointer to the AES registers.
 * @param plain    - Pointer to plaintext (zero padded to multiple of 128 bits)
 * @param length   - Length of plaintext in bytes
 * @param cipher   - Pointer to cipertext output
 */
void
rdw_aes_encrypt(volatile RDW_AES_T *aes, const unsigned char plain[], unsigned int length, unsigned char cipher[])
{
    unsigned int i;
    /* Change mode to Encipher */
    rdw_aes_cipher(aes);

    for (i = 0; i < length; i += 128/8)
    {
        /* Write plaintext. A write to the last data_in location will trigger an AES start */
        rdw_aes_write_data_in(aes, plain);
        /* Wait */
        rdw_aes_wait(aes);
        /* Read ciphertext */
        rdw_aes_read_data_out(aes, cipher);
        /* Move pointers */
        plain += (128/8);
        cipher += (128/8);
    }
}

/** Decrypt main loop
 * @param aes      - Pointer to the AES registers.
 * @param cipher   - Pointer to cipertext (zero padded to multiple of 128 bits)
 * @param length   - Length of ciphertext in bytes
 * @param plain    - Pointer to plaintext output
 */
void
rdw_aes_decrypt(volatile RDW_AES_T *aes, const unsigned char cipher[], unsigned int length, unsigned char plain[])
{
    unsigned int i;
    /* Change mode to Decipher */
    rdw_aes_decipher(aes);

    for (i = 0; i < length; i += 128/8)
    {
        /* Write ciphertext. A write to the last data_in location will trigger an AES start */
        rdw_aes_write_data_in(aes, cipher);
        /* Wait */
        rdw_aes_wait(aes);
        /* Read plaintext */
        rdw_aes_read_data_out(aes, plain);
        /* Move pointers */
        plain += (128/8);
        cipher += (128/8);
    }
}
/** Encrypt using ECB
 * @param aes      - Pointer to the AES registers.
 * @param plain    - Pointer to plaintext (zero padded to multiple of 128 bits)
 * @param length   - Length of plaintext in bytes
 * @param cipher   - Pointer to cipertext output
 */
void
rdw_aes_encrypt_ecb(volatile RDW_AES_T *aes, const unsigned char plain[], unsigned int length, unsigned char cipher[])
{
    /* Chain mode ECB */
    rdw_aes_chain_mode(aes, RDW_AES_CHAIN_ECB_MODE);
    /* Encrypt */
    rdw_aes_encrypt(aes, plain, length, cipher);
}
/** Decrypt using ECB
 * @param aes      - Pointer to the AES registers.
 * @param cipher   - Pointer to cipertext (zero padded to multiple of 128 bits)
 * @param length   - Length of ciphertext in bytes
 * @param plain    - Pointer to plaintext output
 */
void
rdw_aes_decrypt_ecb(volatile RDW_AES_T *aes, const unsigned char cipher[], unsigned int length, unsigned char plain[])
{
    /* Chain mode ECB */
    rdw_aes_chain_mode(aes, RDW_AES_CHAIN_ECB_MODE);
    /* Decrypt */
    rdw_aes_decrypt(aes, cipher, length, plain);
}
/** Encrypt using CBC
 * @param aes      - Pointer to the AES registers.
 * @param iv       - Pointer to initialisation vector
 * @param plain    - Pointer to plaintext (zero padded to multiple of 128 bits)
 * @param length   - Length of plaintext in bytes
 * @param cipher   - Pointer to cipertext output
 */
void
rdw_aes_encrypt_cbc(volatile RDW_AES_T *aes,
                    const unsigned char iv[],
                    const unsigned char plain[],
                    unsigned int length,
                    unsigned char cipher[])
{
    /* Chain mode CBC */
    rdw_aes_chain_mode(aes, RDW_AES_CHAIN_CBC_MODE);
    /* Write IV vector */
    rdw_aes_write_iv(aes, iv);
    /* Encrypt */
    rdw_aes_encrypt(aes, plain, length, cipher);
}
/** Decrypt using CBC
 * @param aes      - Pointer to the AES registers.
 * @param iv       - Pointer to initialisation vector
 * @param cipher   - Pointer to cipertext (zero padded to multiple of 128 bits)
 * @param length   - Length of ciphertext in bytes
 * @param plain    - Pointer to plaintext output
 */
void
rdw_aes_decrypt_cbc(volatile RDW_AES_T *aes, const unsigned char iv[], const unsigned char cipher[], unsigned int length, unsigned char plain[])
{
    /* Chain mode CBC */
    rdw_aes_chain_mode(aes, RDW_AES_CHAIN_CBC_MODE);
    /* Write IV vector */
    rdw_aes_write_iv(aes, iv);
    /* Decrypt */
    rdw_aes_decrypt(aes, cipher, length, plain);
}
/** Encrypt using CTR
 * @param aes      - Pointer to the AES registers.
 * @param iv       - Pointer to initialisation vector
 * @param plain    - Pointer to plaintext (zero padded to multiple of 128 bits)
 * @param length   - Length of plaintext in bytes
 * @param cipher   - Pointer to cipertext output
 */
void
rdw_aes_encrypt_ctr(volatile RDW_AES_T *aes, const unsigned char iv[], const unsigned char plain[], unsigned int length, unsigned char cipher[])
{
    /* Chain mode CTR */
    rdw_aes_chain_mode(aes, RDW_AES_CHAIN_CTR_MODE);
    /* Write IV vector */
    rdw_aes_write_iv(aes, iv);
    /* Encrypt */
    rdw_aes_encrypt(aes, plain, length, cipher);
}
/** Decrypt using CTR
 * @param aes      - Pointer to the AES registers.
 * @param iv       - Pointer to initialisation vector
 * @param cipher   - Pointer to cipertext (zero padded to multiple of 128 bits)
 * @param length   - Length of ciphertext in bytes
 * @param plain    - Pointer to plaintext output
 */
void
rdw_aes_decrypt_ctr(volatile RDW_AES_T *aes, const unsigned char iv[], const unsigned char cipher[], unsigned int length, unsigned char plain[])
{
    /* Chain mode CTR */
    rdw_aes_chain_mode(aes, RDW_AES_CHAIN_CTR_MODE);
    /* Write IV vector */
    rdw_aes_write_iv(aes, iv);
    /* Decrypt */
    rdw_aes_decrypt(aes, cipher, length, plain);
}
#endif

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
void
rdw_aes_encrypt_gcm(volatile RDW_AES_T *aes,
                    const unsigned char iv[],
                    const unsigned char aad[],
                    const unsigned char plain[],
                    unsigned int lenA,
                    unsigned int lenC,
                    unsigned char cipher[],
                    unsigned char tag[])
{
    unsigned int i;
    /* Chain mode GCM */
    rdw_aes_chain_mode(aes, RDW_AES_CHAIN_GCM_MODE);
    /* Write ICB vector */
    rdw_aes_write_icb_gcm(aes, iv);
    // Configure AES in Cipher mode
    rdw_aes_cipher(aes);
    /* Generate H */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_H_MODE);
    /* Write all zeros */
    rdw_aes_write_all_zero_data_in(aes);
    /* Wait */
    rdw_aes_wait(aes);
    /* Load AAD */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_DATA_MODE);
    for (i = 0; i < lenA; i += 128/8)
    {
        rdw_aes_write_data_in(aes, aad);
        /* Wait */
        rdw_aes_wait(aes);
        /* Move pointers */
        aad += (128/8);
    }
    /* Load plaintext */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_CIPHER_MODE);
    for (i = 0; i < lenC; i += 128/8)
    {
        rdw_aes_write_data_in(aes, plain);
        /* Wait */
        rdw_aes_wait(aes);
        /* Read ciphertext */
        rdw_aes_read_data_out(aes, cipher);
        /* Move pointers */
        plain += (128/8);
        cipher += (128/8);
    }
    /* Load length */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_DATA_MODE);
    /* Write lengths */
    rdw_aes_write_length_data_in(aes, lenA, lenC);
    /* Wait */
    rdw_aes_wait(aes);
    /* Generate Tag */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_TAG_MODE);
    /* Write J0 */
    rdw_aes_write_j0_gcm(aes, iv);
    /* Wait */
    rdw_aes_wait(aes);
    /* Read tag */
    rdw_aes_read_data_out(aes, tag);
}
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
void
rdw_aes_decrypt_gcm(volatile RDW_AES_T *aes,
                    const unsigned char iv[],
                    const unsigned char aad[],
                    const unsigned char cipher[],
                    unsigned int lenA,
                    unsigned int lenC,
                    unsigned char plain[],
                    unsigned char tag[])
{
    unsigned int i;
    /* Chain mode GCM */
    rdw_aes_chain_mode(aes, RDW_AES_CHAIN_GCM_MODE);
    /* Write ICB vector */
    rdw_aes_write_icb_gcm(aes, iv);
    // Configure AES in Decipher mode
    rdw_aes_decipher(aes);
    /* Generate H */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_H_MODE);
    /* Write all zeros */
    rdw_aes_write_all_zero_data_in(aes);
    /* Wait */
    rdw_aes_wait(aes);
    /* Load AAD */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_DATA_MODE);
    for (i = 0; i < lenA; i += 128/8)
    {
        rdw_aes_write_data_in(aes, aad);
        /* Wait */
        rdw_aes_wait(aes);
        /* Move pointers */
        aad += (128/8);
    }
    /* Load ciphertext */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_CIPHER_MODE);
    for (i = 0; i < lenC; i += 128/8)
    {
        rdw_aes_write_data_in(aes, cipher);
        /* Wait */
        rdw_aes_wait(aes);
        /* Read plaintext */
        rdw_aes_read_data_out(aes, plain);
        /* Move pointers */
        plain += (128/8);
        cipher += (128/8);
    }
    /* Load length */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_DATA_MODE);
    /* Write lengths */
    rdw_aes_write_length_data_in(aes, lenA, lenC);
    /* Wait */
    rdw_aes_wait(aes);
    /* Generate Tag */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_TAG_MODE);
    /* Write J0 */
    rdw_aes_write_j0_gcm(aes, iv);
    /* Wait */
    rdw_aes_wait(aes);
    /* Read tag */
    rdw_aes_read_data_out(aes, tag);
}
/** Write a 96-bit ICB for GCM
 * @param aes  - Pointer to the AES registers.
 * @param iv   - 96-bit IV vector
 */
void
rdw_aes_write_icb_gcm (volatile RDW_AES_T *aes, const unsigned char iv[])
{
  unsigned char temp[128/8];
  unsigned int i;
  for (i = 0; i < 96/8; i++)
  {
      temp[i] = iv[i];
  }
  temp[12] = 0x00;
  temp[13] = 0x00;
  temp[14] = 0x00;
  temp[15] = 0x02;
  rdw_aes_write_iv(aes, temp);
}
/** Write a 96-bit j0 for GCM
 * @param aes  - Pointer to the AES registers.
 * @param iv  - 96-bit IV vector
 */
void
rdw_aes_write_j0_gcm (volatile RDW_AES_T *aes, const unsigned char iv[])
{
  unsigned char temp[128/8];
  unsigned int i;
  for (i = 0; i < 96/8; i++)
  {
      temp[i] = iv[i];
  }
  temp[12] = 0x00;
  temp[13] = 0x00;
  temp[14] = 0x00;
  temp[15] = 0x01;
  rdw_aes_write_data_in(aes, temp);
}
/** Write all zeros to the data_in registers
 * @param aes  - Pointer to the AES registers.
 */
void
rdw_aes_write_all_zero_data_in (volatile RDW_AES_T *aes)
{
  unsigned char temp[128/8];
  unsigned int i;
  for (i = 0; i < 128/8; i++)
  {
    temp[i] = 0;
  }
  rdw_aes_write_data_in(aes, temp);
}
/** Write the two 64-bit length vectors for GCM
 * @param aes   - Pointer to the AES registers.
 * @param lenA  - Length of AAD in bytes
 * @param lenC  - Length of plain or cipher text in bytes
 */
void
rdw_aes_write_length_data_in (volatile RDW_AES_T *aes, unsigned int lenA, unsigned int lenC)
{
  unsigned long long lenAA, lenCC;
  lenAA = (unsigned long long)lenA * 8;
  lenCC = (unsigned long long)lenC * 8;
#if (__bits__ == 32)
  aes->data_in[0] = (((lenAA >> 32) & 0xFF) << 24) | (((lenAA >> 40) & 0xFF) << 16) | (((lenAA >> 48) & 0xFF) << 8) | (((lenAA >> 56) & 0xFF) << 0);
  aes->data_in[1] = (((lenAA >>  0) & 0xFF) << 24) | (((lenAA >>  8) & 0xFF) << 16) | (((lenAA >> 16) & 0xFF) << 8) | (((lenAA >> 24) & 0xFF) << 0);
  aes->data_in[2] = (((lenCC >> 32) & 0xFF) << 24) | (((lenCC >> 40) & 0xFF) << 16) | (((lenCC >> 48) & 0xFF) << 8) | (((lenCC >> 56) & 0xFF) << 0);
  aes->data_in[3] = (((lenCC >>  0) & 0xFF) << 24) | (((lenCC >>  8) & 0xFF) << 16) | (((lenCC >> 16) & 0xFF) << 8) | (((lenCC >> 24) & 0xFF) << 0);
#else
  aes->data_in[0] = (((lenAA >> 48) & 0xFF) << 8) | (((lenAA >> 56) & 0xFF) << 0);
  aes->data_in[1] = (((lenAA >> 32) & 0xFF) << 8) | (((lenAA >> 40) & 0xFF) << 0);
  aes->data_in[2] = (((lenAA >> 16) & 0xFF) << 8) | (((lenAA >> 24) & 0xFF) << 0);
  aes->data_in[3] = (((lenAA >>  0) & 0xFF) << 8) | (((lenAA >>  8) & 0xFF) << 0);
  aes->data_in[4] = (((lenCC >> 48) & 0xFF) << 8) | (((lenCC >> 56) & 0xFF) << 0);
  aes->data_in[5] = (((lenCC >> 32) & 0xFF) << 8) | (((lenCC >> 40) & 0xFF) << 0);
  aes->data_in[6] = (((lenCC >> 16) & 0xFF) << 8) | (((lenCC >> 24) & 0xFF) << 0);
  aes->data_in[7] = (((lenCC >>  0) & 0xFF) << 8) | (((lenCC >>  8) & 0xFF) << 0);
#endif
}
/** Write 128-bit data in - safe for data[] stored on either big or little endian machine
 * @param aes - Pointer to the AES registers
 * @param data - Pointer to the input data
 */
void
rdw_aes_write_data_in (volatile RDW_AES_T *aes, const unsigned char data[])
{
#if (__bits__ == 32)
  aes->data_in[0] = data[ 0] | (data[ 1] << 8) | (data[ 2] << 16) | (data[ 3] << 24);
  aes->data_in[1] = data[ 4] | (data[ 5] << 8) | (data[ 6] << 16) | (data[ 7] << 24);
  aes->data_in[2] = data[ 8] | (data[ 9] << 8) | (data[10] << 16) | (data[11] << 24);
  aes->data_in[3] = data[12] | (data[13] << 8) | (data[14] << 16) | (data[15] << 24);
#else
  aes->data_in[0] = data[ 0] | (data[ 1] << 8);
  aes->data_in[1] = data[ 2] | (data[ 3] << 8);
  aes->data_in[2] = data[ 4] | (data[ 5] << 8);
  aes->data_in[3] = data[ 6] | (data[ 7] << 8);
  aes->data_in[4] = data[ 8] | (data[ 9] << 8);
  aes->data_in[5] = data[10] | (data[11] << 8);
  aes->data_in[6] = data[12] | (data[13] << 8);
  aes->data_in[7] = data[14] | (data[15] << 8);
#endif
}
/** Read 128-bit data out - safe for data[] stored on either big or little endian machine
 * @param aes - Pointer to the AES registers
 * @param data - Pointer to the output data
 */
void
rdw_aes_read_data_out (volatile RDW_AES_T *aes, unsigned char data[])
{
  unsigned int i;
  unsigned int temp;
#if (__bits__ == 32)
  for (i = 0; i < (128 / 32); i++)
  {
    temp = aes->data_out[i];
    *data++ = temp       & 0xff;
    *data++ = temp >>  8 & 0xff;
    *data++ = temp >> 16 & 0xff;
    *data++ = temp >> 24 & 0xff;
  }
#else
  for (i = 0; i < (128 / 16); i++)
  {
    temp = aes->data_out[i];
    *data++ = temp      & 0xff;
    *data++ = temp >> 8 & 0xff;
  }
#endif
}
/** Write 128/192/256-bit key - safe for key[] stored on either big or little endian machine
 * @param aes - Pointer to the AES registers
 * @param key - Pointer to the key data
 * @param key_size - 128/192/256
 */
void
rdw_aes_write_key (volatile RDW_AES_T *aes, const unsigned char key[], unsigned int key_size)
{
#if (__bits__ == 32)
  aes->data_in[0] = key[ 0] | (key[ 1] << 8) | (key[ 2] << 16) | (key[ 3] << 24);
  aes->data_in[1] = key[ 4] | (key[ 5] << 8) | (key[ 6] << 16) | (key[ 7] << 24);
  aes->data_in[2] = key[ 8] | (key[ 9] << 8) | (key[10] << 16) | (key[11] << 24);
  aes->data_in[3] = key[12] | (key[13] << 8) | (key[14] << 16) | (key[15] << 24);
  if (key_size > 128)
  {
    aes->data_in[4] = key[16] | (key[17] << 8) | (key[18] << 16) | (key[19] << 24);
    aes->data_in[5] = key[20] | (key[21] << 8) | (key[22] << 16) | (key[23] << 24);
    if (key_size > 192)
    {
      aes->data_in[6] = key[24] | (key[25] << 8) | (key[26] << 16) | (key[27] << 24);
      aes->data_in[7] = key[28] | (key[29] << 8) | (key[30] << 16) | (key[31] << 24);
    }
  }
#else
  aes->data_in[0] = key[ 0] | (key[ 1] << 8);
  aes->data_in[1] = key[ 2] | (key[ 3] << 8);
  aes->data_in[2] = key[ 4] | (key[ 5] << 8);
  aes->data_in[3] = key[ 6] | (key[ 7] << 8);
  aes->data_in[4] = key[ 8] | (key[ 9] << 8);
  aes->data_in[5] = key[10] | (key[11] << 8);
  aes->data_in[6] = key[12] | (key[13] << 8);
  aes->data_in[7] = key[14] | (key[15] << 8);
  if (key_size > 128)
  {
    aes->data_in[ 8] = key[16] | (key[17] << 8);
    aes->data_in[ 9] = key[18] | (key[19] << 8);
    aes->data_in[10] = key[20] | (key[21] << 8);
    aes->data_in[11] = key[22] | (key[23] << 8);
    if (key_size > 192)
    {
      aes->data_in[12] = key[24] | (key[25] << 8);
      aes->data_in[13] = key[26] | (key[27] << 8);
      aes->data_in[14] = key[28] | (key[29] << 8);
      aes->data_in[15] = key[30] | (key[31] << 8);
    }
  }
#endif
}
/** Write IV in using aligned access - safe for iv[] stored on either big or little endian machine
 * @param aes - Pointer to the AES registers
 * @param iv - Pointer to the 128-bit IV
 */
void
rdw_aes_write_iv (volatile RDW_AES_T *aes, const unsigned char iv[])
{
#if (__bits__ == 32)
  aes->iv[0] = iv[ 0] | (iv[ 1] << 8) | (iv[ 2] << 16) | (iv[ 3] << 24);
  aes->iv[1] = iv[ 4] | (iv[ 5] << 8) | (iv[ 6] << 16) | (iv[ 7] << 24);
  aes->iv[2] = iv[ 8] | (iv[ 9] << 8) | (iv[10] << 16) | (iv[11] << 24);
  aes->iv[3] = iv[12] | (iv[13] << 8) | (iv[14] << 16) | (iv[15] << 24);
#else
  aes->iv[0] = iv[ 0] | (iv[ 1] << 8);
  aes->iv[1] = iv[ 2] | (iv[ 3] << 8);
  aes->iv[2] = iv[ 4] | (iv[ 5] << 8);
  aes->iv[3] = iv[ 6] | (iv[ 7] << 8);
  aes->iv[4] = iv[ 8] | (iv[ 9] << 8);
  aes->iv[5] = iv[10] | (iv[11] << 8);
  aes->iv[6] = iv[12] | (iv[13] << 8);
  aes->iv[7] = iv[14] | (iv[15] << 8);
#endif
}
/** Select chaining mode
 * @param aes - Pointer to the AES registers
 * @param chain_mode - chain mode
 */
void
rdw_aes_chain_mode (volatile RDW_AES_T *aes, unsigned chain_mode)
{
  switch (chain_mode)
  {
    case RDW_AES_CHAIN_ECB_MODE:
      aes->control = (aes->control & ~RDW_AES_CHAIN_MASK) | RDW_AES_CHAIN_ECB;
      break;
    case RDW_AES_CHAIN_CBC_MODE:
      aes->control = (aes->control & ~RDW_AES_CHAIN_MASK) | RDW_AES_CHAIN_CBC;
      break;
    case RDW_AES_CHAIN_CTR_MODE:
      aes->control = (aes->control & ~RDW_AES_CHAIN_MASK) | RDW_AES_CHAIN_CTR;
      break;
    case RDW_AES_CHAIN_GCM_MODE:
      aes->control = (aes->control & ~RDW_AES_CHAIN_MASK) | RDW_AES_CHAIN_GCM;
      break;
    default:
      break;
  }
}
/** Select AAD mode
 * @param aes - Pointer to the AES registers
 * @param aad_mode - aad mode
 */
void
rdw_aes_aad_mode (volatile RDW_AES_T *aes, unsigned aad_mode)
{
  switch (aad_mode)
  {
    case RDW_AES_AAD_CIPHER_MODE:
      aes->control = (aes->control & ~RDW_AES_AAD_MASK) | RDW_AES_AAD_CIPHER;
      break;
    case RDW_AES_AAD_DATA_MODE:
      aes->control = (aes->control & ~RDW_AES_AAD_MASK) | RDW_AES_AAD_DATA;
      break;
    case RDW_AES_AAD_H_MODE:
      aes->control = (aes->control & ~RDW_AES_AAD_MASK) | RDW_AES_AAD_H;
      break;
    case RDW_AES_AAD_TAG_MODE:
      aes->control = (aes->control & ~RDW_AES_AAD_MASK) | RDW_AES_AAD_TAG;
      break;
    default:
      break;
  }
}
/* resolve the extern rdw_aes_wait because the ISS models hardware behaviour */
void rdw_aes_wait (volatile RDW_AES_T *aes)
{
    rdw_aes_wait_complete(aes);
}




static volatile  RDW_AES_T   *coney_aes;

/// AES environment structure
struct cobra4_aes_env_tag
{
    /// List of AES function to execute
    struct co_list  queue;
    /// AES under execution
    bool aes_ongoing;
};

/// AES queue environment
static struct cobra4_aes_env_tag  aes_env;





static unsigned int aes_init_flag = 0;
void cobra4_aes_init(void)
{
	coney_aes = (RDW_AES_T *)AES_BASE;

	co_list_init(&(aes_env.queue));
	aes_env.aes_ongoing = false;

	coney_aes->control = (  RDW_AES_ENABLE                            | RDW_AES_KEY_EXPANSION | RDW_AES_KEY_128
	                      | RDW_AES_KEY_SRC_APB | RDW_AES_INT_ENABLE  | RDW_AES_CHAIN_ECB_MODE);
	aes_init_flag = 1;
}

static struct aes_func_env* cobra4_aes_alloc(uint16_t size, aes_func_continue_cb aes_continue_cb, aes_func_result_cb res_cb,
                               uint32_t src_info)
{
    struct aes_func_env* aes_env;

    // allocate AES memory in non-retention to execute it as soon as possible
    aes_env = ke_malloc(size, KE_MEM_NON_RETENTION);

    // initialize AES environment
    aes_env->aes_continue_cb = aes_continue_cb;
    aes_env->aes_res_cb      = res_cb;
    aes_env->src_info        = src_info;

    return aes_env;
}


static void cobra4_encrypt_function(const uint8_t* key, aes_key_size ksize,  const uint8_t *val, const uint8_t *iv, AES_MODE mode)
{
	unsigned int key_size;

	switch(ksize)
	{
		case KEY_SIZE_128:
			key_size = 128;
			break;
		case KEY_SIZE_192:
			key_size = 192;
			break;
		case KEY_SIZE_256:
			key_size = 256;
			break;
		default:
			break;
	}
	switch(mode)
	{
		case ENCRYPT_ECB_MODE:
		{
				rdw_aes_enable(coney_aes);
				rdw_aes_int_disable(coney_aes);
				rdw_aes_key_size(coney_aes, key_size);
				rdw_aes_key_expansion(coney_aes);
				rdw_aes_write_key(coney_aes, key, key_size);
				rdw_aes_wait(coney_aes);
				rdw_aes_clear_int(coney_aes);
				rdw_aes_int_enable(coney_aes);

				rdw_aes_chain_mode(coney_aes, RDW_AES_CHAIN_ECB_MODE);
				rdw_aes_cipher(coney_aes);
				rdw_aes_write_data_in(coney_aes, val);

		}
		break;
		case ENCRYPT_CBC_MODE:
			rdw_aes_enable(coney_aes);
			rdw_aes_int_disable(coney_aes);
			rdw_aes_key_size(coney_aes, key_size);
			rdw_aes_key_expansion(coney_aes);
			rdw_aes_write_key(coney_aes, key, key_size);
			rdw_aes_wait(coney_aes);
			rdw_aes_clear_int(coney_aes);
			rdw_aes_int_enable(coney_aes);

			rdw_aes_chain_mode(coney_aes, RDW_AES_CHAIN_CBC_MODE);

			rdw_aes_write_iv(coney_aes, iv);

			rdw_aes_cipher(coney_aes);
			rdw_aes_write_data_in(coney_aes, val);

			break;
		case ENCRYPT_CTR_MODE:
			rdw_aes_enable(coney_aes);
			rdw_aes_int_disable(coney_aes);
			rdw_aes_key_size(coney_aes, key_size);
			rdw_aes_key_expansion(coney_aes);
			rdw_aes_write_key(coney_aes, key, key_size);
			rdw_aes_wait(coney_aes);
			rdw_aes_clear_int(coney_aes);
			rdw_aes_int_enable(coney_aes);

			rdw_aes_chain_mode(coney_aes, RDW_AES_CHAIN_CTR_MODE);

			rdw_aes_write_iv(coney_aes, iv);

			rdw_aes_cipher(coney_aes);
			rdw_aes_write_data_in(coney_aes, val);


			break;
#if 0
		case ENCRYPT_GCM_MODE:
				rdw_aes_enable(coney_aes);
				rdw_aes_int_disable(coney_aes);
				rdw_aes_key_size(coney_aes, key_size);
				rdw_aes_key_expansion(coney_aes);
				rdw_aes_write_key(coney_aes, key, key_size);
				rdw_aes_wait(coney_aes);
				rdw_aes_clear_int(coney_aes);
				rdw_aes_int_enable(coney_aes);


				rdw_aes_chain_mode(coney_aes, RDW_AES_CHAIN_GCM_MODE);
			    /* Write ICB vector */
			    rdw_aes_write_icb_gcm(coney_aes, iv);
			    // Configure AES in Cipher mode
			    rdw_aes_cipher(coney_aes);
			    /* Generate H */
			    rdw_aes_aad_mode(coney_aes, RDW_AES_AAD_H_MODE);
			    /* Write all zeros */
			    rdw_aes_write_all_zero_data_in(coney_aes);
			    /* Wait */
			   // rdw_aes_wait(aes);

			break;
		case ENCRYPT_GCM_MODE_1:
			rdw_aes_aad_mode(coney_aes, RDW_AES_AAD_DATA_MODE);

			    for (i = 0; i < lenA; i += 128/8)
			    {
			        rdw_aes_write_data_in(aes, aad);
			        /* Wait */
			        rdw_aes_wait(aes);
			        /* Move pointers */
			        aad += (128/8);
			    }

			break;
#endif
		case DECRYPT_ECB_MODE:
			rdw_aes_enable(coney_aes);
			rdw_aes_int_disable(coney_aes);
			rdw_aes_key_size(coney_aes, key_size);
			rdw_aes_key_expansion(coney_aes);
			rdw_aes_write_key(coney_aes, key, key_size);
			rdw_aes_wait(coney_aes);
			rdw_aes_clear_int(coney_aes);
			rdw_aes_int_enable(coney_aes);

			rdw_aes_chain_mode(coney_aes, RDW_AES_CHAIN_ECB_MODE);
			rdw_aes_decipher(coney_aes);
			rdw_aes_write_data_in(coney_aes, val);
			break;
		case DECRYPT_CBC_MODE:
			rdw_aes_enable(coney_aes);
			rdw_aes_int_disable(coney_aes);
			rdw_aes_key_size(coney_aes, key_size);
			rdw_aes_key_expansion(coney_aes);
			rdw_aes_write_key(coney_aes, key, key_size);
			rdw_aes_wait(coney_aes);
			rdw_aes_clear_int(coney_aes);
			rdw_aes_int_enable(coney_aes);

			rdw_aes_chain_mode(coney_aes, RDW_AES_CHAIN_CBC_MODE);

			rdw_aes_write_iv(coney_aes, iv);

			rdw_aes_decipher(coney_aes);
			rdw_aes_write_data_in(coney_aes, val);
			break;
		case DECRYPT_CTR_MODE:
			rdw_aes_enable(coney_aes);
			rdw_aes_int_disable(coney_aes);
			rdw_aes_key_size(coney_aes, key_size);
			rdw_aes_key_expansion(coney_aes);
			rdw_aes_write_key(coney_aes, key, key_size);
			rdw_aes_wait(coney_aes);
			rdw_aes_clear_int(coney_aes);
			rdw_aes_int_enable(coney_aes);

			rdw_aes_chain_mode(coney_aes, RDW_AES_CHAIN_CTR_MODE);

			rdw_aes_write_iv(coney_aes, iv);

			rdw_aes_decipher(coney_aes);
			rdw_aes_write_data_in(coney_aes, val);

			break;
		default:
			break;
	}
}

void cobra4_aes_start(struct aes_func_env* env, const uint8_t* key, aes_key_size key_size,  const uint8_t *val,  const unsigned char *iv, AES_MODE mode)
{
    // put function environment at end of list.
    co_list_push_back(&(aes_env.queue), &(env->hdr));

    // store parameter information
     env->key = key;
     env->val = val;
     env->mode = mode;
     env->key_size = key_size;
     env->iv = iv;

    // AES encryption can be immediately performed
    if(!aes_env.aes_ongoing)
    {
        aes_env.aes_ongoing = true;
      //  coney_aes_function(param);
        cobra4_encrypt_function(key, key_size, val, iv, mode);
    }
}

void  cobra4_aes_encrypt_decryption(const uint8_t* key, aes_key_size key_size, AES_MODE mode, const uint8_t *val,  const unsigned char *iv, aes_func_result_cb res_cb, uint32_t src_info)
{
	
        // allocate environment for the AES execution
        struct aes_func_env* env = cobra4_aes_alloc(sizeof(struct aes_func_env), NULL, res_cb, src_info);

        // Start AES execution
        cobra4_aes_start(env, key, key_size, val, iv, mode);


}

void cobra4_aes_interrupt_function(void)
{
	if(aes_init_flag)
	{
		if(rdw_aes_is_done(coney_aes))
		{
				rdw_aes_clear_int(coney_aes);
				ke_event_set(COBRA4_KE_EVENT_AES_END);
		}
	}
}

void cobra4_aes_result_handler(uint8_t status, uint8_t* result)
{
    // extract first element of the list that contains
    struct aes_func_env* env = (struct aes_func_env*)      co_list_pop_front(&(aes_env.queue));
    struct aes_func_env* env_next = (struct aes_func_env*) co_list_pick(&(aes_env.queue));
    aes_env.aes_ongoing      = false;

    // Prepare new AES Run if requested
    if(env_next != NULL)
    {
        aes_env.aes_ongoing = true;
        cobra4_encrypt_function(env_next->key, env_next->key_size, env_next->val, env_next->iv,  env_next->mode);
      //  rwip_aes_encrypt(env_next->key, env_next->val);
    }

    // Check that AES result has a requester
    if(env)
    {
        bool finished = true;

        // check status of current result
        if(status == 0)
        {
            // continue function computation
            if(env->aes_continue_cb != NULL)
            {
                finished = env->aes_continue_cb(env, result);
            }
        }

        // if function execution is over
        if(finished)
        {
            // Inform requester of end of AES based algorithm
            if(env->aes_res_cb != NULL)
            {
                env->aes_res_cb(status, result, env->src_info);
            }
            {
            	//rdw_printf("0x%x, 0x%x, 0x%x  \n", result[0], result[1], result[2]);
            	//rdw_uart_write(UART0, result[0]);
            	//rdw_uart_write(UART0, result[1]);
            	//rdw_uart_write(UART0, result[2]);
            	//rdw_uart_write(UART0, result[3]);
            	//rdw_uart_write(UART0, result[4]);
            }
            // remove function environment
            ke_free(env);
        }
    }
}


void cobra4_aes_crypt_evt_handler(void)
{
    uint8_t aes_result[16];

    // Clear event
    ke_event_clear(COBRA4_KE_EVENT_AES_END);

    // Load AES result
   // em_rd(aes_result, EM_ENC_OUT_OFFSET, KEY_LEN);
    rdw_aes_read_data_out(coney_aes, aes_result);

    // inform AES result handler
    cobra4_aes_result_handler(0, aes_result);
}


void app_cobra4_aes_event_callback_init(void)
{
		ke_event_callback_set(COBRA4_KE_EVENT_AES_END, &cobra4_aes_crypt_evt_handler);
		NVIC_SetPriority(AES0_SM4_IRQn, 1);
		NVIC_EnableIRQ(AES0_SM4_IRQn);
	
}

#if 0
typedef struct
{
			const uint8_t* key;
			aes_key_size  key_size;
			unsigned char *iv;
	        unsigned char *aad;
	        unsigned char *plain;
	        unsigned int lenA;
	        unsigned int lenC;
	        unsigned char *cipher;
	        unsigned char *tag;
	        unsigned int  operate_step;
}aes_gcm_struct;

static aes_gcm_struct aes_gcm_data;

static void aes_encrypt_gcm_callback(uint8_t status, const uint8_t* aes_res, uint32_t src_info)
{
	 if(aes_gcm_data.operate_step == 1)
	 {
		 aes_gcm_data.operate_step = 2;
		 aes_encrypt_decryption(aes_gcm_data.key, aes_gcm_data.key_size, ENCRYPT_GCM_MODE_1, aes_gcm_data.aad, \
				 aes_gcm_data.iv, aes_encrypt_gcm_callback, 0);
	 }
	 else if(aes_gcm_data.operate_step == 2)
	 {

	 }
}

void aes_encrypt_gcm(  const uint8_t* key,
					aes_key_size ksize,
					const unsigned char iv[],
                    const unsigned char aad[],
                    const unsigned char plain[],
                    unsigned int lenA,
                    unsigned int lenC,
                    unsigned char cipher[],
                    unsigned char tag[])
{
    unsigned int i;

    memset(&aes_gcm_data, 0, sizeof(aes_gcm_data));

    aes_gcm_data.operate_step = 1;   //STEP1
    aes_encrypt_decryption(key, ksize, ENCRYPT_GCM_MODE,aad, iv, aes_encrypt_gcm_callback, 0 );
#if 1
    /* Chain mode GCM */
    rdw_aes_chain_mode(aes, RDW_AES_CHAIN_GCM_MODE);
    /* Write ICB vector */
    rdw_aes_write_icb_gcm(aes, iv);
    // Configure AES in Cipher mode
    rdw_aes_cipher(aes);
    /* Generate H */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_H_MODE);
    /* Write all zeros */
    rdw_aes_write_all_zero_data_in(aes);
    /* Wait */
    rdw_aes_wait(aes);
#endif
    /* Load AAD */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_DATA_MODE);
    for (i = 0; i < lenA; i += 128/8)
    {
        rdw_aes_write_data_in(aes, aad);
        /* Wait */
        rdw_aes_wait(aes);
        /* Move pointers */
        aad += (128/8);
    }
    /* Load plaintext */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_CIPHER_MODE);
    for (i = 0; i < lenC; i += 128/8)
    {
        rdw_aes_write_data_in(aes, plain);
        /* Wait */
        rdw_aes_wait(aes);
        /* Read ciphertext */
        rdw_aes_read_data_out(aes, cipher);
        /* Move pointers */
        plain += (128/8);
        cipher += (128/8);
    }
    /* Load length */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_DATA_MODE);
    /* Write lengths */
    rdw_aes_write_length_data_in(aes, lenA, lenC);
    /* Wait */
    rdw_aes_wait(aes);
    /* Generate Tag */
    rdw_aes_aad_mode(aes, RDW_AES_AAD_TAG_MODE);
    /* Write J0 */
    rdw_aes_write_j0_gcm(aes, iv);
    /* Wait */
    rdw_aes_wait(aes);
    /* Read tag */
    rdw_aes_read_data_out(aes, tag);
}

#endif

#endif

















