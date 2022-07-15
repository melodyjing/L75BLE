/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_light.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/3/23

	Description :   
       mesh 灯的控制

	Changed Log	:

		Liujian  2021/3/23			- Creation

*****************************************************************************/
#ifndef __COBRA_LIGHT_H__
#define __COBRA_LIGHT_H__
#if (BLE_APP_MESH == 1)
#include "cobra4A_mpwm.h"
#include "m_config.h"

#define LIGHTNESS_MIN      		(1)			// can not set 0
#define LIGHTNESS_MAX      		(0xFFFF)
#define CTL_TEMP_MIN			(0x0320)	// 800
#define CTL_TEMP_MAX			(0x4E20)	// 20000
#define CTL_D_UV_MIN			(-32768)
#define CTL_D_UV_MAX			(32767)
#define HSL_HUE_MIN				(0)
#define HSL_HUE_MAX				(0xFFFF)
#define HSL_SAT_MIN				(0)
#define HSL_SAT_MAX				(0xFFFF)
#define XYL_X_MIN				(0)
#define XYL_X_MAX				(0xFFFF)
#define XYL_Y_MIN				(0)
#define XYL_Y_MAX				(0xFFFF)
#define LC_DEFAULT_MIN	(0x1000)

#define LC_DEFAULT_MIN	(0x1000)

#define ONOFF_DEFAULT   	    (1)
#define LIGHTNESS_DEFAULT   	(LIGHTNESS_MAX)
#define CTL_TEMP_DEFAULT   		(CTL_TEMP_MAX)
#define CTL_D_UV_DEFAULT   		(0)
#define HSL_HUE_DEFAULT   		(HSL_HUE_MAX)
#define HSL_SAT_DEFAULT   		(HSL_SAT_MAX)
#define XYL_X_DEFAULT   		(XYL_X_MAX)
#define XYL_Y_DEFAULT   		(XYL_Y_MAX)

#define PWM_R       PIN5_INDEX		//red
#define PWM_G       PIN27_INDEX		//green
#define PWM_B       PIN15_INDEX		//blue
#define PWM_W       PIN29_INDEX		//white


#define RES_HW_PWM_R    {PWM_R,0}
#define RES_HW_PWM_G    {PWM_G,1}
#define RES_HW_PWM_B    {PWM_B,1}
#define RES_HW_PWM_W    {PWM_W,0}


#define LED_INDICATE_VAL    (rgb_lumen_map[100])
#define LED_INDICATE_LUM_VAL LED_INDICATE_VAL
#define CLOCK_SYS_CLOCK_HZ  (48000000UL)

#define MAX_SCENS			15
#define MAX_SCENS_HSL		10

#define ARRAY_SIZE(a) 			(sizeof(a) / sizeof(*a))

#define CT_MI_MIN		2700
#define CT_MI_MAX		6500

typedef struct{
    float h;
    float s;
    float l;
}HSL_set;

typedef struct{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}RGB_set;


typedef struct{
    uint32_t gpio;
    uint8_t invert;
}light_res_hw_t;


//typedef struct{
//    uint32_t onoff;
//    uint32_t level;
//    uint32_t level_target;
//    uint32_t light_ln;
//    uint32_t light_ln_target;
//		uint32_t light_hsl_h;
//		uint32_t light_hsl_h_target;
//		uint32_t light_hsl_s;
//		uint32_t light_hsl_s_target;
//		uint32_t light_hsl_l;
//		uint32_t light_hsl_l_target;
//		uint32_t light_ctr_ln;
//		uint32_t light_ctr_ln_target;
//		uint32_t light_ctr_temp;
//		uint32_t light_ctr_temp_target;
//		uint16_t scens_id;
//		uint8_t  scens_daz_id;
//		uint8_t  settime_flag;
//		uint32_t seconds;
//		uint32_t time1;
//	  uint32_t trans_time_ms;
//		uint16_t state_id;
//		uint16_t last_state_id;
//		uint8_t  mdl_lid[MESH_MODEL_NB_MAX];
//		uint32_t model_id[MESH_MODEL_NB_MAX];
//		scens_set scens_hsl[MAX_SCENS*MAX_SCENS_HSL];  //scens_hsl.upflag  == 0 no modify; ==1: modify; ==2: del
//		uint8_t  total_mdl;
//		uint8_t  temp_cycle;
//		uint8_t  flash_time;
//		uint16_t app_key_lid;
//		uint8_t publish_time;
//}mesh_ind_status;


void light_pwm_init();

void light_hsl();
void scen_default_hsl(void);
uint8_t light_soft_ctl(void);
void light_dim_set_hw(int idx, uint16_t val);
void spwm_set_lum (uint16_t y, int pol);
void pwm_set_lum (int id, uint16_t y, int pol);


void app_light_onoff_control(void);

void app_tulipi_light_loop(void);

void app_light_tulipi_set_hsl_lightness(uint32_t value);

void app_light_tulipi_set_hsl_hue(uint32_t value);

void app_light_tulipi_set_hsl_sat(uint32_t value);
void app_light_tulipi_set_ctl_temp(uint32_t value);

void app_light_tulipi_set_ctl_lightness(uint32_t value);

uint32_t app_light_is_on(void);
/////////////////////////////////////////////////

typedef struct color_hsl{
	 float h;
	 float s;
   float l;	
}COLOR_HSL;

typedef struct color_rgb{
	uint8_t  r;
	uint8_t  g;
	uint8_t  b;
}COLOR_RGB;

typedef struct color_ct{
	uint8_t  ctl_lightness; 
	uint8_t  ctl_temp;  
	uint8_t  ctl_duv;
}COLOR_CTL;

void app_cobra_light_init(void);
void app_cobra_light_enable (void) ;  
void app_cobra_light_set_duty (unsigned int duty);

//设置蓝光
void app_cobra_light_set_blue_duty(unsigned char duty);

//设置绿光
void app_cobra_light_set_green_duty(unsigned char duty);

//设置红光
void app_cobra_light_set_red_duty(unsigned char duty);

//设置冷光
void app_cobra_light_set_cool_duty(unsigned char duty);

void app_ctl_set_warm_duty(unsigned char duty);

void app_color_hsl_to_rgb(COLOR_HSL * hsl, COLOR_RGB * rgb);

//关掉调色光
void app_cobra_light_disable_white_light(void);

//关掉彩光
void app_cobra_light_disable_color_light(void);
	
// CT 模式下 lightness 转换
uint8_t  app_cobra_light_level_2_lum(uint16_t level);

// use for driver pwm, 0--100 is absolute value, not related to temp range
uint8_t app_cobra_temp_2_temp100(uint16_t temp) ;

static void app_cobra_spwm_set_ctl(unsigned int frequency, unsigned int duty);


void app_cobra_tuiple_light_timer_init(void);
#endif
#endif