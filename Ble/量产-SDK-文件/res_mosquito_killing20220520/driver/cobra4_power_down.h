/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_power_down.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/5/17

	Description :   


	Changed Log	:

		Liujian  2021/5/17		- Creation

*****************************************************************************/
#ifndef __COBRA4_POWER_DOWN_H__
#define __COBRA4_POWER_DOWN_H__
#include <stdbool.h>
#if (COBRA_POWER_DOWN_ENABLE)

#if  LOW_POWER_WAKE_UP_BY_UART

void  cobra_low_power_timer0_init(void);
void  cobra_low_power_uart0_receive_handle(void);

#endif

extern volatile bool ble_deep_sleep;	

void app_poweron_lowpower_init(void);

int app_cobra4_power_down_process(void);

void app_cobra4_power_down_process_48m(void);

void  cobra_low_power_timer0_start(void);
#endif

#endif