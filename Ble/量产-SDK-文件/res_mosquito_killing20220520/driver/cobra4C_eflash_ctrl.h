/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_eflash_ctrl.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/19

	Description :   flash_ctrl

	Changed Log	:

		Liujian  2019/3/21			- Creation


*****************************************************************************/

#ifndef __COBRA4C_EFLASH_CTRL__
#define __COBRA4C_EFLASH_CTRL__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_EFLASH_CTRL_EN

#if COBRA_AON_PMU_EN
#include "cobra_aon_pmu.h"
#endif

typedef struct cobra_eflash_t {
  __attribute__ ((aligned (4))) volatile unsigned int unlock1_enable;         						
  __attribute__ ((aligned (4))) volatile unsigned int unlock2_enable;          						
  __attribute__ ((aligned (4))) volatile unsigned int unlock3_enable;        							 

	
  __attribute__ ((aligned (4))) volatile unsigned int debug_control;  									

//0x10	
  __attribute__ ((aligned (4))) volatile unsigned int auto_mode_reg;   											
	
  __attribute__ ((aligned (4))) volatile unsigned int elash_parameter1;          					
	
  __attribute__ ((aligned (4))) volatile unsigned int elash_parameter2;        			
	
  __attribute__ ((aligned (4))) volatile unsigned int elash_parameter3;   					
	
	//flash address , byte  Bit[1:0] must be 0
//0x20	
  __attribute__ ((aligned (4))) volatile unsigned int flash_address;      					
	
	//flash program data

  __attribute__ ((aligned (4))) volatile unsigned int flash_program_word0;       									
	
  __attribute__ ((aligned (4))) volatile unsigned int old_flash_program_word1;       									 
	
  __attribute__ ((aligned (4))) volatile unsigned int old_flash_program_word2;       									
//0x30	
  __attribute__ ((aligned (4))) volatile unsigned int old_flash_program_word3;   
	
	//---------------------flash read data
//0x34
  __attribute__ ((aligned (4))) volatile unsigned int flash_read_word0;         							
	
	__attribute__ ((aligned (4))) volatile unsigned int flash_read_word1;    						  			  
	
	__attribute__ ((aligned (4))) volatile unsigned int flash_read_word2;    							  			
	
	__attribute__ ((aligned (4))) volatile unsigned int flash_read_word3;    	
	
	//--------------------------------------------------------------------------
	//0x44
	//redundancy page0 address
	__attribute__ ((aligned (4))) volatile unsigned int redun_page0_addr;    							  
	
	//redundancy page1 address
	__attribute__ ((aligned (4))) volatile unsigned int redun_page1_addr;    		

//	
	__attribute__ ((aligned (4))) volatile unsigned int flash_control_op_and_parameter;    			
//0x50	
	__attribute__ ((aligned (4))) volatile unsigned int flash_state;    									
//0x54	
	__attribute__ ((aligned (4))) volatile unsigned int flash_state_clear;    											
	
}COBRA_EFLASH_T;


/// Flash environment structure
struct cobra_flash_env_tag
{
    /// base address
    uint32_t    base_addr;
    /// length
    uint32_t    length;

};

#define COBRA_EFLASH_BOUNDARY4K   0x1000

////sfn55ulp16kx128m32i32g32r0
//#define EFLASH_TYPE_UNKNOWN  				 0
//#define EFLASH_TYPE_SFN55ULP16KX128  1

/*

0: no-operation(standby), 
1: read, 
2: sleep
4: reference cell erase(only for test)
5: mass erase, 
6: page erase, 
7: program
8: cache access

*/

typedef enum {
		EFLASH_STANDBY,
		EFLASH_READ,
		EFLASH_SLEEP,
		EFLASH_REFERENCE_CELL_ERASE,
		EFLASH_MASS_ERASE,
		EFLASH_PAGE_ERASE,
		EFLASH_PROGRAM,
//		EFLASH_CACHE_ACCESS	
}EFLASH_AUTO_OPERATE;

/*
"00: normal, not any reponse
01: reponse error for invalid access
10: hreadyout set to low when access is invalid
11: forbidden"

*/
typedef enum {
		CTRL_NORMAL,
		CTRL_REPONSE_ERR_FOR_INVALID_ACCESS,
		CTRL_HREADYOUT_LOW_FOR_INVALID_ACCESS,
		CTRL_FORBIDDEN
}EFLASH_CROSS_CTRL;


/*
0: dynamic control, 1: static control(high)

*/
typedef enum {
			DYNAMIC_CONTROL,
			STATIC_CONTROL,
}FLASHXYE_CONTROL;

#define EFLASH_INFOMATION_ADD_CONTROL_FLAG		(1 << 19)

#define COBRA_EFLASH_BASE				(0x400C6800UL)

void cobra_eflash_init(void);
void  cobra_eflash_parameter_init(void);
void cobra_eflash_erase_all(void);
unsigned int cobra_flash_erase(unsigned int offset, unsigned int size);

unsigned int cobra_flash_write(unsigned int offset, unsigned int length, unsigned char *buffer);

void cobra_information_flash_read(uint32_t address, uint16_t number, uint8_t * p_buffer);

void app_cobra4_eflash_test(void);

#if(TUYA_BLE_SDK_ENABLE == 1)
int cobra_tuya_write_nv_data(uint32_t addr,const uint8_t *p_data, uint32_t size);
#endif	

#if(APP_RAINBOW_ENABLE == 1)
void  cobra_eflash_rainbow_parameter_init(void);
#endif	

#if(APP_MOSQUITO_KILLING_ENABLE == 1)
void  cobra_eflash_mosquito_killing_parameter_init(void);
#endif

#endif

#endif


