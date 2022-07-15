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
#ifndef __APP_IRIS_DEVICE_H__
#define __APP_IRIS_DEVICE_H__
#include "rwip_config.h"     // SW configuration

#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "device1_client_uart.h"             
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"	
#include "co_utils.h"
#include "co_math.h"
#include <string.h>
#include "app_realtek_console.h"
#include "def_self.h"

void app_iris_device_add_profile(void);

void app_iris_device_find_service(uint8_t conidx);

void app_iris_increase_msg(void);

void app_iris_decrease_msg(void);

INT32 app_iris_get_msg(void); 
	
extern CONST struct app_subtask_handlers app_iris_device_handlers;

extern uint8_t adv_restart_enable[2];
#endif
