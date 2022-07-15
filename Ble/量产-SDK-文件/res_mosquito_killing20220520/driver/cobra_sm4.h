/**
 * \file sm4.h
 */
#ifndef XYSSL_SM4_H
#define XYSSL_SM4_H
 
#define SM4_ENCRYPT     1
#define SM4_DECRYPT     0
 
/**
 * \brief          SM4 context structure
 */
typedef struct
{
    int mode;                   /*!<  encrypt/decrypt   */
    unsigned long sk[32];       /*!<  SM4 subkeys       */
}
sm4_context;
 
 
#ifdef __cplusplus
extern "C" {
#endif

/*
 * 32-bit integer manipulation macros (big endian)
 */
 
#define BBIG_ENDIAN  1
//fool compiler for Little Endian
#ifdef BBIG_ENDIAN
#ifndef GET_ULONG
#define GET_ULONG(n,b,i)                            \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
        | ( (unsigned long) (b)[(i) + 1] << 16 )        \
        | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 3]       );       \
}
#endif
 
#ifndef PUT_ULONG
#define PUT_ULONG(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif
#else
#ifndef GET_ULONG
#define GET_ULONG(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 0 )        \
        | ( (unsigned long) (b)[(i) + 1] << 8 )        \
        | ( (unsigned long) (b)[(i) + 2] << 16 )        \
        | ( (unsigned long) (b)[(i) + 3] << 24 );       \
}
#endif

#ifndef PUT_ULONG
#define PUT_ULONG(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 0 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 8 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
}
#endif
#endif
 

/**
 * \brief          SM4 key schedule (128-bit, encryption)
 *
 * \param ctx      SM4 context to be initialized
 * \param key      16-byte secret key
 */
extern void sm4_setkey_enc( sm4_context *ctx, unsigned char key[16] );
 
/**
 * \brief          SM4 key schedule (128-bit, decryption)
 *
 * \param ctx      SM4 context to be initialized
 * \param key      16-byte secret key
 */
extern void sm4_setkey_dec( sm4_context *ctx, unsigned char key[16] );
 
/**
 * \brief          SM4-ECB block encryption/decryption
 * \param ctx      SM4 context
 * \param mode     SM4_ENCRYPT or SM4_DECRYPT
 * \param length   length of the input data
 * \param input    input block
 * \param output   output block
 */
extern void sm4_crypt_ecb( sm4_context *ctx,
				     int mode,
					 int length,
                     unsigned char *input,
                     unsigned char *output);
 
/**
 * \brief          SM4-CBC buffer encryption/decryption
 * \param ctx      SM4 context
 * \param mode     SM4_ENCRYPT or SM4_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 */
extern void sm4_crypt_cbc( sm4_context *ctx,
                     int mode,
                     int length,
                     unsigned char iv[16],
                     unsigned char *input,
                     unsigned char *output );

									 
#if USE_RDW_HARDWARE_SUPPORT 
extern void cobra_sm4_IRQHandler(void);
#endif										 
//sm4 test
extern void sm4_test(void);
										 
extern void cobra_sm4_init(void);
										 
#ifdef __cplusplus
}
#endif
 
#endif /* sm4.h */
