/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	tuya_ble_port_rdw2150.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/4/30

	Description :   ²Î¿¼nordic µÄ52832, ÒÆÖ² Í¿Ñ» SDK 
	Changed Log	:

		Liujian  2021/4/30		- Creation
*****************************************************************************/
#ifndef __TUYA_BLE_PORT_RDW2150_H__
#define __TUYA_BLE_PORT_RDW2150_H__

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_config.h"
#include "elog.h"


tuya_ble_status_t nrf_err_code_convert(uint32_t errno);

#if (TUYA_BLE_LOG_ENABLE||TUYA_APP_LOG_ENABLE)

#define TUYA_BLE_PRINTF(...)            log_d(__VA_ARGS__)//
#define TUYA_BLE_HEXDUMP(...)           elog_hexdump("", 8, __VA_ARGS__)// 

#else

#define TUYA_BLE_PRINTF(...)           
#define TUYA_BLE_HEXDUMP(...)  

#endif


#endif
