/*****************************************************************

	Cobra Drivers

	File Name	:	cobra_soft_timer.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/5

	Description :   
	
	Changed Log	:

		Liujian  2019/5/5		- Creation
		
********************************************************************/


#ifndef __COBRA_SOFT_TIMER_H__
#define __COBRA_SOFT_TIMER_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_SOFT_TIMER_EN

#define MAX_SOFT_TIMERS				8

typedef void (*timer_handler_t)(uint32_t);

#define TIMER_CREATE_MASK					0x01
#define TIMER_ACTIVER_MASK				0x02
#define TIMER_REPEAT_MASK					0x04


typedef struct
{
	  //!< Timeout tick counter. */
	  
    uint32_t timer_out_tick; 				
		uint32_t reload_timer_out_tick;
	
		uint32_t timer_mode;
	
	  //!< Timeout function handler */
    timer_handler_t TimerHandler; 	
}soft_timer_t;


#define COBRA_SOFT_TIMER_TICK		100   //10ms

void cobra_soft_timer_init(void);
void cobra_soft_timer_update(void);

int32_t cobra_soft_timer_create(timer_handler_t FunctionPointer);
void cobra_soft_timer_destroy(uint32_t timerId);
void cobra_soft_timer_start(uint32_t timerId, uint32_t timeout, uint32_t period);
void cobra_soft_timer_stop(uint32_t timerId);

void app_cobra_soft_timer_init(void);
void app_cobra_soft_timer_uninit(void);
#endif   //COBRA_SOFT_TIMER_EN


#endif

