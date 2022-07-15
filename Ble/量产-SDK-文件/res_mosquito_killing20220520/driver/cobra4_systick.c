/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4A_systick.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/10

	Description :   
       根据之前的定时器, 来做的更改, 算得上是第二个版本

	Changed Log	:

		Liujian  2020/8/10 		- Creation

*****************************************************************************/
#include "cobra4_systick.h"
#include "cobra_system.h"

#if (COBRA_POWER_DOWN_ENABLE==1)
static systick_callback  cobra4_systick_callback __attribute__((section("retention_area"),zero_init));
#else
static systick_callback  cobra4_systick_callback = NULL;
#endif

void cobra4_systick_init(unsigned int timer_frequency, systick_callback callback)
{
	  volatile unsigned int tick_number;
		volatile unsigned int cpu_hz;
	  cobra4_systick_callback = callback;
	  cpu_hz = cobra_system_get_cpu_frequency();
	  tick_number = cpu_hz/timer_frequency;           
		
	  SysTick_Config(tick_number);      

		NVIC_SetPriority(SysTick_IRQn, 1);
//  	NVIC_EnableIRQ(SysTick_IRQn);	
}

void cobra4_systick_uninit(void)
{
//		NVIC_DisableIRQ(SysTick_IRQn);
    cobra4_systick_callback =NULL;	
	  SysTick->CTRL = 0x00;
}


void SysTick_Handler(void)
{
	  if(cobra4_systick_callback != NULL)
		{
				cobra4_systick_callback();
		}		
}