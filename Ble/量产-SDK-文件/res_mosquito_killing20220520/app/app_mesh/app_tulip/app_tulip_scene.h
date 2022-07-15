/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Application for 

	File Name	:	app_tulip_scene.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/4/15
	
*******************************************************************************/
#ifndef __APP_TULIPE_SCENE_H__
#define __APP_TULIPE_SCENE_H__

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))

#include <stdint.h>
#include "cobra_light.h"

#define SCENE_STATIC   1    //静态
#define SCENE_CHANGED  2    //跳变
#define SCENE_BREATHE  3    //呼吸

#define SCENE_NUM_MAX	(15)
#define COLOR_NUM_MAX	(10)

typedef struct {
	
	uint8_t   color_type;
	uint8_t  color_1;
	
	uint8_t  color_2;
	
	uint8_t  color_3;	
	uint8_t   trans_time;
	
}color_data;


typedef struct {
	
   uint8_t  scene_mode;    //场景模式 
  
   uint8_t  color_type[COLOR_NUM_MAX]; 	// 颜色类型    0, HSL  1 调光
	
	 uint8_t  trans_time[COLOR_NUM_MAX];
	
	 uint8_t  color_count;
	
	 union
	 {
	     COLOR_RGB   rgb_array[COLOR_NUM_MAX];
		   COLOR_CTL   ctl_array[COLOR_NUM_MAX];
	 }array;
	 
//	#if (APP_LIGHT_BREATH_FUNCTION)
//  LIGHT_HSL_ELEMENT_RESUME	scens_hsl[COLOR_NUM_MAX];
//	#endif	 
   uint8_t update_flag;            //0: no modify ; 1: modify ; 2:del
}scene_data;



typedef struct {
	
	  uint8_t   scene_index;
	  uint8_t   mode_flag;
	  uint8_t   len;	
	  color_data  data[COLOR_NUM_MAX];		
}scene_edit_cmd;

void app_tulipe_scene_color_init(void);

void app_tulipe_scene_process(scene_edit_cmd *p_cmd);

void app_tulipe_scense_save(void);

void app_tulipe_scene_color_process(unsigned int scene_index);

#endif

#endif
