/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_m3_systick.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/11

	Description :   


	Changed Log	:

		Liujian  2019/4/11			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_m3_systick.h"

#include <stdbool.h>
#include <stdint.h>
#include "cobra_system.h"
#include "rdw_gpio.h"
#include "ke.h"
#include "ke_event.h"
#include "rdw_mcu.h"


#if COBRA_M3_SYSTICK_EN

//模拟一个时间
//10ms 一次中断
static uint32_t ble_time = 0;

void (*timer_func)(void); 


void cobra_m3_sys_tick_init(void)
{
		unsigned int tick_number;
		unsigned int cpu_hz;
	  cpu_hz = cobra_system_get_cpu_frequency();
	  tick_number = cpu_hz/KERNEL_TIME_HZ;           
		
	
	  SysTick_Config(tick_number);      


		NVIC_SetPriority(SysTick_IRQn, 1);
  	NVIC_EnableIRQ(SysTick_IRQn);
}



void cobra_m3_sys_tick_uninit(void)
{
		NVIC_DisableIRQ(SysTick_IRQn);
}




//uint32_t ble_systick_get()
//{
//	 return ble_time;
//}


void SysTick_Handler(void)
{

	 ble_time++;
#if CFG_CSEM_RF_PROTOCOL	
	 ke_event_set(KE_EVENT_KE_TIMER);
#endif
	

}
#if CFG_CSEM_RF_PROTOCOL	
uint32_t timer_get_time(void)
{
		return ble_time;
}
#endif

#if CFG_CSEM_RF_PROTOCOL	
void timer_enable(bool enable)
{
		ble_time = 0;
	  timer_func = NULL;
}
#endif

#if CFG_CSEM_RF_PROTOCOL	
void timer_set_timeout(uint32_t target, void (*pfnHandler)(void))
{
		if(target == 0)
		{
				timer_func = NULL;
		}
		else
		{
				timer_func = pfnHandler;
		}
}
#endif



#endif














