/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:LuFeng
	
	Cobra Drivers for light breath function

	File Name	:	app_light_breath.c
	Version		:	0.1
	Created	By	:	LuFeng
	Date		:	   2021/4/29

	Description :   
	
*******************************************************************************/

#include "app_tulip_comm.h"
#include "app_tulip_scene.h"
#include "cobra_light.h"
#include "cobra_soft_timer.h"
#include "rdw_mcu.h"
#include "cobra4C_eflash_ctrl.h"

#include "app_light_breath.h"
#include <string.h>

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))

#if (APP_LIGHT_BREATH_FUNCTION)

hl_gens_color_breath_env_t p_gens_color_breath_env;
hl_gens_color_breath_env_t p_gens_color_breath_env2;
CTRL_DEF_COLOR_E  light_breath_type;
//BREATH_COLOR_RGB  breath_color;
#if APP_FLASH_LIGHT_NUMBERS
extern bool flash_light_enable;
#endif
void app_cobra_light_set_lum(uint8_t value);
uint8_t get_timer_callback_used_enable(void);
void set_timer_callback_used_enable(uint8_t b);

const uint16_t sBreathParm[]  = {1000, 999, 997, 994, 989, 982, 975, 966, 956, 944, 
                                    932,  918, 903, 887, 870, 853, 834, 815, 795, 774,
                                    753,  731, 709, 687, 665, 642, 619, 596, 573, 551, 
                                    528,  506, 484, 462, 440, 419, 399, 379, 359, 340,
                                    322,  304, 286, 269, 253, 238, 223, 209, 195, 182,
                                    170,  158, 147, 136, 126, 117, 108, 100,  92,  85,
                                     78,   71,  65,  60,  55,  50,  46,  41,  38,  34,  
                                     31,   28,  25,  23,  21,  19,  17,  15,  13,  12,
                                     11,   10,   8,   8,   7,   6,   5,   5,   4,   4,   
                                      3,    3,   2,   2,   2,   2,   1,   1,   1,   1, 1
                                };

static void app_light_ctrl_breathing(uint32_t  param)
{    
    //Gaussian distribution  
    static uint16_t usMin = 0;	
	  static uint16_t usStandIndex = 0;
	  static uint16_t usStandIndex2 = 0;
	  static uint8_t ucStandAgainCnt = 1;
	  static uint8_t ucStandAgainCnt2 = 0;
	  static uint16_t usStandCnt = 0;
	  uint16_t usTempIndex = 0;
	  uint16_t usTempIndex2 = 0;
	  uint16_t usCur;
	  uint16_t usCur2;
    

		switch(p_gens_color_breath_env.state)
		{
        case(BREATH_INIT):
		    {
				    usMin = 0;
			      memset((unsigned char *)&p_gens_color_breath_env.rgb, 0, sizeof(COLOR_RGB));
				
            rdw_light_gradual_ctrl_start(); 
					
				    usStandIndex =0;
				    usStandIndex2=0;
        
           p_gens_color_breath_env.state = BREATH_CHANGE;								
		   }			 
       break;

		   case	(BREATH_CHANGE):
		   {
           memset(&p_gens_color_breath_env.rgb, 0, sizeof(COLOR_RGB));

           if(ucStandAgainCnt < 1) 
					 {
              usTempIndex = ((sizeof(sBreathParm)/sizeof(uint16_t)) - usStandIndex - 1);  /*  */
           } 
					 else 
					 {
              usTempIndex = usStandIndex;
           }
				
				   if(ucStandAgainCnt2 < 1) 
					 {
              usTempIndex2 = ((sizeof(sBreathParm)/sizeof(uint16_t)) - usStandIndex2 - 1);  /*  */
           } 
					 else 
					 {
               usTempIndex2 = usStandIndex2;
           }
					 

					 if(p_gens_color_breath_env.coolorcolor==0)
					 {
               usCur  = (uint16_t) (sBreathParm[usTempIndex] /1000.0 * ((float)(p_gens_color_breath_env.hsl.actual_lightness/1.0f) - usMin)) + usMin ;
				   }
					 else
					 {
				       usCur  = (uint16_t) (sBreathParm[usTempIndex] /1000.0 * ((float)(p_gens_color_breath_env.ctl.actual_lightness/1.0f) - usMin)) + usMin ;
           }
					 
				   if(p_gens_color_breath_env2.coolorcolor==0)
					 {
				      usCur2 = (uint16_t) (sBreathParm[usTempIndex2] /1000.0 * ((float)(p_gens_color_breath_env2.hsl.actual_lightness/1.0f) - usMin)) + usMin ;
           }
					 else
					 {
				       usCur2 = (uint16_t) (sBreathParm[usTempIndex2] /1000.0 * ((float)(p_gens_color_breath_env2.ctl.actual_lightness/1.0f) - usMin)) + usMin ;
				   }

					 rdw_light_gradual_ctrl_targetset(usCur,usCur2);

           usStandIndex++;
				   usStandIndex2++;
					 
           if(usStandIndex >= (sizeof(sBreathParm)/sizeof(uint16_t))) 
					 {
               usStandIndex = 0;
               ucStandAgainCnt++;
               if(ucStandAgainCnt >= 2) 
							 {
                  p_gens_color_breath_env.state = BREATH_OK;
                  ucStandAgainCnt = 0;
               } 
							 else 
							 {
                   p_gens_color_breath_env.state = BREATH_CHANGE;
               }
           } 
					 
           rdw_light_gradual_ctrl_start(); 
		 }			 
     break;
		 
		 default:
			 break;
	  }		 
		
      if(p_gens_color_breath_env.state == BREATH_OK) 
      {
          usStandIndex ++;

          if(usStandIndex < 2) 
				  {  /* wait 2*BREATH_TIMER_CYCLE_MS (ms) */
            ;
          } 
				  else 
				  {
              usStandIndex = 1;
              usStandCnt++;
              if(usStandCnt >= p_gens_color_breath_env.uiBreathCnt) 
							{ 
								// breath unit cnt ok 
                //@attention: if set alway breath, the uiBreathCnt will set 0xFFFFFFFF or (0xFFFFFFFF / 3) (bigger than 65535)
                p_gens_color_breath_env.state = BREATH_INIT;
                usStandCnt = 0;
                usStandIndex = 0;

                memset(&p_gens_color_breath_env.rgb, 0, sizeof(COLOR_RGB));
							
                rdw_light_gradual_ctrl_start(); 

                printf("Breath ok!!! stop!!!!!");
							
                cobra_soft_timer_stop(p_gens_color_breath_env.timer_id);
							
              } 
						  else 
							{
                  p_gens_color_breath_env.state = BREATH_CHANGE;
              }   
        }
     }//if(p_gens_color_breath_env.state == BREATH_OK) 
		 else
		 {
            cobra_soft_timer_start(p_gens_color_breath_env.timer_id, p_gens_color_breath_env.transtime, 1);
     }
		 
}

/*** 得到呼吸灯的类型***/
CTRL_DEF_COLOR_E rdw_light_cfg_init_get_netcolor(void)
{
   return light_breath_type; 
}

void rdw_light_cfg_init_set_netcolor(CTRL_DEF_COLOR_E type)
{
   light_breath_type = type; 
}

/**
 * @brief: get color min limit
 * @param {none}
 * @retval: uint8_t   0~100
 */
uint8_t rdw_light_cfg_init_get_colormin(void)
{
	return 0;
}

/**
 * @brief: get cold and warm light min value
 * @param {none}
 * @retval: min light value  0~100
 */
uint8_t rdw_light_cfg_init_get_cwmin(void)
{
	return 0;
}

void rdw_light_gradual_ctrl_targetset(uint16_t usBright,uint16_t usBright2)
{
	  COLOR_HSL HSL;
    COLOR_RGB RGB;
//	  printf("h=%x usBright=%x s=%x",p_gens_color_breath_env.hsl.actual_hue,usBright,p_gens_color_breath_env.hsl.actual_saturation);
	  #if APP_FLASH_LIGHT_NUMBERS
    if(flash_light_enable == true)
			return;
    #endif
	  if(p_gens_color_breath_env.coolorcolor == 0) //hsl
		{
	      HSL.h = ((float)(p_gens_color_breath_env.hsl.actual_hue)/255.0f);
		    HSL.s = ((float)(p_gens_color_breath_env.hsl.actual_saturation)/255.0f);
		    HSL.l = ((float)(usBright)/255.0f);
	
	      app_color_hsl_to_rgb(&HSL,&RGB);
	
	      p_gens_color_breath_env.rgb = RGB;
		}
		else
		{
//			printf("usBright=%x %x\r\n",usBright,p_gens_color_breath_env.ctl.actual_temparture);
			 app_cobra_light_set_lum(usBright);
			 if(p_gens_color_breath_env2.coolorcolor == 0)
			 app_ctl_set_warm_duty(usBright*p_gens_color_breath_env.ctl.actual_temparture/100);
		}
		
	
	  if(p_gens_color_breath_env2.coolorcolor == 0) //hsl
		{		
		    HSL.h = ((float)(p_gens_color_breath_env2.hsl.actual_hue)/255.0f);
		    HSL.s = ((float)(p_gens_color_breath_env2.hsl.actual_saturation)/255.0f);
		    HSL.l = ((float)(usBright2)/255.0f);
	
	      app_color_hsl_to_rgb(&HSL,&RGB);
		    p_gens_color_breath_env2.rgb = RGB;
		}
		else
		{
			 app_cobra_light_set_lum(usBright2);
			 if(p_gens_color_breath_env.coolorcolor == 0)
			 app_ctl_set_warm_duty(usBright2*p_gens_color_breath_env2.ctl.actual_temparture/100);			
		}		

    if(p_gens_color_breath_env.coolorcolor == 0 && p_gens_color_breath_env2.coolorcolor == 0)	
		{			
	     RGB.r = p_gens_color_breath_env.rgb.r + p_gens_color_breath_env2.rgb.r;
	     RGB.g = p_gens_color_breath_env.rgb.g + p_gens_color_breath_env2.rgb.g;
	     RGB.b = p_gens_color_breath_env.rgb.b + p_gens_color_breath_env2.rgb.b;
		}
	
    if(p_gens_color_breath_env.coolorcolor == 0 || p_gens_color_breath_env2.coolorcolor == 0)	
		{		
	      app_cobra_light_set_blue_duty(RGB.b);
		    app_cobra_light_set_green_duty(RGB.g);
		    app_cobra_light_set_red_duty(RGB.r);
		}
		
		if(p_gens_color_breath_env.coolorcolor == 1 && p_gens_color_breath_env2.coolorcolor == 1)	
		{
			app_cobra_light_set_lum(usBright + usBright2);
			app_ctl_set_warm_duty(usBright*p_gens_color_breath_env.ctl.actual_temparture/100 + usBright2*p_gens_color_breath_env2.ctl.actual_temparture/100);
		}
	
}

void rdw_light_gradual_ctrl_init(const COLOR_RGB * rgb_data1,const COLOR_RGB * rgb_data2)
{
//	printf("rdw_light_gradual_ctrl_init %x %x %x\r\n",hsl1.actual_hue,hsl1.actual_lightness,hsl1.actual_saturation);
	
	if(p_gens_color_breath_env.coolorcolor==0)
	{
	p_gens_color_breath_env.hsl.actual_hue = rgb_data1->r;//hsl.actual_hue;
	p_gens_color_breath_env.hsl.actual_lightness = rgb_data1->b;//;
	p_gens_color_breath_env.hsl.actual_saturation = rgb_data1->g;//;
	}
	else{
	p_gens_color_breath_env.ctl.actual_lightness  = rgb_data1->b;
	p_gens_color_breath_env.ctl.actual_temparture = rgb_data1->g;
	}
	
	if(p_gens_color_breath_env2.coolorcolor==0)
	{	
	p_gens_color_breath_env2.hsl.actual_hue = rgb_data2->r;//hsl.actual_hue;
	p_gens_color_breath_env2.hsl.actual_lightness = rgb_data2->b;//;
	p_gens_color_breath_env2.hsl.actual_saturation = rgb_data2->g;//;	
	}
	else{
	p_gens_color_breath_env2.ctl.actual_lightness  = rgb_data2->b;
	p_gens_color_breath_env2.ctl.actual_temparture = rgb_data2->g;
	}	
	
	p_gens_color_breath_env.uiBreathCnt = 1;//0xffffff;
	
	rdw_light_cfg_init_set_netcolor(DEF_COLOR_RGB);
	
	app_light_ctrl_breathing_start();
}

void app_light_shade_ctrl_disable(void)
{
	cobra_soft_timer_stop(p_gens_color_breath_env.timer_id);
}


void rdw_light_gradual_ctrl_start(void)
{

}
void app_light_ctrl_breathing_timer_init(void)
{
//	if(p_gens_color_breath_env.timer_id == 0)
	  p_gens_color_breath_env.timer_id = cobra_soft_timer_create(app_light_ctrl_breathing);	
}

void app_light_ctrl_breathing_start(void)
{
	
	if(p_gens_color_breath_env.timer_id < 0)
	{
		  printf("breath timer stop error!");	
		  return;
	}
	cobra_soft_timer_stop(p_gens_color_breath_env.timer_id);
	cobra_soft_timer_start(p_gens_color_breath_env.timer_id, p_gens_color_breath_env.transtime, 0);
  
}

#endif 

#endif