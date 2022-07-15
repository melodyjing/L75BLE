/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_m0_patch.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/27

	Description :   

	Changed Log	:

		Liujian  2019/3/27			- Creation


*****************************************************************************/

#ifndef __COBRA_M0_PATCH__
#define __COBRA_M0_PATCH__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_cortex_m0_nvic.h>
#include <cobra_config.h>

#if COBRA_M0_PATCH_EN


typedef struct cobra_m0_patch_t {
	
		__attribute__ ((aligned (4))) volatile unsigned int  ctrl;    // 
	
		__attribute__ ((aligned (4))) volatile unsigned int  enable_index;
	
		__attribute__ ((aligned (4))) volatile unsigned int  rom_base;
	
		__attribute__ ((aligned (4))) volatile unsigned int  remap_base;
	
		__attribute__ ((aligned (4))) volatile unsigned int  reve[6];
	
		__attribute__ ((aligned (4))) volatile unsigned int  rom_index[20];
	
}COBRA_M0_PATCH_T;

typedef enum {
		PATCH_INDEX0,
		PATCH_INDEX1,
		PATCH_INDEX2,
		PATCH_INDEX3,
		PATCH_INDEX4,
		PATCH_INDEX5,	
}PATCH_INDEX;

void cobra_m0_patch_ctrl(unsigned int en);

#define COBRA_M0_PATCH_BASE				(0x40124000UL)

#endif   //COBRA_M0_PATCH_EN



#endif

