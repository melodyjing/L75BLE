/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_m3_systick.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/11

	Description :   
	
	Changed Log	:

		Liujian  2019/4/11		- Creation



*****************************************************************************/

#ifndef __COBRA_M3_SYSTICK_H__
#define __COBRA_M3_SYSTICK_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_M3_SYSTICK_EN

uint32_t timer_get_time(void);

void timer_enable(bool enable);

void timer_set_timeout(uint32_t target, void (*pfnHandler)(void));

void cobra_m3_sys_tick_init(void);

void cobra_m3_sys_tick_uninit(void);

#endif
#endif

