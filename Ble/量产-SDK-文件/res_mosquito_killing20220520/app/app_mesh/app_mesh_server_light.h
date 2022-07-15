/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_mesh_server_light.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/7/31
	
	Description :  server light mesh task

	Changed Log	:

		Liujian  2020/7/31			- Creation
*****************************************************************************/

#ifndef __APP_MESH_SERVER_LIGHT_H__
#define __APP_MESH_SERVER_LIGHT_H__

#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "co_bt.h"
#include "co_utils.h"
#include "prf_types.h"               // Profile common types definition
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "m_api.h" 
#include <string.h>


#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_SERVER_LIGHT == 1))

#include "cobra_system.h"	

#define MESH_COMPANY_ID		0x59

#define MESH_AILIBABA_ID		0x01A8

void app_mesh_save_information_event(void);
void app_mesh_task_init(void);
void app_mesh_task_add_mesh(void);
void app_mesh_task_enable_profile(void);
void app_mesh_task_load_info(void);
void app_mesh_task_begin(void);
void app_mesh_task_disconnection(void);
void app_mesh_save_time_stop(void);

extern const struct app_subtask_handlers app_mesh_handlers;
#endif


#endif
