/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_mouse.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/9

	Description :   
	  		

	Changed Log	:

		Liujian  2021/6/9		- Creation


*****************************************************************************/

#include "app_mouse.h"

#if (APP_MOUSE_ENABLE == 1)

#include "rdw_adc.h"
#include "cobra4C_eflash_ctrl.h"
#include "cobra_timer.h"
#include "app_bk_task.h"


static void app_mouse_adc_sample_callback(void);

void app_mouse_init(void)
{

#if (APP_MOUSE_TEST_EMI_ENABLE == 1)
	   int i;
	   
	   app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_OUTPUT);
	   app_m3_gpio_set_direction(GPIO_PIN_5, GPIO_OUTPUT);
	
	    for(i = 0; i< 1000;i++)
		  {
			     cobra_soft_delay_us(1000);
		  }
				 
     while(1)
		 {
			   
			   app_m3_gpio_write_pin(GPIO_PIN_8, GPIO_LOW);	
			   app_m3_gpio_write_pin(GPIO_PIN_5, GPIO_LOW);	
			   for(i = 0; i< 200;i++)
			   {
			     cobra_soft_delay_us(1000);
				 }
				 
			   app_m3_gpio_write_pin(GPIO_PIN_8, GPIO_HIGH);
         app_m3_gpio_write_pin(GPIO_PIN_5, GPIO_HIGH);			 
			   for(i = 0; i< 200;i++)
				 {
			     cobra_soft_delay_us(1000);
			   }
		 }
#endif
	
	   cobra4_aon_pd_top_protection_en(1);
	   cobra4_aon_pd_top_d2a_anatmux_en(1);
	   cobra4_aon_pd_top_d2a_anatmux_sel(10);
	   cobra4_aon_pd_top_protection_en(0);
	
	   rdw_adc_set_adc_power(PD0_SEL_BGP_LN, REF_SEL_BLP_LN);
	   rdw_adc_set_adc_clk(ADC_RATE_8M, CLK_SRC_XO);
	
	   rdw_adc_get_adc_data_init(ADC_MODE_NORMAL, ADC_SINGLE_END_MODE, ADC_GAIN_1X, DIFF_POS_DPIN29, DIFF_NEG_DPIN_MAX);
	
	   rdw_adc_fifo_init();
	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN24);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN25);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN26);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN27);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN28);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN29);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN30);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN31);	
	
	   app_cobra_timer1_set_callback(app_mouse_adc_sample_callback);
	
	   app_cobra_init_timer1(1000);
	
	   app_cobra_timer1_enable(1);
	
	   app_m3_gpio_set_direction(GPIO_PIN_13, GPIO_OUTPUT);
	
	
}


static void app_mouse_adc_sample_callback(void)
{
	   cobra_app_set_mouse_sample_adc_flag_task();
}

volatile unsigned short adc_sampe_data[8];
void app_mouse_adc_sample_task(void)
{
	   volatile unsigned short temp;
	   app_m3_gpio_write_pin(GPIO_PIN_13, GPIO_LOW);	
	   rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN24);	
	   cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[0] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[0] = cobra4_gpadc_read_adc_data();


	   rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN25);	
	   cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[1] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[1] = cobra4_gpadc_read_adc_data();

		 
	
	   rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN26);	
	   cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[2] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[2] = cobra4_gpadc_read_adc_data();


		 
		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN27);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[3] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[3] = cobra4_gpadc_read_adc_data();


		 
		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN28);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[4] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[4] = cobra4_gpadc_read_adc_data();

		 
		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN29);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[5] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[5] = cobra4_gpadc_read_adc_data();


		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN30);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[6] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[6] = cobra4_gpadc_read_adc_data();

		 
		 
		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN31);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[7] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[7] = cobra4_gpadc_read_adc_data();

		 
	   app_m3_gpio_write_pin(GPIO_PIN_13, GPIO_HIGH);
     printf("%04x %04x %04x %04x %04x %04x %04x %04x \n", adc_sampe_data[0], 
		                                                      adc_sampe_data[1], 
																													adc_sampe_data[2], 
																													adc_sampe_data[3], 
																													adc_sampe_data[4], 
																													adc_sampe_data[5], 
																													adc_sampe_data[6], 
																													adc_sampe_data[7]);
																													
																					
}

#endif

