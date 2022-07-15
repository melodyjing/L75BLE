/******************************************************************************
	File Name	  :	  ota_device_task.h
	Version		  :	  0.1
	Created	By	:	  LiuJian
	Date		    :	  2020/08/26

	Description :  
    用于和OTA 设备建立连接	

	Changed Log	:
		Liujian  2020/08/26	   - Creation

*****************************************************************************/
#ifndef __OTA_DEVICE_TASK_H__
#define __OTA_DEVICE_TASK_H__

#if ((BLE_APP_RECEIVE_VOICE == 0) && (APP_OTA_MATSER_ENABLE == 1))

#include <stdint.h>         // Standard Integer
#include "rwip_task.h"      // Task definitions
#include "ke_task.h"        // Kernel Task
#include "rwip_config.h"             // SW configuration
#include "gapc_task.h"            // GAP Controller Task API
#include "gapm_task.h"            // GAP Manager Task API
#include "arch.h"                 // Platform Definitions
#include <string.h>
#include "co_utils.h"
#include "ke_timer.h"             // Kernel timer
#include <stdio.h>
#include "ota_master_task.h"
#include "ota_device.h"
#endif


#endif
