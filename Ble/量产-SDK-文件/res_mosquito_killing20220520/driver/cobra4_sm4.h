/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.	
	Author:Liujian
	cobra Drivers
	File Name	:	cobra4_sm4.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/19
	Description :   
	Changed Log	:
		Liujian  2019/8/19			- Creation
*****************************************************************************/
#ifndef __COBRA4_SM4_H__
#define __COBRA4_SM4_H__

#include <cobra_config.h>
#include "rwip_config.h"
#include "ke_event.h"
#include "rdw_mcu.h"
#include "co_list.h"
#include "ke_mem.h"         // Used to allocate memory environment
#include "co_list.h"        // List of AES execution to perform
#include "co_math.h"        // For rand functions

#if COBRA4_SM4_EN

typedef struct cobra4_sm4_type
{
	  volatile __attribute__ ((aligned (4))) unsigned long cfg_sm4_basic;  //config	[22:22] [21:21] [20:20]
	  volatile __attribute__ ((aligned (4))) unsigned long PLAINTEXT_X[4];             //X0 [127:96]
	 // volatile __attribute__ ((aligned (4))) unsigned long PLAINTEXT_X1;             //X1 [95:64]
	 // volatile __attribute__ ((aligned (4))) unsigned long PLAINTEXT_X2;             //X2 [63:32]
	 // volatile __attribute__ ((aligned (4))) unsigned long PLAINTEXT_X3;             //X3 [31:0]
	  volatile __attribute__ ((aligned (4))) unsigned long KEY[4];           //Key0 [127:96]
	 // volatile __attribute__ ((aligned (4))) unsigned long KEY1;           //Key1 [95:64
	 // volatile __attribute__ ((aligned (4))) unsigned long KEY2;           // Key2 [63:32]
	  //volatile __attribute__ ((aligned (4))) unsigned long KEY3;           //Key3 [31:0]
      volatile __attribute__ ((aligned (4))) unsigned long IntCntl;        //Interrupt Control Register
	  
	  
	  volatile __attribute__ ((aligned (4))) unsigned long reserver[10];  
	  
      volatile __attribute__ ((aligned (4))) unsigned long run_status;        //Run Status Register
      volatile __attribute__ ((aligned (4))) unsigned long int_status;        //Interrupt Status Register
	  
	  volatile __attribute__ ((aligned (4))) unsigned long OUTPUT_Y[4];             //X0 [127:96]
//	  volatile __attribute__ ((aligned (4))) unsigned long OUTPUT_Y1;             //X1 [95:64]
//	  volatile __attribute__ ((aligned (4))) unsigned long OUTPUT_Y2;             //X2 [63:32]
//	  volatile __attribute__ ((aligned (4))) unsigned long OUTPUT_Y3;             //X3 [31:0]
	  
} COBRA4_SM4_TypeDef;

typedef enum {
		OP_DECRYPTION,
		OP_ENCRYPTION
}COBRA4_OP_TYPE;

#define  COBRA4_SM4_BASE  (0x400C1000UL)


void cobra4_sm4_init(void);
void cobra4_sm4_cfg_encryp0_cypher1(COBRA4_OP_TYPE op);
void cobra4_sm4_cfg_compute_ekey_en(unsigned int en);
void cobra4_sm4_cfg_sm4_en(unsigned int en);
void cobra4_sm4_cfg_int_clr(void);
void cobra4_sm4_cfg_int_timeout2(unsigned int timeout);
void cobra4_sm4_cfg_int_timeout1(unsigned int timeout);
void cobra4_sm4_cfg_y_vld_int_mask(unsigned int en);
void cobra4_sm4_cfg_sticky_int_en(unsigned int en);
unsigned int cobra4_sm4_ekey_vld(void);
unsigned int cobra4_sm4_y_vld(void);
unsigned int cobra4_sm4_interrupt(void);
unsigned int cobra4_sm4_sticky_int(void);
unsigned int cobra4_sm4_y_vld_int(void);

struct sm4_func_env;

typedef  bool (*sm4_func_continue_cb) (struct sm4_func_env* sm4_env, uint8_t* sm4_result);

typedef void (*sm4_func_result_cb) (uint8_t status, const uint8_t* sm4_res);

struct sm4_func_env
{
    /// used to put AES function in the AES execution queue
    struct co_list_hdr    hdr;
    /// SM4 continue callback
    sm4_func_continue_cb   sm4_continue_cb;
    /// SM4 End callback
    sm4_func_result_cb     sm4_res_cb;

    /// Key to use for the SM4 execution
    const uint8_t*       key;
    /// Value to use for SM4 Cypher/Decypher
    const uint8_t*       val;   
};

/// SM4 environment structure
struct cobra4_sm4_env_tag
{
    /// List of SM4 function to execute
    struct co_list  queue;
    /// SM4 under execution
    bool  sm4_ongoing;
};


#endif
#endif
