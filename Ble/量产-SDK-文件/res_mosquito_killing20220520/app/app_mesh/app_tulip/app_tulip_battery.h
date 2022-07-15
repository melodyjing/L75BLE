/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_tulip_battery.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/9

	Description :   
	  		

	Changed Log	:

		Liujian  2021/6/9		- Creation

*****************************************************************************/

#ifndef __APP_TULIP_BATTERY_H__
#define __APP_TULIP_BATTERY_H__
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1) && APP_TULIP_BATTER_DETECT_VERSION2)

#include "rdw_adc.h"
#include "cobra4C_eflash_ctrl.h"
#include "cobra_timer.h"

void app_tulip_battery_init(void);

void app_tulip_power_down_save_task(void);

void app_tulip_power_up_process(void);
#endif
#endif

