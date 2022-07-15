/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_cache_ctrl.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/22

	Description :   from arm IP CG092
					The CG092 is a simple cache for on-chip embedded flash (eFlash).

	Changed Log	:

		Liujian  2019/3/22			- Creation


*****************************************************************************/

#ifndef __COBRA_CACHE_CTRL__
#define __COBRA_CACHE_CTRL__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_CACHE_CTRL_EN


typedef struct cobra_cache_t {
	
		__attribute__ ((aligned (4))) unsigned int cache_config; /*Configuration and Control Register*/
		
		__attribute__ ((aligned (4))) unsigned int cache_status;  /*Status Register*/
	
		__attribute__ ((aligned (4))) unsigned int cache_irq_mask;  /*Interrupt Request Mask register*/
	
		__attribute__ ((aligned (4))) unsigned int cache_irq_state;  /*Interrupt Request Status register */
	
		__attribute__ ((aligned (4))) unsigned int cache_hwparams;  /* Hardware Parameters register */
	
		__attribute__ ((aligned (4))) unsigned int cache_statistic_hit;  /* Cache Statistic Hit Register */
	
		__attribute__ ((aligned (4))) unsigned int cache_statistic_miss;  /* Cache Statistic Miss Register */
	 
}COBRA_CACHE_T;


#define COBRA_CACHE_BASE				(0x400C6000UL)


void cobra_cache_init(void);
void cobra_cache_normal_mode_enable(unsigned int en_cache);
void cobra_cache_invalidate_the_cache(void);
#endif   //COBRA_CACHE_CTRL_EN



#endif

