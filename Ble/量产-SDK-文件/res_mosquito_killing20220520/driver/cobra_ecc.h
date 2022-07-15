/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_ecc.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/29

	Description :   
	
	Changed Log	:

		Liujian  2019/5/29		- Creation



*****************************************************************************/


#ifndef __COBRA_ECC_H__
#define __COBRA_ECC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <co_list.h>
#include <cobra_config.h>

#if COBRA_ECC_EN
typedef struct cobra_ecc_type
{
		volatile __attribute__ ((aligned (4))) unsigned int control;                /**< Control fields  */
    volatile __attribute__ ((aligned (4))) unsigned int status;                 /**< Status fields */
    volatile __attribute__ ((aligned (4))) unsigned int px;                     /**< EC base point x coordinate or operand */
    volatile __attribute__ ((aligned (4))) unsigned int py;                     /**< EC base point y coordinate or operand */
    volatile __attribute__ ((aligned (4))) unsigned int reserve1;                    /**< Private key */
    volatile __attribute__ ((aligned (4))) unsigned int reserve2;                     /**< EC "a" parameter */
    volatile __attribute__ ((aligned (4))) unsigned int p;                      /**< EC "prime" or "order" depending on operation */
//    __attribute__ ((aligned (4))) unsigned int rx;                     /**< Result x coordinate */
//    __attribute__ ((aligned (4))) unsigned int ry;                     /**< Result y coordinate */
//    __attribute__ ((aligned (4))) unsigned int sx;                     /**< Second operand in ECA x coordinate */
//    __attribute__ ((aligned (4))) unsigned int sy;                     /**< Second operand in ECA y coordinate */
//    __attribute__ ((aligned (4))) unsigned int m;                      /**< Hashed message digest for EC DSA */

} COBRA_ECC_TypeDef; 



/** ECC context
 * ecc          - Holds a pointer to the ECC hardware base address
 * key_size     - Run-time key size being operated on
 * key_size_max - Maximum key size the hardware was parameterised with,
 *                this may be less than RDW_ECC_KEY_SIZE_MAX which is the
 *                maximum for any hardware parameterisation. The loading and
 *                unloading functions need to know this information and its not
 *                captured in the ECC registers.
 */
typedef struct cobra_ecc_ctx
{
//    volatile COBRA_ECC_TypeDef *ecc;                                                             /**< Pointer to the ECC hardware base address */
    unsigned int key_size;                                                      /**< Run-time key size in 32-bit words */
    unsigned int key_size_max;                                                  /**< Hardware parameterised maximum key size in 32-bit words */
} COBRA_ECC_CTX_T;


typedef enum{
	ECC_MODULAR_ADDITION,
	ECC_MODULAR_SUBTRACTION,
	ECC_MODULAR_MULTIPLICATION,
	ECC_MODULAR_DIVISION,
	ECC_MODULAR_INVERSION,
}ECC_OPERATION;

//typedef void (*eccmicro_func_result_cb) (uint8_t status, const uint8_t* aes_res, uint32_t src_info);

#define  COBRA_ECC_KEYSIZE_MAX             521                                     /**< Maximum key size in bits. Parameterised hardware may be less but not more */
#define  COBRA_ECC_KEYSIZE32_MAX           ((COBRA_ECC_KEYSIZE_MAX+31)/32)           /**< Maximum key size in rounded up 32-bit words */


#define  COBRA_ECC_INF_32                  0xFFFFFFFFUL                            // Assume infinity is all F's upto key_size


#define  COBRA_ECC_BASE	 (0x400C2000UL)


void cobra_ecc_init(void);

void cobra_app_eccmicro_task_init(void);

void app_cobra_eccmicro_test(void);

void app_cobra_eccmicro_operation(COBRA_ECC_CTX_T *ctx, 
																	const unsigned int  *px_operation, 
																  const unsigned int  *py_operation, 
																	const unsigned int  *prime_factor, 
																	ECC_OPERATION  operation
																	);

typedef void (*eccmicro_eca_res_cb) (const uint8_t* p_ecc_res);
																	

									
									


					
//cobra ecc application function and data
/// ECCMICRO environment structure
struct ecc_env_tag
{
    /// List of ECC function to execute
    struct co_list  queue;
    /// ECC under execution
    bool  ecc_ongoing;
};

struct ecc_func_env;

typedef void (*ecc_func_result_cb) (uint32_t* ecc_res1,  uint32_t* ecc_res2, uint32_t src_info);

typedef  bool (*ecc_func_continue_cb) (struct ecc_func_env* ecc_func_env, uint32_t* ecc_result1, uint32_t* ecc_result2);	

/// Environment variable required for an ECC based function
/// This structure must be Header of all function environment variables
struct ecc_func_env
{
    /// used to put ECC function in the ECC execution queue
   // struct co_list_hdr    hdr;
    /// ECC continue callback
    ecc_func_continue_cb       ecc_continue_cb;
    /// ECC End callback
    ecc_func_result_cb    			ecc_res_cb;

	
		const unsigned int  *px;
		const unsigned int  *py;
		const unsigned int  *p_factor;
		
		COBRA_ECC_CTX_T *ctx;
	
		ECC_OPERATION operation;
    /// Information put in source id message to retrieve requester
    uint32_t             src_info;
};


struct eccmicro_eca_env
{
    /// ECC Environment structure
    struct ecc_func_env  ecc_eca_env;
		
		uint16_t						step;
		
		const unsigned int * qx;
		const unsigned int * qy;
		
		unsigned int * rx;
		unsigned int * ry;   

		unsigned int D[COBRA_ECC_KEYSIZE32_MAX];        // Temporary array storage
    unsigned int E[COBRA_ECC_KEYSIZE32_MAX];
	
};

int app_cobra_eccmicro_eca(COBRA_ECC_CTX_T *ctx,
                  const unsigned int *px,
                  const unsigned int *py,
                  const unsigned int *qx,
                  const unsigned int *qy,
                  const unsigned int *modulus,
                  unsigned int *rx,
                  unsigned int *ry,
								  eccmicro_eca_res_cb res_cb,
								  ecc_func_result_cb res_cb1);
									
#endif   //COBRA_ECC_EN
									
#endif

