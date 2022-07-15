/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Application for Tulips

	File Name	:	app_tulip_light.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/2/19
	
*******************************************************************************/
#ifndef __APP_TULIPE_LIGHT_H__
#define __APP_TULIPE_LIGHT_H__

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

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
	
#define MESH_COMPANY_ID		0x59

#define MESH_AILIBABA_ID		0xE156

void app_mesh_save_information_event(void);
void app_mesh_task_init(void);
void app_mesh_task_add_mesh(void);
void app_mesh_task_enable_profile(void);
void app_mesh_task_load_info(void);
void app_mesh_task_begin(void);
void app_mesh_task_disconnection(void);
extern const struct app_subtask_handlers app_mesh_handlers;

void app_mesh_report_light_on_off_state(unsigned int on_off_state);

void app_mesh_key_response_on_off_state(unsigned int on_off);

#if APP_FLASH_LIGHT_NUMBERS
void app_mesh_led_first_show(void);
void set_flash_light_number(uint8_t number);
void app_mesh_prov_start_timer(void);
#endif
void app_mesh_prov_init_timer(void);
#endif

#endif
