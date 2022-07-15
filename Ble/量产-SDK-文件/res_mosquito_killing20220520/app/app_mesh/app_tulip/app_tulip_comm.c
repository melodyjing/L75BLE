/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_tulip_comm.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/3/25

	Description :   
	
*******************************************************************************/

#include "app_tulip_comm.h"

#include "app_tulip_light.h"
#include "app_tulip_scene.h"
#include "cobra_light.h"
#include "cobra_soft_timer.h"
#include "rdw_mcu.h"
#include "cobra4C_eflash_ctrl.h"

#if APP_LIGHT_BREATH_FUNCTION
#include "app_light_breath.h"
#endif


#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))


static LIGHT_TEMP_LN  g_light_ctl = {0};

static LIGHT_HSL      g_light_hsl = {0};


static LIGHT_CTL_ELEMENT  ctl_element;

static LIGHT_HSL_ELEMENT  hsl_element;

unsigned int   g_light_current_scene_index = 0;

unsigned int screen_index;   //0 ctl_screen  1 hsl_screen ;2: scene_screen

static uint32_t light_change_timer_id;    //场景跳变需要用到的时间
static const COLOR_RGB * p_scene_change_rgb;
static const COLOR_RGB * p_scene_change_rgb_begin;

#if APP_LIGHT_BREATH_FUNCTION

static uint32_t light_breath_timer_id;    //场景跳变需要用到的时间
static const LIGHT_HSL_ELEMENT_RESUME * p_scene_breath_hsl;
static const LIGHT_HSL_ELEMENT_RESUME * p_scene_breath_hsl_begin;
void rdw_light_gradual_ctrl_init(const COLOR_RGB * rgb_data1,const COLOR_RGB * rgb_data2);
extern hl_gens_color_breath_env_t p_gens_color_breath_env;
extern hl_gens_color_breath_env_t p_gens_color_breath_env2;
#endif

static uint32_t  light_change_count;
static uint32_t  light_change_index;
static uint8_t global_lum_100;
#if APP_FLASH_LIGHT_NUMBERS
extern bool flash_light_enable;
#endif
#if (COBRA_SCENE_VERSION2_ENABLE==1)
extern scene_data   global_scene_data[SCENS_ID_MAX];
#endif
#if (COBRA_FLASH_POWERON_CRASE == 1)
static led_resume_set mesh_screen_para;
static bool is_mesh_scene_data;
#endif


void app_tulip_set_screen_index(unsigned int index)
{
	  screen_index = index;
	
	  if((index == SCREEN_IN_WARM_COOL_LIGHT) || (index == SCREEN_IN_COLOR_LIGHT))
		{
	       app_scene_close_change_light();
					#if (APP_LIGHT_BREATH_FUNCTION)
	         cobra_soft_timer_stop(light_breath_timer_id);
			     app_light_shade_ctrl_disable();
	        #endif
		}
		else if(index == SCREEN_IN_SCENE)
		{
			  g_light_ctl.ctl_lightness = 0;
			  g_light_ctl.ctl_temp = 0;
			  g_light_hsl.hsl_hue = 0;
			  g_light_hsl.hsl_lightness = 0;
			  g_light_hsl.hsl_saturate = 0;
		}
}




//初始化灯的状态
void  app_tulip_light_init(void)
{
	  screen_index = 0;
	ctl_element.actual_lightness[T_TAR] = DEFAULT_LIGHTNESS;
	
}

void app_light_tulipi_set_hsl_lightness(uint32_t value)
{
	  if(screen_index)
	      hsl_element.actual_lightness[T_TAR] = value;
		else
			  ctl_element.actual_lightness[T_TAR] = value;
}

void app_light_tulipi_set_hsl_hue(uint32_t value)
{
	  hsl_element.actual_hue[T_TAR] = value;
}

void app_light_tulipi_set_hsl_sat(uint32_t value)
{
	  hsl_element.actual_saturation[T_TAR] = value;
}

void app_light_tulipi_set_ctl_temp(uint32_t value)
{
	  ctl_element.actual_temparture[T_TAR] = value;
}

/*
处理开关消息的函数, 手机会发送开关灯的命令过来，
然后使用下面的函数来处理
*/
void app_light_tulipi_onff(uint32_t onoff_flag)
{
	  if(onoff_flag)   //ON action
		{
			  if(screen_index == SCREEN_IN_WARM_COOL_LIGHT)
				{
					  g_light_ctl.ctl_onff = true;
					  g_light_ctl.ctl_lightness = ctl_element.actual_lightness[T_CUR];
					  g_light_ctl.ctl_temp = ctl_element.actual_temparture[T_CUR];
				}
				else if(screen_index == SCREEN_IN_COLOR_LIGHT)
				{
					  g_light_hsl.hsl_onoff = true;
					  g_light_hsl.hsl_hue = hsl_element.actual_hue[T_CUR];
					  g_light_hsl.hsl_lightness = hsl_element.actual_lightness[T_CUR];
					  g_light_hsl.hsl_saturate = hsl_element.actual_saturation[T_CUR];
				}
				else if(SCREEN_IN_SCENE==screen_index)
				{
					  
					#if APP_LIGHT_BREATH_FUNCTION
					  if(global_scene_data[g_light_current_scene_index-1].scene_mode == SCENE_CHANGED \
							|| global_scene_data[g_light_current_scene_index-1].scene_mode == SCENE_BREATHE)
						{
	//						app_tulip_process_scene_recall(g_light_current_scene_index);
							  if(global_scene_data[g_light_current_scene_index-1].color_type[0] == 0)
									g_light_hsl.hsl_onoff = true;
								else
									g_light_ctl.ctl_onff = true;
						}
					  else if(global_scene_data[g_light_current_scene_index-1].scene_mode == SCENE_STATIC)
						{
					#endif
#if (COBRA_SCENE_VERSION2_ENABLE==0)					
						if(g_light_scens[(g_light_current_scene_index-1)*SCENS_COLOR_MAX+light_change_index].color_type == 0)
#else
						if(global_scene_data[g_light_current_scene_index-1].color_type[0] == 0)
#endif					
						{
							g_light_hsl.hsl_onoff = true;
							g_light_hsl.hsl_hue = hsl_element.actual_hue[T_CUR];
							g_light_hsl.hsl_lightness = hsl_element.actual_lightness[T_CUR];
							g_light_hsl.hsl_saturate = hsl_element.actual_saturation[T_CUR];
						}
						else
						{
							g_light_ctl.ctl_onff = true;
							g_light_ctl.ctl_lightness = ctl_element.actual_lightness[T_CUR];
							g_light_ctl.ctl_temp = ctl_element.actual_temparture[T_CUR];														
						}
						#if APP_LIGHT_BREATH_FUNCTION
					  }
						#endif
				}
		}
		else            //OFF action
		{
			  if(screen_index == SCREEN_IN_WARM_COOL_LIGHT)
				{
					  g_light_ctl.ctl_onff = false;
					  g_light_ctl.ctl_lightness = 0;
					  g_light_ctl.ctl_temp = CTL_TEMP_MIN;
				}
				else if(screen_index == SCREEN_IN_COLOR_LIGHT)
				{
					  g_light_hsl.hsl_onoff = false;
					  g_light_hsl.hsl_hue = 0;
					  g_light_hsl.hsl_lightness = 0;
					  g_light_hsl.hsl_saturate = 0;
				}
				else if(SCREEN_IN_SCENE==screen_index)
				{
					printf("app_light_tulipi_onff\n");
					  g_light_hsl.hsl_onoff = false;
						app_scene_close_change_light();
						#if (APP_LIGHT_BREATH_FUNCTION)
					  g_light_ctl.ctl_onff = false;
	          cobra_soft_timer_stop(light_breath_timer_id);
					  app_light_shade_ctrl_disable();
						g_light_hsl.hsl_hue = 0;
						g_light_hsl.hsl_lightness = 0;
						g_light_hsl.hsl_saturate = 0;
						g_light_ctl.ctl_lightness = 0;
						g_light_ctl.ctl_temp = CTL_TEMP_MIN;
	          #endif

						if(global_scene_data[g_light_current_scene_index-1].color_type[0] == 0)			
						{
							g_light_hsl.hsl_hue = 0;
							g_light_hsl.hsl_lightness = 0;
							g_light_hsl.hsl_saturate = 0;
						}
						else
						{
							g_light_ctl.ctl_lightness = 0;
							g_light_ctl.ctl_temp = CTL_TEMP_MIN;
						}
				}
		}
		
		if(screen_index == SCREEN_IN_WARM_COOL_LIGHT)
		{
			  if(g_light_ctl.ctl_onff)
				{
					  app_cobra_light_control_light();    //调光亮
				}
				else
				{
					  app_cobra_light_disable_white_light();   //调光关掉
				}
		}
		else if(screen_index == SCREEN_IN_COLOR_LIGHT)
		{
			  if(g_light_hsl.hsl_onoff)
				{
		        app_mesh_led_process();
				}
				else
				{
					  app_cobra_light_disable_color_light();
				}
		}
		else if(SCREEN_IN_SCENE==screen_index)
		{

				if(global_scene_data[g_light_current_scene_index-1].color_type[0] == 0)
				{
					if(g_light_hsl.hsl_onoff)
					{
							app_tulip_process_scene_recall(g_light_current_scene_index);
					}
					else
					{
							app_cobra_light_disable_color_light();
							app_cobra_light_disable_white_light();   //调光关掉
					}
				}
				else
				{
					if(g_light_ctl.ctl_onff)
					{
							app_tulip_process_scene_recall(g_light_current_scene_index);
					}
					else
					{
							app_cobra_light_disable_color_light();
							app_cobra_light_disable_white_light();   //调光关掉
					}
				}
			
		}
		
}


void app_mesh_led_control(void)
{
	  HSL_set HSL;
    RGB_set RGB;
	
	  
	
	  if((g_light_hsl.hsl_hue != hsl_element.actual_hue[T_TAR]) ||  \
			(g_light_hsl.hsl_lightness != hsl_element.actual_lightness[T_TAR]) ||  \
		  (g_light_hsl.hsl_saturate != hsl_element.actual_saturation[T_TAR]))
		{
			   g_light_hsl.hsl_hue = hsl_element.actual_hue[T_TAR];
			   g_light_hsl.hsl_lightness = hsl_element.actual_lightness[T_TAR];
			   g_light_hsl.hsl_saturate = hsl_element.actual_saturation[T_TAR];
			
			  app_mesh_led_process();
			
			  hsl_element.actual_hue[T_CUR] = hsl_element.actual_hue[T_TAR];
				hsl_element.actual_lightness[T_CUR] = hsl_element.actual_lightness[T_TAR];
				hsl_element.actual_saturation[T_CUR] = hsl_element.actual_saturation[T_TAR];
			
		}
	
}

void app_mesh_led_process(void)
{
	  COLOR_HSL HSL;
    COLOR_RGB RGB;
	
	  HSL.h = ((float)(g_light_hsl.hsl_hue)/65535.0f);
		HSL.s = ((float)(g_light_hsl.hsl_saturate)/65535.0f);
		HSL.l = ((float)(g_light_hsl.hsl_lightness)/65535.0f);
	
		app_color_hsl_to_rgb(&HSL,&RGB);
			
	  app_cobra_light_set_blue_duty(RGB.b);
		app_cobra_light_set_green_duty(RGB.g);
		app_cobra_light_set_red_duty(RGB.r);
	
		printf("R = %d , G = %d, B = %d \n", RGB.r, RGB.g, RGB.b);
	
	  app_cobra_light_disable_white_light();
	
}

void app_cobra_light_set_lum(uint8_t value)
{
	 global_lum_100 = value;
}

uint8_t  app_cobra_light_get_lum(void)
{
	  return global_lum_100;
}

//调光函数
void app_cobra_light_control_light(void)
{
	  uint8_t lum_100;
	  uint8_t ct_100;
	  float   temp_cool;
	  float   temp_warm;
	  uint8_t cool_duty = 0;
	  uint8_t warm_duty;
	
	  if((g_light_ctl.ctl_lightness != ctl_element.actual_lightness[T_TAR]) || \
			  (g_light_ctl.ctl_temp != ctl_element.actual_temparture[T_TAR]))
		{
			  g_light_ctl.ctl_lightness = ctl_element.actual_lightness[T_TAR];
			  g_light_ctl.ctl_temp  = ctl_element.actual_temparture[T_TAR];
			
			  lum_100 = app_cobra_light_level_2_lum(g_light_ctl.ctl_lightness);
			
			  if(lum_100<1)
					lum_100 = 1;
				
			  ct_100 = 0;
		
        if(lum_100 != 0)
			  {
	        ct_100 = app_cobra_temp_2_temp100(g_light_ctl.ctl_temp);
					if(ct_100>98)
						ct_100 =100;
			  }
				
				printf("ct_100 = %d ,  lum_100 = %d \n", ct_100, lum_100);
				

#if (SOFT_PWM == 1)				
				temp_cool = (100 - ct_100);
				temp_cool *= lum_100;
				temp_cool /= 100.0;
		//		temp_cool = (100 - ct_100)*lum_100/100.0;
				temp_cool /= 100.0;
				cool_duty = temp_cool*255;
				
				app_cobra_light_set_cool_duty(cool_duty);
#endif				

		
#if (SOFT_PWM == 1)
				warm_duty = ct_100*lum_100/100;
#else				
				temp_cool = lum_100*ct_100;
				temp_cool /= 100.0;
				warm_duty = temp_cool;
#endif
				app_cobra_light_set_lum(lum_100);
				app_ctl_set_warm_duty(warm_duty);
				
				printf("cool_duty = %d,  warm_duty = %d \n", cool_duty, warm_duty);
				
				app_cobra_light_disable_color_light();

		}
}


void app_mesh_lightness_process(void)
{
	  if(screen_index == SCREEN_IN_COLOR_LIGHT)    //调色, 彩光
	  {
			 if(g_light_hsl.hsl_onoff)   //只有在灯亮着的时候才可以工作
			 {
	        app_mesh_led_control();	
			 }				 
	  }
    else if(screen_index == SCREEN_IN_WARM_COOL_LIGHT)    //调光 
	  {
			 if(g_light_ctl.ctl_onff)  //只有在灯亮着的时候才可以工作
			 {
         app_cobra_light_control_light();
			 }
	  }	
	
}


void app_mesh_led_all_light_on_off(unsigned int light_flag)
{
	  
		app_cobra_light_set_green_duty(0);
	  app_cobra_light_set_red_duty(0);
#if (SOFT_PWM == 1)	
	  app_ctl_set_warm_duty(0);
	  app_cobra_light_set_cool_duty(0);
#else
		app_cobra_light_disable_white_light();
#endif	
	
	
    if(light_flag)
		{
			 app_cobra_light_set_blue_duty(255);
		}
		else
		{
			 app_cobra_light_set_blue_duty(0); 
		}
}


//进入界面后设置灯的初始状态

void app_mesh_led_init_state(void)
{
	      app_cobra_light_set_blue_duty(0);
		    app_cobra_light_set_green_duty(0);
				app_cobra_light_set_red_duty(0);

#if APP_FLASH_LIGHT_NUMBERS
        app_cobra_light_disable_white_light();
#else	
#if (SOFT_PWM == 1)	
	      app_ctl_set_warm_duty(0);
	      app_cobra_light_set_cool_duty(0);
#else
				app_ctl_set_warm_duty(0);		  
#endif	
#endif		
	 
}

/**
 ****************************************************************************************
 * @brief Convert received publish period value in milliseconds.
 *
 * @param[in] period        Receive publish period
 *
 * @return Publish period in milliseconds
 ****************************************************************************************
 */
uint32_t app_mesh_trans_time_convert(uint8_t period)
{
    // Publish period in milliseconds
    uint32_t period_ms = 0;
    // Get number of steps
    uint8_t step_nb = period & 0x3f;
    // Get step resolution
    uint8_t step_resol = (period>>6)&0x03;

    // Convert the step resolution in milliseconds
    switch (step_resol)
    {
        case (TRANS_TIME_PERIOD_STEP_RESOL_100MS):
        {
            period_ms = 100;
        } break;

        case (TRANS_TIME_PERIOD_STEP_RESOL_1S):
        {
            period_ms = 1000;
        } break;

        case (TRANS_TIME_PERIOD_STEP_RESOL_10S):
        {
            period_ms = 10000;
        } break;

        case (TRANS_TIME_PERIOD_STEP_RESOL_10M):
        {
            period_ms = 600000;
        } break;

        default:
        {
        } break;
    }

    // Multiply by the number of steps
    period_ms *= step_nb;

    // Return the computed value
    return (period_ms);
}



//处理恢复场景
void app_tulip_process_scene_recall(unsigned int scene_index)
{
	  
	  g_light_current_scene_index = scene_index;

	  app_tulipe_scene_color_process(scene_index);		
}


//设置静态场景
void app_scene_set_static_light(unsigned char r_color, unsigned char g_color, unsigned char b_color,unsigned char color_type)
{
	
	   cobra_soft_timer_stop(light_change_timer_id);

#if (APP_LIGHT_BREATH_FUNCTION)
	   cobra_soft_timer_stop(light_breath_timer_id);
	   app_light_shade_ctrl_disable();
#endif
		 if(color_type ==0)
		 {
#if (SOFT_PWM == 1)	
	      app_ctl_set_warm_duty(0);
	      app_cobra_light_set_cool_duty(0);
#else
		    app_cobra_light_disable_white_light();
#endif	
	        
			  app_cobra_light_set_red_duty(r_color);
			  app_cobra_light_set_green_duty(g_color);
			  app_cobra_light_set_blue_duty(b_color);	   
		 }
		 else
		 {
			  app_cobra_light_disable_color_light();
#if (SOFT_PWM == 1)	
			  app_ctl_set_warm_duty(g_color);
			  app_cobra_light_set_cool_duty(r_color);
#else
			 app_cobra_light_set_lum(b_color);
			 app_ctl_set_warm_duty(g_color);			 
#endif	
		 }
} 

void app_scene_change_light_callback(uint32_t  param)
{
	//		printf("R = %d G = %d, B = %d , count = %d \n", 	\
//		  p_scene_change_rgb->r, p_scene_change_rgb->g, p_scene_change_rgb->b, light_change_count);
	
#if APP_FLASH_LIGHT_NUMBERS
      if(flash_light_enable==false)  // 防止和蓝灯闪，冲突。
			{
#endif	

			if(global_scene_data[g_light_current_scene_index-1].color_type[light_change_index]==0)

			{
#if (SOFT_PWM == 1)	
		    app_ctl_set_warm_duty(0);
		    app_cobra_light_set_cool_duty(0);
#else
		    app_cobra_light_disable_white_light();
#endif	
		    app_cobra_light_set_red_duty(p_scene_change_rgb->r);
		    app_cobra_light_set_green_duty(p_scene_change_rgb->g);
		    app_cobra_light_set_blue_duty(p_scene_change_rgb->b);
	  }
	  else
	  {
		     app_cobra_light_disable_color_light();
#if (SOFT_PWM == 1)	
		     app_ctl_set_warm_duty(p_scene_change_rgb->g);
		     app_cobra_light_set_cool_duty(p_scene_change_rgb->r);
#else
			   app_cobra_light_set_lum(p_scene_change_rgb->b);
			   app_ctl_set_warm_duty(p_scene_change_rgb->g);
#endif					
	}
#if APP_FLASH_LIGHT_NUMBERS
}
#endif		
	p_scene_change_rgb++;
	light_change_index++;

	 
	  if(light_change_index >= light_change_count)
	  {
		     p_scene_change_rgb = p_scene_change_rgb_begin;
		     light_change_index = 0;
	  }
			
}

//设置跳变场景
void app_scene_set_change_light(const COLOR_RGB * rgb_data, uint32_t count,uint32_t trans_time,unsigned char color_type)
{
    // Publish period in milliseconds
	  if(rgb_data == NULL) return;

	  p_scene_change_rgb = rgb_data;
	  p_scene_change_rgb_begin = rgb_data;
	  
	  light_change_count = count;
	  light_change_index = 1;
    p_scene_change_rgb++;

		#if (APP_LIGHT_BREATH_FUNCTION)
	   cobra_soft_timer_stop(light_breath_timer_id);
	   app_light_shade_ctrl_disable();
	  #endif
	
	  cobra_soft_timer_stop(light_change_timer_id);
		
	  cobra_soft_timer_start(light_change_timer_id, COBRA_SOFT_TIMER_TICK*trans_time/10, 1);
}

void app_scene_set_change_light_init(void)
{
	  light_change_timer_id = cobra_soft_timer_create(app_scene_change_light_callback);
}

void app_scene_close_change_light(void)
{
	 cobra_soft_timer_stop(light_change_timer_id);
}
void app_scene_close_breath_light(void)
{
cobra_soft_timer_stop(light_breath_timer_id);
}

//定时开灯
void app_tulip_time_schedule_action_on(void)
{
	   if((screen_index == SCREEN_IN_COLOR_LIGHT) || (screen_index == SCREEN_IN_WARM_COOL_LIGHT))    //调色, 彩光
		 {
			   app_light_tulipi_onff(1);
		 }
		 else if(screen_index == SCREEN_IN_SCENE)
		 {
			  app_tulip_process_scene_recall(g_light_current_scene_index);
		 }
		 app_mesh_key_response_on_off_state(1);
}


//定时关灯
void app_tulip_time_schedule_action_off(void)
{
	  if(screen_index == SCREEN_IN_SCENE)
		{
	     app_scene_close_change_light();
			 #if (APP_LIGHT_BREATH_FUNCTION)
	     cobra_soft_timer_stop(light_breath_timer_id);
			 app_light_shade_ctrl_disable();
	     #endif
	  }
		
		else if(screen_index == SCREEN_IN_COLOR_LIGHT)
		{
			 g_light_hsl.hsl_onoff = false;
	     g_light_hsl.hsl_hue = 0;
	     g_light_hsl.hsl_lightness = 0;
	     g_light_hsl.hsl_saturate = 0;
		}
		
		else if(screen_index == SCREEN_IN_WARM_COOL_LIGHT)
		{
			 g_light_ctl.ctl_onff = false;
	     g_light_ctl.ctl_lightness = 0;
	     g_light_ctl.ctl_temp = 0;
		}
		
	  app_mesh_led_all_light_on_off(0);
		
		app_mesh_key_response_on_off_state(0);
		  
}

bool get_ctl_onoff(void)
{
	return g_light_ctl.ctl_onff;
}


void app_mesh_scens_to_flash(void)
{
	app_tulipe_scense_save();
}


void app_mesh_flash_to_scens(void)
{
	  app_tulipe_scene_color_init();
}


bool get_hsl_onoff(void)
{
	return g_light_hsl.hsl_onoff;
}

bool get_light_onoff(void)
{	
	return (g_light_hsl.hsl_onoff | g_light_ctl.ctl_onff | (screen_index == SCREEN_IN_SCENE));
}

unsigned int get_index(void)
{
	return screen_index;
}

unsigned int get_screen_index(void)
{
	return g_light_current_scene_index;
}

LIGHT_CTL_ELEMENT get_current_ctl(void)
{
	return ctl_element;
}


LIGHT_HSL_ELEMENT get_current_hsl(void)
{
    return  hsl_element;
}

void led_resume_light_handle()
{	
    led_resume_set flash_read_para;
	  led_resume_set led_resume_para; 
	
#if (COBRA_FLASH_POWERON_CRASE == 1)
    flash_read_para = get_mesh_screen_para();	
#else	
    uint8_t *p_addr = (uint8_t * )M4_EFLASH_SCREEN_ADDRESS;
	
    memcpy(&flash_read_para, p_addr, sizeof(led_resume_set));
#endif	
	
	  memset(&led_resume_para, 0, sizeof(led_resume_set));
    
	  if(flash_read_para.flag== 0xAABBCCDD)
	  {
        memcpy(&led_resume_para, &flash_read_para, sizeof(led_resume_set));
		    app_cobra_light_set_lum(led_resume_para.global_lum_100);
	  }
	  else
	  {
		    led_resume_para.screen_index = SCREEN_IN_WARM_COOL_LIGHT;
		    led_resume_para.light_ctl.actual_lightness = DEFAULT_LIGHTNESS;
		    led_resume_para.light_ctl.ctl_onff = 1;
//			  printf("led_resume_light_handle1\n");
			
				#if APP_FLASH_LIGHT_NUMBERS
        app_mesh_led_first_show();
			  return;
			  #endif
	  }
		
	
    screen_index = led_resume_para.screen_index;
		
    if(SCREEN_IN_SCENE == screen_index)
    {
           app_tulip_process_scene_recall(led_resume_para.g_light_current_scene_index);
    }
    else
    {
           if(screen_index == SCREEN_IN_COLOR_LIGHT)
           {
           		    led_resume_para.light_hsl.hsl_onff   = 1;		//always on
                  hsl_element.actual_hue[T_TAR]        = led_resume_para.light_hsl.actual_hue;
                  hsl_element.actual_lightness[T_TAR]  = led_resume_para.light_hsl.actual_lightness;
                  hsl_element.actual_saturation[T_TAR] = led_resume_para.light_hsl.actual_saturation;
                  g_light_hsl.hsl_onoff                = led_resume_para.light_hsl.hsl_onff;
           }
           else
           {
           		    led_resume_para.light_ctl.ctl_onff   = 1;		//always on
                  ctl_element.actual_lightness[T_TAR]  = led_resume_para.light_ctl.actual_lightness;
                  ctl_element.actual_temparture[T_TAR] = led_resume_para.light_ctl.actual_temparture;
                  g_light_ctl.ctl_onff                 = led_resume_para.light_ctl.ctl_onff;
           }
					 
           app_mesh_lightness_process();
    }
  
}

void app_mesh_data_save(void)
{
    led_resume_set save_data;
	
    LIGHT_CTL_ELEMENT ctl = get_current_ctl();
    LIGHT_HSL_ELEMENT hsl = get_current_hsl();
    save_data.screen_index = get_index();
    save_data.g_light_current_scene_index = get_screen_index();
    save_data.global_lum_100 = app_cobra_light_get_lum();
    
    save_data.light_ctl.actual_lightness = ctl.actual_lightness[T_TAR];
    save_data.light_ctl.actual_temparture = ctl.actual_temparture[T_TAR];
    save_data.light_ctl.ctl_onff = get_ctl_onoff();
    
    save_data.light_hsl.actual_lightness = hsl.actual_lightness[T_TAR];
    save_data.light_hsl.actual_hue = hsl.actual_hue[T_TAR];
    save_data.light_hsl.actual_saturation = hsl.actual_saturation[T_TAR];
    save_data.light_hsl.hsl_onff = get_hsl_onoff();

    
    save_data.flag = 0xAABBCCDD;
    cobra_flash_write(M4_EFLASH_SCREEN_ADDRESS, sizeof(save_data),  (unsigned char *)&save_data);
    
    app_mesh_scens_to_flash();
    
    cobra_cache_normal_mode_enable(0);
    cobra_cache_normal_mode_enable(1);

}
#if (APP_LIGHT_BREATH_FUNCTION)

void app_mesh_led_process1(void)
{
	  COLOR_HSL HSL;
    COLOR_RGB RGB;
	
	//  printf("*h=%x s=%x l=%x",g_light_hsl.hsl_hue,g_light_hsl.hsl_saturate,g_light_hsl.hsl_lightness);
	
	  HSL.h = ((float)(g_light_hsl.hsl_hue)/255.0f);
		HSL.s = ((float)(g_light_hsl.hsl_saturate)/255.0f);
		HSL.l = ((float)(g_light_hsl.hsl_lightness)/255.0f);

		app_color_hsl_to_rgb(&HSL,&RGB);
			
	  app_cobra_light_set_blue_duty(RGB.b);
		app_cobra_light_set_green_duty(RGB.g);
		app_cobra_light_set_red_duty(RGB.r);
	
	//	printf("R = %d , G = %d, B = %d \n", RGB.r, RGB.g, RGB.b);
	
	  app_cobra_light_disable_white_light();
	
}
#endif


#if (APP_COLOR_TRANSITION_FUNCTION || APP_LIGHT_BREATH_FUNCTION)
/***** ctl******/
uint16_t app_mesh_get_ctl_curr_lightness(void)
{
	   return g_light_ctl.ctl_lightness;
}

void app_mesh_ctl_set_curr_lightness(uint16_t lightness)
{
	  g_light_ctl.ctl_lightness = lightness;
}

uint16_t app_mesh_get_ctl_tar_lightness(void)
{
	  return ctl_element.actual_lightness[T_TAR];
}

uint16_t app_mesh_get_ctl_curr_temp(void)
{
	  return g_light_ctl.ctl_temp;
}

void app_mesh_ctl_set_curr_temp(uint16_t temp)
{
	  g_light_ctl.ctl_temp = temp;
}

uint16_t app_mesh_get_ctl_tar_temp(void)
{
	  return ctl_element.actual_temparture[T_TAR];
}

/***** hsl******/
unsigned int app_mesh_get_hsl_curr_h(void)
{
	  return g_light_hsl.hsl_hue;
}

void app_mesh_hsl_set_curr_h(unsigned int hue)
{
	  g_light_hsl.hsl_hue = hue;
}

unsigned int app_mesh_get_hsl_tar_h(void)
{
	  return hsl_element.actual_hue[T_TAR];
}

unsigned int app_mesh_get_hsl_curr_l(void)
{
	  return g_light_hsl.hsl_lightness;
}

void app_mesh_hsl_set_curr_l(unsigned int lightness)
{
	  g_light_hsl.hsl_lightness = lightness;
}

uint16_t app_mesh_get_hsl_tar_l(void)
{
	  return hsl_element.actual_lightness[T_TAR];
}

unsigned int app_mesh_get_hsl_curr_s(void)
{
	   return g_light_hsl.hsl_saturate;
}

void app_mesh_hsl_set_curr_s(unsigned int saturate)
{
	   g_light_hsl.hsl_saturate = saturate;
}

uint16_t app_mesh_get_hsl_tar_s(void)
{
	   return hsl_element.actual_saturation[T_TAR];
}

#endif

#if APP_LIGHT_BREATH_FUNCTION


void app_scene_breath_light_timer_callback(uint32_t  param)
{
 	//	  printf("h = %d s = %d, l = %d , count = %d \n", 	\
		  p_scene_breath_hsl->actual_hue, p_scene_breath_hsl->actual_saturation, p_scene_breath_hsl->actual_lightness, light_change_count);

	   p_gens_color_breath_env.state = 0;
	
	   if(light_change_index+1>= light_change_count)
	   {
		     if(global_scene_data[g_light_current_scene_index-1].color_type[light_change_index]==0 && \
			              global_scene_data[g_light_current_scene_index-1].color_type[0]==0)
		     {
					 
			       app_cobra_light_disable_white_light();
					 
		     }
		     else if(global_scene_data[g_light_current_scene_index-1].color_type[light_change_index]==1 && \
			             global_scene_data[g_light_current_scene_index-1].color_type[0]==1)
		     {
					 
			       app_cobra_light_disable_color_light();
					 
		     }
			p_gens_color_breath_env.coolorcolor  = global_scene_data[g_light_current_scene_index-1].color_type[light_change_index];
		p_gens_color_breath_env2.coolorcolor = global_scene_data[g_light_current_scene_index-1].color_type[0];
		rdw_light_gradual_ctrl_init(p_scene_change_rgb,p_scene_change_rgb_begin);
     }
	   else
	   {
	        if(global_scene_data[g_light_current_scene_index-1].color_type[light_change_index]==0 && \
			          global_scene_data[g_light_current_scene_index-1].color_type[light_change_index+1]==0)
		      {
						
			         app_cobra_light_disable_white_light();
						
		      }
		      else if(global_scene_data[g_light_current_scene_index-1].color_type[light_change_index]==1 && \
			         global_scene_data[g_light_current_scene_index-1].color_type[light_change_index+1]==1)
		      {
						
			          app_cobra_light_disable_color_light();
						
		      }
				p_gens_color_breath_env.coolorcolor  = global_scene_data[g_light_current_scene_index-1].color_type[light_change_index];
		p_gens_color_breath_env2.coolorcolor = global_scene_data[g_light_current_scene_index-1].color_type[light_change_index+1];		
		rdw_light_gradual_ctrl_init(p_scene_change_rgb,(p_scene_change_rgb+1));
	}
	

	
	p_scene_change_rgb++;
	light_change_index++;

	 printf("light_change_index = %d light_change_count = %d,",light_change_index,light_change_count);
	    if(light_change_index >= light_change_count)
	    {
		       p_scene_change_rgb = p_scene_change_rgb_begin;
		       light_change_index = 0;
	    }
			
}

//////////////////////////////

void app_scene_set_breath_light_init(void)
{
	  light_breath_timer_id = cobra_soft_timer_create(app_scene_breath_light_timer_callback);
}


//设置呼吸场景
void app_scene_set_breath_light(const COLOR_RGB * rgb_data, uint32_t count,uint32_t trans_time,unsigned char color_type)
{
    // Publish period in milliseconds
	printf("app_scene_set_breath_light %xr\n",light_change_timer_id);
	  if(rgb_data == NULL) return;

	  p_scene_change_rgb = rgb_data;
	  p_scene_change_rgb_begin = rgb_data;
	  
	  light_change_count = count;
	  light_change_index = 0;
	
//	  memset(&p_gens_color_breath_env, 0, sizeof(hl_gens_color_breath_env_t));	
	
	  cobra_soft_timer_stop(light_change_timer_id);
	
	  cobra_soft_timer_stop(light_breath_timer_id);
	  
	  app_light_shade_ctrl_disable();
		
	  cobra_soft_timer_start(light_breath_timer_id, COBRA_SOFT_TIMER_TICK*trans_time/10, 1);
	
	  p_gens_color_breath_env.transtime = trans_time;
	
	  app_scene_breath_light_timer_callback(0);
}
uint8_t get_breath_light_color_type(void)
{
	return global_scene_data[g_light_current_scene_index-1].color_type[light_change_index];
}
#endif

#if (COBRA_FLASH_POWERON_CRASE == 1)
void read_mesh_screen(void)
{
	uint8_t *p_addr = (uint8_t * )M4_EFLASH_SCREEN_ADDRESS;
	memcpy(&mesh_screen_para, p_addr, sizeof(led_resume_set));
	printf("mesh_screen_para=%x",mesh_screen_para.flag);
}
led_resume_set get_mesh_screen_para(void)
{
	return mesh_screen_para;
}
 
bool read_mesh_scene_data(void)
{
	volatile uint32_t * p_addr = (volatile uint32_t * )APP_MESH_TULIPX_SCENS_DATA;
	if(p_addr[0] != 0xffffffff)
	{
    memcpy(&global_scene_data, p_addr, sizeof(global_scene_data));
		return true;
	}
	else return false;
}
void set_mesh_scene_data(bool b)
{
	is_mesh_scene_data = b;
}
bool get_mesh_scene_data(void)
{
	return is_mesh_scene_data;
}
#endif
#endif