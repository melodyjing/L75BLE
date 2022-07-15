/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_mesh_tscns.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/2/5

	Description :   Time and schdule
	
*******************************************************************************/
#include "rwip_config.h"  
#include "app_mesh_tscns.h"
#include <stdio.h>

#if (BLE_APP_MESH == 1)

static uint32_t g_tai_seconds;
static uint32_t g_tai_seconds_old;
static uint8_t  g_zone_offset;
static p_set_g_tai_seconds_callback  p_app_mesh_set_seconds_callback;

void mesh_scheduler_proc();

void app_mesh_tscns_init(void)
{
	   g_tai_seconds = 0;
	   g_zone_offset = 0;
	   p_app_mesh_set_seconds_callback = NULL;
}


void app_mesh_tscns_set_tai_seconds(uint32_t seconds)
{
	   g_tai_seconds = seconds;	
	   g_tai_seconds_old = g_tai_seconds;
}

void app_mesh_tscns_inc_tai_seconds(void)
{
	    if(g_tai_seconds != 0)
			{
	   	    g_tai_seconds++;
			}
}

void app_mesh_tscns_set_zone_offset(uint32_t offset)
{
	    g_zone_offset = offset;
}

void app_mesh_tscns_set_seconds_callback(p_set_g_tai_seconds_callback func)
{
   	p_app_mesh_set_seconds_callback = func;	
}


void app_mesh_tscns_loop_process(void)
{
	   if(g_tai_seconds == 0)  return;
	   if(g_tai_seconds_old != g_tai_seconds)
		 {
			   g_tai_seconds_old = g_tai_seconds;
			   if(p_app_mesh_set_seconds_callback != NULL)
				 {
					  p_app_mesh_set_seconds_callback(g_tai_seconds_old);
				 } 
		 }
}


static inline int16_t get_time_zone_offset_min(uint8_t zone_offset)
{
    return (int16_t)(((int32_t)zone_offset - 64)*15);
}


uint32_t get_local_tai()
{
    return (g_tai_seconds_old + get_time_zone_offset_min(g_zone_offset)*60);
}

#endif

