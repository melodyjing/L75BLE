/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Application for Tulips

	File Name	:	app_tulip_scene.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2021/4/15
	
*******************************************************************************/


#include "app_tulip_comm.h"
#include "app_tulip_scene.h"
#include <string.h>
#include "rdw_mcu.h"
#include "cobra4C_eflash_ctrl.h"

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1) && (COBRA_SCENE_VERSION2_ENABLE == 1))

#define CUSTOM_SCENSE_MAX		SCENS_ID_MAX
//炫彩 的定义的颜色
static const COLOR_RGB xuan_cai_scene[] = 
{
	 {255,  0,     0},
	 {255,  125,    0},
	 {255,  255,   0},
	 {0,    255,   0},
	 {0,    255,  255},
	 {0,    0,    255},
	 {255,  0,    255},
};

//自定义跳变
static const COLOR_RGB self_define_scene[] = 
{
	 {0,    255,    128},
	 {85,    255,   128},
	 {171,    255,  128},
};

scene_data   global_scene_data[CUSTOM_SCENSE_MAX];

///////////////////////////////////////////////////////////////////////
void app_tulipe_scene_process(scene_edit_cmd *p_cmd)
{
	    uint8_t end_flag;
	    uint8_t scene_index;
			uint8_t i;
			scene_data  temp_scene_data;
			printf("scene_process scene_index:%d, mode_flag:%d, len:%d\n", \
			p_cmd->scene_index,p_cmd->mode_flag,  p_cmd->len);

	    if(p_cmd->len > SCENS_COLOR_MAX*5)
			{
				   return;
			}
			
			if(p_cmd->scene_index > SCENS_ID_MAX)
			{
				  return;
			}
			
			
			memset(&temp_scene_data, 0, sizeof(scene_data));
			temp_scene_data.scene_mode = p_cmd->mode_flag;
			
			for(i = 0; i< p_cmd->len/5 ;i++)
			{
					 temp_scene_data.color_type[i] = p_cmd->data[i].color_type;
					 temp_scene_data.trans_time[i] = p_cmd->data[i].trans_time;
				
				   if(p_cmd->data[i].color_type == 0)
					 {
						  COLOR_HSL HSL;
              COLOR_RGB RGB;
	    		
	            HSL.h = ((float)(p_cmd->data[i].color_2/255.0f));
		          HSL.s = ((float)(p_cmd->data[i].color_3/100.0f));
		          HSL.l = ((float)(p_cmd->data[i].color_1/100.0f));
	
						  #if APP_LIGHT_BREATH_FUNCTION
						  if(temp_scene_data.scene_mode == SCENE_BREATHE)
							{

			        temp_scene_data.array.rgb_array[i].r = (uint8_t)(HSL.h*255);
			        temp_scene_data.array.rgb_array[i].g = (uint8_t)(HSL.s*255);
				      temp_scene_data.array.rgb_array[i].b = (uint8_t)(HSL.l*255);		
							}
							else
							{
		              app_color_hsl_to_rgb(&HSL,&RGB);
						 
			            temp_scene_data.array.rgb_array[i].r = RGB.r;
			            temp_scene_data.array.rgb_array[i].g = RGB.g;
				          temp_scene_data.array.rgb_array[i].b = RGB.b;
				          
								 //printf("SCENE r, g,b:  %d:%d:%d \n", RGB.r,RGB.g,RGB.b);								
							}
							
						  #else
		              app_color_hsl_to_rgb(&HSL,&RGB);
						 
			            temp_scene_data.array.rgb_array[i].r = RGB.r;
			            temp_scene_data.array.rgb_array[i].g = RGB.g;
				          temp_scene_data.array.rgb_array[i].b = RGB.b;
				          //printf("SCENE r, g,b:  %d:%d:%d \n", RGB.r,RGB.g,RGB.b);
						  #endif

					 }
					 else
					 {
						   uint8_t lum_100;
						   uint8_t ct_100;
						   float   temp_cool;
						   float   temp_warm;
						   uint8_t cool_duty = 0;
						   uint8_t warm_duty;

						   lum_100 = (p_cmd->data[i].color_1);

						   ct_100 = 0;

						   if(lum_100 != 0)
						   {
							     ct_100 = (p_cmd->data[i].color_2);
							     if(ct_100>98)
								      ct_100 = 100;
						   }

						   #if (SOFT_PWM == 1)				
						        temp_cool = (100 - ct_100);
						        temp_cool *= lum_100;
						        temp_cool /= 100.0;
						        temp_cool /= 100.0;
						        cool_duty = temp_cool*255;
						   #endif				


						#if (SOFT_PWM == 1)
						  warm_duty = ct_100*lum_100/100;
						  if(warm_duty==0)
						  {
							    if(cool_duty < 5)
								    cool_duty = 5;
						  }
						
						#else				
						   temp_cool = lum_100*ct_100;
						   temp_cool /= 100.0;
						   warm_duty = temp_cool;
						   app_cobra_light_set_lum(lum_100);
						#endif

						temp_scene_data.array.rgb_array[i].r = cool_duty;
						#if APP_LIGHT_BREATH_FUNCTION
						if(temp_scene_data.scene_mode == SCENE_BREATHE)
							{
							temp_scene_data.array.rgb_array[i].g = ct_100;
							}
							else
						#endif
						temp_scene_data.array.rgb_array[i].g = warm_duty;
						#if (SOFT_PWM == 1)
						   temp_scene_data.array.rgb_array[i].b = 0; //reserve
						#else
						   temp_scene_data.array.rgb_array[i].b = lum_100; //reserve
						#endif

            printf("ct_100:  %d; warm_duty: %d; lum_100:%d\n", ct_100, warm_duty,lum_100);

					 }
			}
				
		   temp_scene_data.update_flag = 0x01;
		   temp_scene_data.color_count = i;
						 
		   memcpy(&global_scene_data[p_cmd->scene_index-1], &temp_scene_data, sizeof(scene_data));
						 
		   printf("p_cmd->scene_index = %d\n", p_cmd->scene_index);
	     app_tulip_process_scene_recall(p_cmd->scene_index);
}


void app_tulipe_scene_color_process(unsigned int scene_index)
{
	  uint32_t trans_time;
	  printf("app_tulipe_scene_color_process, scene_index:%d, mode:%d \n", scene_index, global_scene_data[scene_index].scene_mode);
	  
	  if(scene_index > CUSTOM_SCENSE_MAX) return;
	
		scene_index -=1;
	  //if(global_scene_data[scene_index].scene_mode == SCENE_STATIC)
#if APP_LIGHT_BREATH_FUNCTION
    if(global_scene_data[scene_index].scene_mode != SCENE_BREATHE)
#endif	
		{
			   app_scene_set_static_light(global_scene_data[scene_index].array.rgb_array[0].r,  \
			                              global_scene_data[scene_index].array.rgb_array[0].g,  \
			                              global_scene_data[scene_index].array.rgb_array[0].b,  \
			                              global_scene_data[scene_index].color_type[0]);
		}
#if APP_LIGHT_BREATH_FUNCTION
		if(global_scene_data[scene_index].scene_mode == SCENE_CHANGED)
		{
			  trans_time = app_mesh_trans_time_convert(global_scene_data[scene_index].trans_time[0]);
			  trans_time /= 100;
			  printf("app_tulipe_scene_color_process =%x",trans_time);
			  if(trans_time>0)  // 防止手机发送错误时间
			  app_scene_set_change_light(global_scene_data[scene_index].array.rgb_array, global_scene_data[scene_index].color_count, trans_time, global_scene_data[scene_index].color_type[0]);
		}
		else if(global_scene_data[scene_index].scene_mode == SCENE_BREATHE)
		{
			  trans_time = app_mesh_trans_time_convert(global_scene_data[scene_index].trans_time[0]);
			  trans_time /= 100;	
				printf("app_tulipe_scene_color_process =%x\r\n",trans_time);
        if(trans_time>0)  // 防止手机发送错误时间			
			  app_scene_set_breath_light(global_scene_data[scene_index].array.rgb_array,global_scene_data[scene_index].color_count, trans_time, global_scene_data[scene_index].color_type[0]);//g_light_scens[(scene_index-1)*SCENS_COLOR_MAX].scens_hsl			
		}
#else		

		if(global_scene_data[scene_index].scene_mode != SCENE_STATIC)
		{
			  trans_time = app_mesh_trans_time_convert(global_scene_data[scene_index].trans_time[0]);
			  trans_time /= 100;
			  app_scene_set_change_light(global_scene_data[scene_index].array.rgb_array, global_scene_data[scene_index].color_count, trans_time, global_scene_data[scene_index].color_type[0]);
		}
#endif		
	
}

void app_tulipe_scense_save(void)
{
	uint16_t i;

	uint8_t flag = 0;

	for(i=0; i<CUSTOM_SCENSE_MAX; i++)
	{
		  if((global_scene_data[i].update_flag)==0x01)
		  {
			   flag = 1;
			   break;
		 }
	}
	

	if(flag)
	{
		volatile uint32_t *p_buf=(volatile uint32_t*)&global_scene_data[0];

		#if (COBRA_FLASH_POWERON_CRASE == 0)
		cobra_flash_erase(APP_MESH_TULIPX_SCENS_DATA,4096);
		#endif
		cobra_flash_write(APP_MESH_TULIPX_SCENS_DATA,sizeof(global_scene_data),(uint8_t *)p_buf);

		//clear update flag
        for(i=0; i<CUSTOM_SCENSE_MAX; i++)
        {
            if((global_scene_data[i].update_flag)==0x01)
            {
                global_scene_data[i].update_flag = 0;   
            }
        }
	 }
	
}


void app_tulipe_scene_color_init(void)
{
	  uint8_t i;
	
	#if (COBRA_FLASH_POWERON_CRASE == 1)
    
	if(get_mesh_scene_data()==false)
	#else
    volatile uint32_t * p_temp = (volatile uint32_t *)APP_MESH_TULIPX_SCENS_DATA;
    
	if(p_temp[0] != 0xffffffff)
	{
        memcpy(&global_scene_data, p_temp, sizeof(global_scene_data));
	}
	else
	#endif
	{
		global_scene_data[0].scene_mode = SCENE_STATIC;
		memset(global_scene_data[0].color_type, 0, COLOR_NUM_MAX);
		//global_scene_data[1].color_type = 0;
		global_scene_data[0].color_count = 1;
		global_scene_data[0].array.rgb_array[0].r = 61;
		global_scene_data[0].array.rgb_array[0].g = 24;
		global_scene_data[0].array.rgb_array[0].b = 0;

		   global_scene_data[1].scene_mode = SCENE_STATIC;
		   memset(global_scene_data[1].color_type, 0, COLOR_NUM_MAX);
		   //global_scene_data[2].color_type = 0;
		   global_scene_data[1].color_count = 1;
		   global_scene_data[1].array.rgb_array[0].r = 222;
		   global_scene_data[1].array.rgb_array[0].g = 219;
		   global_scene_data[1].array.rgb_array[0].b = 140;

		   global_scene_data[2].scene_mode = SCENE_STATIC;
		   memset(global_scene_data[2].color_type, 0, COLOR_NUM_MAX);
		   global_scene_data[2].color_count = 1;
		   global_scene_data[2].array.rgb_array[0].r = 255;
		   global_scene_data[2].array.rgb_array[0].g = 255;
		   global_scene_data[2].array.rgb_array[0].b = 255;

		   global_scene_data[3].scene_mode = SCENE_STATIC;
		   memset(global_scene_data[3].color_type, 0, COLOR_NUM_MAX);
		   global_scene_data[3].color_count = 1;
		   global_scene_data[3].array.rgb_array[0].r = 150;
		   global_scene_data[3].array.rgb_array[0].g = 98;
		   global_scene_data[3].array.rgb_array[0].b = 10;


		   global_scene_data[4].scene_mode = SCENE_STATIC;
		   memset(global_scene_data[4].color_type, 0, COLOR_NUM_MAX);
		   global_scene_data[4].color_count = 1;
		   global_scene_data[4].array.rgb_array[0].r = 255;
		   global_scene_data[4].array.rgb_array[0].g = 45;
		   global_scene_data[4].array.rgb_array[0].b = 69;


		   global_scene_data[7].scene_mode = SCENE_STATIC;
		   memset(global_scene_data[7].color_type, 0, COLOR_NUM_MAX);
		   global_scene_data[7].color_count = 1;
		   global_scene_data[7].array.rgb_array[0].r = 255;
		   global_scene_data[7].array.rgb_array[0].g = 31;
		   global_scene_data[7].array.rgb_array[0].b = 0;

		   global_scene_data[5].scene_mode = SCENE_CHANGED;
		   memset(global_scene_data[5].color_type, 0, COLOR_NUM_MAX);
		   global_scene_data[5].color_count = sizeof(xuan_cai_scene)/sizeof(COLOR_RGB);

		
		   for(i = 0; i < sizeof(xuan_cai_scene)/sizeof(COLOR_RGB); i++)
		   {
			     global_scene_data[5].color_type[i] = 0;
			     global_scene_data[5].array.rgb_array[i].r = xuan_cai_scene[i].r;
			     global_scene_data[5].array.rgb_array[i].g = xuan_cai_scene[i].g;
			     global_scene_data[5].array.rgb_array[i].b = xuan_cai_scene[i].b;
			     global_scene_data[5].trans_time[i] = 10;
		  }

		  global_scene_data[6].scene_mode = SCENE_BREATHE;
		  memset(global_scene_data[6].color_type, 0, COLOR_NUM_MAX);
		  global_scene_data[6].color_count = sizeof(self_define_scene)/sizeof(COLOR_RGB);

		for(i = 0; i < sizeof(self_define_scene)/sizeof(COLOR_RGB); i++)
		{
			global_scene_data[6].color_type[i] = 0;
			global_scene_data[6].array.rgb_array[i].r = self_define_scene[i].r;
			global_scene_data[6].array.rgb_array[i].g = self_define_scene[i].g;
			global_scene_data[6].array.rgb_array[i].b = self_define_scene[i].b;

			global_scene_data[6].trans_time[i] = 30;
		}
	}
}


#endif

