/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_tulip_comm.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/3/25

	Description :   
	
*******************************************************************************/

#ifndef __APP_TULIPEI_COMM_H__
#define __APP_TULIPEI_COMM_H__
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
#include <stdbool.h>
#include <stdlib.h>
#include "cobra_light.h"

#define SCREEN_IN_WARM_COOL_LIGHT   0             //调光
#define SCREEN_IN_COLOR_LIGHT       1             //彩光
#define SCREEN_IN_SCENE             2             //场景

#define CTL_TEMP_MIN  CT_MI_MIN

#define SCENS_ID_MAX			15
#define SCENS_COLOR_MAX		10

#define DEFAULT_LIGHTNESS	(0x2666)//0x3333



enum {
	T_CUR,
	T_TAR,
	T_MAX,
};

/// Step resolution values
enum trans_time_period_step_resol
{
    /// The step resolution is 100 milliseconds
    TRANS_TIME_PERIOD_STEP_RESOL_100MS = 0,
    /// The step resolution is 1 second
    TRANS_TIME_PERIOD_STEP_RESOL_1S,
    /// The step resolution is 10 seconds
    TRANS_TIME_PERIOD_STEP_RESOL_10S,
    /// The step resolution is 10 minutes
    TRANS_TIME_PERIOD_STEP_RESOL_10M,
};

/// Publish period fields
enum trans_time_period_fields
{
    /// Number of steps
    TRANS_TIME_PERIOD_STEP_NB_MASK    = 0x3F,
    TRANS_TIME_PERIOD_STEP_NB_LSB     = 0,

    /// Step resolution
    TRANS_TIME_PERIOD_STEP_RESOL_MASK = 0xC0,
    TRANS_TIME_PERIOD_STEP_RESOL_LSB  = 6,
};


typedef struct light_ctl_element{
	
   	unsigned int actual_lightness[T_MAX];
	  unsigned int actual_temparture[T_MAX];
	
}LIGHT_CTL_ELEMENT;

typedef struct light_hsl_element{
	
   	unsigned int actual_lightness[T_MAX];
	  unsigned int actual_hue[T_MAX];
	  unsigned int actual_saturation[T_MAX];
	
	  bool          hsl_onff;
	
}LIGHT_HSL_ELEMENT;

typedef struct light_ctl_element_resume{
       
         unsigned int actual_lightness;
         unsigned int actual_temparture;
       
         bool          ctl_onff;   
}LIGHT_CTL_ELEMENT_RESUME;

typedef struct light_hsl_element_resume{
       
         unsigned int actual_lightness;
         unsigned int actual_hue;
         unsigned int actual_saturation;
       
         bool          hsl_onff;
       
}LIGHT_HSL_ELEMENT_RESUME;

typedef struct{

	unsigned int flag;

	unsigned int screen_index;
	unsigned int   g_light_current_scene_index;
	uint8_t global_lum_100;
	LIGHT_CTL_ELEMENT_RESUME  light_ctl; 
	LIGHT_HSL_ELEMENT_RESUME  light_hsl;
}led_resume_set;




typedef struct light_temperature_lightless
{
	  unsigned int  ctl_lightness;           //表示当前灯的亮度 
	  bool          ctl_lightness_changing;  //表示灯现在是否在改变
	
    unsigned int  ctl_temp;                //当前 的 色温
    bool          ctl_temp_changing;       //当前色 温是否在改变		
	
	  bool          ctl_onff;
	
}LIGHT_TEMP_LN;  


typedef struct light_hue_sat_ln{
	  unsigned int  hsl_lightness;   
	  bool          hsl_lightness_changing;
	  
	  unsigned int  hsl_saturate;   
	  bool          hsl_saturate_changing;
	
	  unsigned int  hsl_hue;   
	  bool          hsl_hue_changing;
	
	  bool          hsl_onoff;
	
}LIGHT_HSL;

typedef struct light_scens_t
{
//	uint16_t scens_id_color;
//	uint8_t scens_mode;
	uint8_t end_Flag;
	uint8_t color_type;
	RGB_set scens_rgb;
	uint8_t trans_time;
	uint8_t update_flag;			//0: no modify ; 1: modify ; 2:del
}LIGHT_SCENS;


void app_mesh_led_all_light_on_off(unsigned int light_flag);

void app_tulip_set_screen_index(unsigned int index);
void app_light_tulipi_onff(unsigned int onoff_flag);



void app_mesh_led_control(void);

void app_mesh_led_process(void);

void app_cobra_light_control_light(void);

void app_mesh_lightness_process(void);

void app_mesh_led_init_state(void);

void app_tulip_process_scene_recall(unsigned int scene_index);

void app_scene_set_change_light_init(void);

void app_scene_close_change_light(void);

//定时开灯
void app_tulip_time_schedule_action_on(void);

//定时关灯
void app_tulip_time_schedule_action_off(void);

bool get_ctl_onoff(void);

void app_scens_default_data(void);

void app_mesh_scens_to_flash(void);

void app_mesh_flash_to_scens(void);

void app_light_tulipi_tscns_save(uint8_t *data);

void app_light_tulipi_tscns_dell(uint32_t state);

//初始化灯的状态
void  app_tulip_light_init(void);

uint8_t  app_cobra_light_get_lum(void);

bool get_light_onoff(void);

unsigned int get_index(void);

unsigned int get_screen_index(void);

LIGHT_CTL_ELEMENT get_current_ctl(void);

LIGHT_HSL_ELEMENT get_current_hsl(void);

void led_resume_light_handle();

void app_scene_set_static_light(unsigned char r_color, unsigned char g_color, unsigned char b_color,unsigned char color_type);
void app_scene_set_change_light(const COLOR_RGB * rgb_data, uint32_t count,uint32_t trans_time,unsigned char color_type);
uint32_t app_mesh_trans_time_convert(uint8_t period);

void app_mesh_data_save(void);
bool get_hsl_onoff(void);

void app_cobra_light_set_lum(uint8_t value);

#if (APP_COLOR_TRANSITION_FUNCTION || APP_LIGHT_BREATH_FUNCTION)
void app_gens_color_transition_timer_init(void);

uint16_t app_mesh_get_ctl_curr_lightness(void);

void app_mesh_ctl_set_curr_lightness(uint16_t lightness);

uint16_t app_mesh_get_ctl_tar_lightness(void);

uint16_t app_mesh_get_ctl_curr_temp(void);

void app_mesh_ctl_set_curr_temp(uint16_t temp);

uint16_t app_mesh_get_ctl_tar_temp(void);

/***** hsl******/
unsigned int app_mesh_get_hsl_curr_h(void);

void app_mesh_hsl_set_curr_h(unsigned int hue);

unsigned int app_mesh_get_hsl_tar_h(void);

unsigned int app_mesh_get_hsl_curr_l(void);

void app_mesh_hsl_set_curr_l(unsigned int lightness);

uint16_t app_mesh_get_hsl_tar_l(void);

unsigned int app_mesh_get_hsl_curr_s(void);

void app_mesh_hsl_set_curr_s(unsigned int saturate);

uint16_t app_mesh_get_hsl_tar_s(void);

void app_gens_color_transition_timer_stop(void);
#endif
#if (APP_LIGHT_BREATH_FUNCTION)
void app_scene_close_breath_light(void);
void app_scene_set_breath_light_init(void);
void app_light_ctrl_breathing_timer_init(void);
void app_scene_set_breath_light(const COLOR_RGB * rgb_data, uint32_t count,uint32_t trans_time,unsigned char color_type);
uint8_t get_breath_light_color_type(void);
#endif
#if (COBRA_FLASH_POWERON_CRASE == 1)
led_resume_set get_mesh_screen_para(void);
#endif
#endif


#endif
