/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4A_linked_softtimer.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/11

	Description :   
     链表式 软件定时器, 需要用到 linked Library

	Changed Log	:

		Liujian  2020/8/11 		- Creation

*****************************************************************************/
#ifndef __COBRA4A_LINKED_SOFTTIMER_H__
#define __COBRA4A_LINKED_SOFTTIMER_H__
#include <stdint.h>
#include "cobra4_common.h"
#include <cobra_config.h>
#include "cobra4A_linked_softtimer_config.h"
#if ((COBRA_LINK_LIBRARY_ENABLE == 1) && (COBRA_LINKEN_SOFT_TIMER_ENABLE == 1))


struct _SoftwareTimer_
{
    uint32_t Milliseconds;
    Bool Active;
    void(*Callback)(struct _SoftwareTimer_ *);
    uint32_t CallbackInterval;
};

typedef struct _SoftwareTimer_  COBRA_SOFTWARE_TIMER;

Bool cobra_linked_softtimer_init_timer_lib(void);

COBRA_SOFTWARE_TIMER *cobra_linked_softtimer_create(COBRA_SOFTWARE_TIMER *SoftwareTimer, void(*SoftwareTimerCallback)(COBRA_SOFTWARE_TIMER *SoftwareTimer), uint32_t CallbackInterval);

Bool cobra_linked_softtimer_start(COBRA_SOFTWARE_TIMER *SoftwareTimer);

Bool cobra_linked_softtimer_stop(COBRA_SOFTWARE_TIMER *SoftwareTimer);

Bool cobra_linked_softtimer_restart(COBRA_SOFTWARE_TIMER *SoftwareTimer);

Bool cobra_linked_softtimer_reset(COBRA_SOFTWARE_TIMER *SoftwareTimer);

Bool cobra_linked_softtimer_delete(COBRA_SOFTWARE_TIMER *SoftwareTimer);

Bool cobra_linked_softtimer_is_running(COBRA_SOFTWARE_TIMER *SoftwareTimer);

Bool cobra_linked_softtimer_set_interval(COBRA_SOFTWARE_TIMER *SoftwareTimer, uint32_t NewInterval);

uint32_t cobra_linked_softtimer_get_interval(COBRA_SOFTWARE_TIMER *SoftwareTimer);

Bool cobra_linked_softtimer_set_callback_and_interval(COBRA_SOFTWARE_TIMER *SoftwareTimer, void(*TimerCallback)(COBRA_SOFTWARE_TIMER *), uint32_t Interval);

Bool cobra_linked_softtimer_set_callback(COBRA_SOFTWARE_TIMER *SoftwareTimer, void(*SoftwareTimerCallback)(COBRA_SOFTWARE_TIMER *));

uint32_t cobra_linked_softtimer_get_milliseconds(COBRA_SOFTWARE_TIMER *SoftwareTimer);

#endif

#endif

