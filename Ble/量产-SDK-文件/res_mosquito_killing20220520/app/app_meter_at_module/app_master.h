/******************************************************************************
	File Name	:	app_iris_device.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/06/10

	Description : 
    使用　client profile, 应用 device1_client_uart profile 	

	Changed Log	:
		Liujian  2020/06/10			- Creation

*****************************************************************************/
#ifndef __APP_MASTER_H__
#define __APP_MASTER_H__
#include "rwip_config.h"     // SW configuration

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions       
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"	
#include "co_utils.h"
#include "co_math.h"
#include <string.h>

void app_master_device_add_profile(void);

void app_master_device_find_service(uint8_t conidx);

void app_master_increase_msg(void);

void app_master_decrease_msg(void);

int32_t app_master_get_msg(void);
	
extern const struct app_subtask_handlers app_master_device_handlers;

#endif

#endif
