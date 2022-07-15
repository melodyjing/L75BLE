/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  tuya_app_ble_services.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/5/18

	Description :   
	      ÒÆÖ²µ½Í¿Ñ» SDK 

	Changed Log	:

		Liujian  2021/5/18			- Creation
*****************************************************************************/

#ifndef   __TUYA_APP_BLE_SERVICES_H__
#define   __TUYA_APP_BLE_SERVICES_H__

#if(TUYA_BLE_SDK_ENABLE == 1)

#include <stdint.h> // Standard Integer Definition
#include "ke_task.h"
#include "tuya_ble_services_task.h"

struct app_tuya_ble_env_tag
{
	
    uint8_t rx_data[MAX_TUYA_BLE_DATA_SIZE];
//    uint8_t receive_data[MAX_RX_TUYA_BLE_SIZE];
//    uint8_t receive_length;
	
};

void app_tuya_ble_send_mtu(const uint8_t * data, uint16_t length);

void app_tuya_ble_init(void);

void app_tuya_ble_enable_prf(uint8_t conidx);

void app_tuya_ble_add_services(void);

extern const struct app_subtask_handlers app_tuya_ble_handlers;

#endif

#endif
