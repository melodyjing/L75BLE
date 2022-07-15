/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_tulip_battery.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/9

	Description :   
	  		

	Changed Log	:

		Liujian  2021/6/9		- Creation


*****************************************************************************/

#include "app_tulip_battery.h"
#include "app_bk_task.h"
#include "ll.h"
#include "app.h"
#include "app_tulip_comm.h"
#include "app_mesh_util.h"
#include "nvds.h"
#include "rdw_mcu.h"
#include "cobra_system.h"

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1) && APP_TULIP_BATTER_DETECT_VERSION2)

static void app_tulip_battery_detect_callback(void);

static uint32_t  battery_max_adc_data;

void app_tulip_battery_init(void)
{
	   uint32_t i;
	
	   cobra4_aon_pd_top_protection_en(1);
	   cobra4_aon_pd_top_d2a_anatmux_en(1);
	   cobra4_aon_pd_top_d2a_anatmux_sel(10);
	   cobra4_aon_pd_top_protection_en(0);
	
	   rdw_adc_set_adc_power(PD0_SEL_BGP_LN, REF_SEL_BLP_LN);
	   rdw_adc_set_adc_clk(ADC_RATE_8M, CLK_SRC_XO);
	
	   rdw_adc_get_adc_data_init(ADC_MODE_BM, ADC_SINGLE_END_MODE, ADC_GAIN_1X, DIFF_POS_DPIN_MAX, DIFF_NEG_DPIN_MAX);
	
	   rdw_adc_fifo_init();
	
	   app_cobra_timer1_set_callback(app_tulip_battery_detect_callback);
	
	   for(i = 0; i < 10; i++)
	   {
			   cobra_soft_delay_us(20);
			   cobra4_gpadc_read_adc_data();
		 }
		 
		 battery_max_adc_data = 0;
		 for(i = 0; i < 8; i++)
	   {
			   cobra_soft_delay_us(20);
			   battery_max_adc_data += cobra4_gpadc_read_adc_data();
		 }
		 
		 battery_max_adc_data >>= 3;
		 
	   printf("battery_max_adc_data = %d \n", battery_max_adc_data);
		 
	   app_cobra_init_timer1(500);
	
	   app_cobra_timer1_enable(1);
	
}

static void app_tulip_battery_detect_callback(void)
{
	    unsigned int temp;
	    temp = cobra4_gpadc_read_adc_data();
	    if((temp + 100) < battery_max_adc_data)
			{
        // printf("write \n");
				   cobra_app_set_tulip_power_down_flag_task();
				   app_cobra_timer1_enable(0);
			}				
	
}

/*
  下电保存数据
*/
void app_tulip_power_down_save_task(void)
{
	     uint8_t temp = MESH_APPKEY_OK_LENGTH;
	
	     uint8_t appkey_flag = 0;
	
	     GLOBAL_INT_STOP();
	
			 app_mesh_data_save();
	
	     cobra_system_nvds_save_to_eflash();
	
	     cobra_soft_delay_ms(300);

	     ota_platform_reset(0x00);		
}

/*
上电处理 tulip
*/
void app_tulip_power_up_process(void)
{
//	uint8_t *  p_count = (uint8_t * )M4_EFLASH_COUNT_ADDRESS;
//	uint8_t *  p_flag = (uint8_t * )M4_EFLASH_COUNT_TIMEOUT_ADDRESS;
//	uint8_t count = 0;
//	uint32_t temp = MESH_MAGIC_TAG;

//	count = *p_count;
//	if(count == 0xff || (*p_flag == 0x88))
//	{
//		count = 1;
//	}
//	else
//	{
//		count ++;
//	}
//	
//	//PRINTF_LOG("app_mesh_drop_count count:%d", count);
//    cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, M0_M3_NVDS_SIZE);
//    cobra_flash_write(M4_EFLASH_MESH_ADDRESS, sizeof(temp),  (unsigned char *)&temp);        
//    cobra_flash_write(M4_EFLASH_COUNT_ADDRESS, sizeof(count),  (unsigned char *)&count);  

	
	 cobra_flash_erase(M4_EFLASH_NVDS_ADDRESS, M0_M3_NVDS_SIZE);
	
}

#endif
