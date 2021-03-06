/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_mesh_task.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2020/5/6

	Description :   mesh task

	Changed Log	:

		Liujian  2020/5/6			- Creation
*****************************************************************************/
#ifndef __APP_TUYA_MESH_TASK_H__
#define __APP_TUYA_MESH_TASK_H__

#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "co_bt.h"
#include "co_utils.h"
#include "prf_types.h"               // Profile common types definition
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "m_api.h" 
#include <string.h>
#include "cobra_system.h"
//#include "m_api.h" 
//#include "m_api_msg.h"
//#include "m_tb.h"

#if ((BLE_APP_MESH == 1) && (BLE_TUYA_MESH_SDK_ENABLE == 1))
#define MESH_COMPANY_ID		0x59

#define MESH_AILIBABA_ID		0x01A8

void app_mesh_save_information_event(void);
void app_mesh_task_init(void);
void app_mesh_task_add_mesh(void);
void app_mesh_task_enable_profile(void);
void app_mesh_task_load_info(void);
void app_mesh_task_begin(void);
void app_mesh_task_disconnection(void);
extern const struct app_subtask_handlers app_mesh_handlers;

typedef struct {
	   uint8_t   elmt_idx;       //注册时用的 element index
	   m_lid_t   mdl_lid[28];    //最多一个element 下有 10 个 model
	   uint32_t  model_id[28];
	   uint8_t   total_model;
}model_idx_element;

#endif


#endif

