/**@file			cobra4A_eflash_ctrl.h
* @brief            
* @details          
* @author      			
* @date				2020-03-19
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2019/03/19  <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __COBRA_EFLASH_CTRL__
#define __COBRA_EFLASH_CTRL__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_EFLASH_CTRL_EN

#if COBRA_AON_PMU_EN
#include "cobra_aon_pmu.h"
#endif

typedef struct cobra_eflash_t
{
	__attribute__((aligned(4))) volatile unsigned int unlock1_enable;
	__attribute__((aligned(4))) volatile unsigned int unlock2_enable;
	__attribute__((aligned(4))) volatile unsigned int unlock3_enable;

	__attribute__((aligned(4))) volatile unsigned int debug_control;

	//0x10
	__attribute__((aligned(4))) volatile unsigned int auto_mode_reg;

	__attribute__((aligned(4))) volatile unsigned int elash_parameter1;

	__attribute__((aligned(4))) volatile unsigned int elash_parameter2;

	__attribute__((aligned(4))) volatile unsigned int elash_parameter3;

	//flash address , byte  Bit[1:0] must be 0
	//0x20
	__attribute__((aligned(4))) volatile unsigned int flash_address;

	//flash program data

	__attribute__((aligned(4))) volatile unsigned int flash_program_word0;

	__attribute__((aligned(4))) volatile unsigned int old_flash_program_word1;

	__attribute__((aligned(4))) volatile unsigned int old_flash_program_word2;
	//0x30
	__attribute__((aligned(4))) volatile unsigned int old_flash_program_word3;

	//---------------------flash read data
	//0x34
	__attribute__((aligned(4))) volatile unsigned int flash_read_word0;

	__attribute__((aligned(4))) volatile unsigned int flash_read_word1;

	__attribute__((aligned(4))) volatile unsigned int flash_read_word2;

	__attribute__((aligned(4))) volatile unsigned int flash_read_word3;

	//--------------------------------------------------------------------------
	//0x44
	//redundancy page0 address
	__attribute__((aligned(4))) volatile unsigned int redun_page0_addr;

	//redundancy page1 address
	__attribute__((aligned(4))) volatile unsigned int redun_page1_addr;

	//
	__attribute__((aligned(4))) volatile unsigned int flash_control_op_and_parameter;
	//0x50
	__attribute__((aligned(4))) volatile unsigned int flash_state;
	//0x54
	__attribute__((aligned(4))) volatile unsigned int flash_state_clear;

} COBRA_EFLASH_T;

/// Flash environment structure
struct cobra_flash_env_tag
{
	/// base address
	uint32_t base_addr;
	/// length
	uint32_t length;
};

#define COBRA_EFLASH_BOUNDARY4K 0x1000

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

typedef enum
{
	EFLASH_STANDBY,
	EFLASH_READ,
	EFLASH_SLEEP,
	EFLASH_REFERENCE_CELL_ERASE,
	EFLASH_MASS_ERASE,
	EFLASH_PAGE_ERASE,
	EFLASH_PROGRAM,
	//		EFLASH_CACHE_ACCESS
} EFLASH_AUTO_OPERATE;

/*
"00: normal, not any reponse
01: reponse error for invalid access
10: hreadyout set to low when access is invalid
11: forbidden"

*/
typedef enum
{
	CTRL_NORMAL,
	CTRL_REPONSE_ERR_FOR_INVALID_ACCESS,
	CTRL_HREADYOUT_LOW_FOR_INVALID_ACCESS,
	CTRL_FORBIDDEN
} EFLASH_CROSS_CTRL;

/*
0: dynamic control, 1: static control(high)

*/
typedef enum
{
	DYNAMIC_CONTROL,
	STATIC_CONTROL,
} FLASHXYE_CONTROL;

#define EFLASH_INFOMATION_ADD_CONTROL_FLAG (1 << 18)

#define COBRA_EFLASH_BASE (0x400C6800UL)
#define	EFLASH_ID		0x01
#define	EFLASH_BASE_ADDR		0x000000
#define EFLASH_SECT_SIZE		4096
#define EFLASH_INFOMATION_ADD_CONTROL		0x80000

void cobra_eflash_page_erase(uint32_t offset);
void cobra_eflash_program(uint32_t offset, uint32_t program_data);
void cobra_eflash_read(uint32_t addr, uint32_t *read_data);
void cobra_eflash_init(void);
void cobra_eflash_parameter_init(void);
void cobra_eflash_erase_all(void);
uint32_t cobra_information_flash_read(unsigned int address);
void cobra_eflash_page_erase(uint32_t offset);
void app_cobra4_eflash_test(void);
void cobra_eflash_program(uint32_t offset, uint32_t program_data);
void cobra_eflash_read(uint32_t addr, uint32_t *read_data);
unsigned int cobra_eflash_auto_op_end(void);
void cobra_eflash_unlock2_enable(unsigned int en);
void cobra_eflash_operation_interrupt_enable(unsigned int en);
void cobra_eflash_bor_interrupt_enable(unsigned int en);
void cobra_eflash_unlock1_enable(unsigned int en);
void cobra_eflash_unlock3_enable(unsigned int en);
void cobra_eflash_flash_auto_operation_mode(EFLASH_AUTO_OPERATE operate);
void cobra_eflash_auto_opend_clr(void);
void cobra_eflash_flash_cache_flush_en(unsigned int en);
#endif

#endif
