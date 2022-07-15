/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_mesh_util.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2020/8/5

	Description :   一些简单的工具, 方便写 应用
	
*******************************************************************************/
#ifndef __APP_MESH_UTIL_H__
#define __APP_MESH_UTIL_H__
#if (BLE_APP_MESH == 1)
#include <stdint.h>
#include <stdbool.h>
#define UTC_BASE_YEAR   1970
#define MONTH_PER_YEAR  12
#define DAY_PER_YEAR    365
#define SEC_PER_DAY     86400
#define SEC_PER_HOUR    3600
#define SEC_PER_MIN     60


void app_mesh_util_unix_time(long linux_time);

void app_mesh_util_init_mesh_eflash(void);

void app_mesh_util_write_flag(void);
	
void app_mesh_util_erase_flag(void);

void app_mesh_task_storage_config(void);

void app_tulip_add_module_ctl(uint8_t elmt_idx, bool main);

void app_mesh_stop_advertising(void);

void app_mesh_drop_count(void);

uint8_t get_repeat_reset();

void set_repeat_reset(uint8_t value);

#endif

#endif
