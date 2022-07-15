/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_cache_ctrl.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/22

	Description :   The CG092 is a simple cache for on-chip embedded flash (eFlash).
					
	Changed Log	:

		Liujian  2019/3/22			- Creation


*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_cache_ctrl.h"


#if COBRA_CACHE_CTRL_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_CACHE_T *cobra_cache __attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_CACHE_T * cobra_cache;
#endif
	
void cobra_cache_init(void)
{
		cobra_cache = (COBRA_CACHE_T *)COBRA_CACHE_BASE;  
}
/*

Bits Name Description Access Reset
[31:7] Reserved - RO,                      RAZ                    0
   6 		STATISTIC_EN Enable statistics logic:       0: Disabled. Counters are stalled.
																							      1: Enable. Counters are running.                         RW 1

	 5    SET_PREFETCH Cache Prefetch Setting         0: Disable prefetch.
                                                    1: Enable prefetch.                                      RW 0

	 4    SET_MAN_INV Cache Invalidate Setting:       0: Automatic cache invalidate when cache enabled.
																										1: Manual cache invalidate mode.                         RW 0

   3    SET_MAN_POW Power Control Setting:          0: Automatic.
																								    1: Manual.                                               RW 0

   2    POW_REQ Manual SRAM power request.                                                                   RW 0
 
   1    INV_REQ Manual invalidate request.
																						        Functional only when SET_MAN_INV is set.
																								    Automatically cleared when invalidation is finished
																										or power or invalidation error occurs. Cannot be
																										cleared manually.
																										Manual invalidation request must be set only when
																										the cache is disabled, otherwise it causes an
																										invalidation error interrupt.                             RW 0
	  0    EN Cache Enable:                           0: Disable cache.
																										1: Enable cache

*/

//Auto power and auto invalidate mode
void cobra_cache_normal_mode_enable(unsigned int en_cache)
{
		if(en_cache)
		{
				cobra_cache->cache_config &= ~(1 << 4);
				cobra_cache->cache_config |= 0x01;
		//		while((cobra_cache->cache_status & 0x03) != 0x02);			//cobra 1 hase bug, need add some here
		}
		else
		{
				cobra_cache->cache_config &= ~0x01;			
		}
}


//Manual power and auto invalidate mode
void cobra_cache_manual_power_auto_invalidate_mode_enable(unsigned int en_cache)
{
		if(en_cache)
		{
				cobra_cache->cache_config |= 1<<3;
				cobra_cache->cache_config |= 1<<2;
				while((cobra_cache->cache_status & 0x03) != 0x01);
				cobra_cache->cache_config |= 0x01;
		}
		else
		{
				cobra_cache->cache_config &= ~0x01;
				cobra_cache->cache_config &= ~(1 << 2);	
		}
}

//Manual power and manual invalidate mode
void cobra_cache_manual_power_manual_invalidate_mode_enable(unsigned int en_cache, unsigned int en_invalidate_sram )
{
		 if(en_cache)
		 {
				 cobra_cache->cache_config |= 1<<3;
				 cobra_cache->cache_config |= 1<<4;
				 cobra_cache->cache_config |= 1<<2;
				 while(!(cobra_cache->cache_status & (1 << 4)));
			   if(en_invalidate_sram)   //invalidate sram
				 {
						cobra_cache->cache_config |= 1<<1;
						while(cobra_cache->cache_config & (1 << 1));
				 }
				 cobra_cache->cache_config |= 0x01;		 
		 }
		 else
		 {
				 cobra_cache->cache_config &= ~0x01;	
		 }
	
}

/*
Invalidating the cache
To invalidate the cache if it is enabled:
	*/
void cobra_cache_invalidate_the_cache(void)
{
		cobra_cache->cache_config = 0x1c;
	  while((cobra_cache->cache_status & 0x03) != 0x00);
		cobra_cache->cache_config |= 1 << 1;
		while(cobra_cache->cache_config & (1 << 1));
		cobra_cache->cache_config |= 0x01;
}

//Cache Statistic Hit Register, CSHR

/*

Enable statistics logic:
0: Disabled. Counters are stalled.
1: Enable. Counters are running.

*/
void cobra_cache_statistic_enable(unsigned int en)
{
			if(en)
			{
					cobra_cache->cache_config |= (1 << 6);
			}
			else
			{
					cobra_cache->cache_config &= ~(1 << 6);
			}
	
}

/*
Counts the number of cache hits during cache look up.
Only cacheable read transactions are looked up by the
CG092.
Writing to the register clears the contents.

*/

void cobra_cache_clear_statistic_hit_number(void)
{
		cobra_cache->cache_statistic_hit = 0x00;	
}

unsigned int  cobra_cache_statistic_hit_number(void)
{
		return cobra_cache->cache_statistic_hit;
}

/*

Counts the number of cache misses during cache look up.
Only cacheable read transactions are looked up by the
CG092.
Writing to the register clears the contents.
*/
void cobra_cache_clear_statistic_miss_number(void)
{
		cobra_cache->cache_statistic_miss = 0x00;	
}

unsigned int  cobra_cache_statistic_miss_number(void)
{
		return cobra_cache->cache_statistic_miss;
}

//interrupt

/*

Mask interrupt request on manual invalidation
error indication (IRQSTAT.MAN_INV_ERR
is set).
*/
void cobra_cache_manual_invalidation_error_interrupt_enable(unsigned int en)
{
	 if(en)
	 {
			cobra_cache->cache_irq_mask &= ~(1 << 1); 
	 }
	 else
	 {
			cobra_cache->cache_irq_mask |= (1 << 1); 	
	 }
}

/*
Mask interrupt request on Power Error
indication (IRQSTAT.POW_ERR is set).
*/
void cobra_cache_power_error_interrupt_enable(unsigned int en)
{
	 if(en)
	 {
			cobra_cache->cache_irq_mask &= ~(1 << 0); 
	 }
	 else
	 {
			cobra_cache->cache_irq_mask |= (1 << 0); 	
	 }
}

//cobra 2 interrupt at M3 interrupt 34 

// at M0 is 
void cobra_cache_interrupt(void)
{
		if(cobra_cache->cache_irq_state & (1 << 1))
		{
				cobra_cache->cache_irq_state |= (1 << 1);
		}
		else if(cobra_cache->cache_irq_state & 0x01)
		{
				cobra_cache->cache_irq_state  |= 0x01;
		}
}

#endif














