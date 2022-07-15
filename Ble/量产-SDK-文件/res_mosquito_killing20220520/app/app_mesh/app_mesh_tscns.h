/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_mesh_tscns.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/2/5

	Description :   Time and schdule
	
*******************************************************************************/

#ifndef __APP_MESH_TSCNS_H__
#define __APP_MESH_TSCNS_H__

#include <stdint.h>

typedef void (*p_set_g_tai_seconds_callback)(uint32_t seconds);

void app_mesh_tscns_init(void);

void app_mesh_tscns_set_tai_seconds(uint32_t seconds);
void app_mesh_tscns_set_zone_offset(uint32_t offset);
void app_mesh_tscns_set_seconds_callback(p_set_g_tai_seconds_callback func);

void app_mesh_tscns_inc_tai_seconds(void);

void app_mesh_tscns_loop_process(void);

#endif
