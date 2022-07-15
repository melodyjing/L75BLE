/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	Author:Liujian
	cobra Drivers
	File Name	:	cobra4_ble_common.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/7/9
	Description :   
          把一部分 cobra4_common  的函数移到这个文件中
	Changed Log	:
		Liujian  2021/7/9			- Creation

*****************************************************************************/

#ifndef __COBRA4_BLE_COMMON_H__
#define __COBRA4_BLE_COMMON_H__

#include "rwble_hl_error.h"

#include <stdint.h>
#include "rwip_config.h"

#if (BLE_APP_MESH == 1)
void app_ble_mesh_update_param(void);	
#endif

void ble_update_adv_data(uint8_t const* p_ad_data, uint8_t ad_len, enum KE_TASK_TYPE task_number);







#endif

