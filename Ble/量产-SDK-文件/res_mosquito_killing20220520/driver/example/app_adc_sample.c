/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_adc_sample.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/9

	Description :   
	  		

	Changed Log	:

		Liujian  2021/6/9		- Creation


*****************************************************************************/

#include "app_adc_sample.h"

#if (APP_ADC_SAMPLE_COMMON_ENABLE == 1)

#include "rdw_adc.h"
#include "cobra4C_eflash_ctrl.h"
#include "cobra_timer.h"
#include "app_bk_task.h"
#include "cobra_pinmux.h"
#include "cobra4_gpadc.h"
#include "cobra4_aon_top.h"
#include "cobra_m4_gpio.h"
#include "cobra_soft_timer.h"

#define COBRA_ADC_MODE_SETTING_SINGLE_END     0
static void app_adc_sample_callback(void);
void adc_calibration(int32_t *p_offset);
static uint32_t cnt = 0;
//static uint32_t sum_data[6] = {0,0,0,0,0,0};
volatile unsigned short adc_sampe_data[8];
static float total = 0;
static unsigned int number = 0;
static unsigned short adcvalue = 0;
static unsigned short max_data = 0;
static unsigned short min_data = 0;
static float max_val = 0.0;
static float min_val = 0.0;
static unsigned short adc_ret = 0;
void app_adc_sample_init(void)
{
	   cobra4_aon_pd_top_protection_en(1);
	   cobra4_aon_pd_top_d2a_anatmux_en(1);
	   cobra4_aon_pd_top_d2a_anatmux_sel(10);
	   cobra4_aon_pd_top_protection_en(0);

			
	
	   rdw_adc_set_adc_power(PD0_SEL_BGP_LN, REF_SEL_BLP_LN);
	   rdw_adc_set_adc_clk(ADC_RATE_8M, CLK_SRC_XO);
		 //adc_pinmux_uint();
	#if (COBRA_ADC_MODE_SETTING_SINGLE_END == 1)
	   rdw_adc_get_adc_data_init(ADC_MODE_NORMAL, ADC_SINGLE_END_MODE, ADC_GAIN_1X, DIFF_POS_DPIN29, DIFF_NEG_DPIN_MAX);
	#else
	   rdw_adc_get_adc_data_init(ADC_MODE_NORMAL, ADC_SINGLE_END_MODE, ADC_GAIN_1X, DIFF_POS_DPIN26, DIFF_NEG_DPIN_MAX);	
	   rdw_adc_get_adc_data_init(ADC_MODE_NORMAL,ADC_DIFFERENTIAL_MODE,ADC_GAIN_1X,DIFF_POS_DPIN28,DIFF_NEG_DPIN27);
		 rdw_adc_get_adc_data_init(ADC_MODE_NORMAL,ADC_DIFFERENTIAL_MODE,ADC_GAIN_1X,DIFF_POS_DPIN29,DIFF_NEG_DPIN27);
		 rdw_adc_get_adc_data_init(ADC_MODE_NORMAL,ADC_DIFFERENTIAL_MODE,ADC_GAIN_1X,DIFF_POS_DPIN30,DIFF_NEG_DPIN27);
		 rdw_adc_get_adc_data_init(ADC_MODE_NORMAL,ADC_DIFFERENTIAL_MODE,ADC_GAIN_1X,DIFF_POS_DPIN31,DIFF_NEG_DPIN27);
	#endif
	
	   rdw_adc_fifo_init();
	#if (COBRA_ADC_MODE_SETTING_SINGLE_END == 1)
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN24);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN25);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN26);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN27);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN28);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN29);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN30);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN31);
  #else
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN24);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN25);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN26);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN27);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN28);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN29);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN30);	
	   cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN31);	
  #endif
	
//	   app_cobra_timer1_set_callback(app_adc_sample_callback);
//	
//	   app_cobra_init_timer1(1);//1Hz
//	
//	   app_cobra_timer1_enable(1);
	
	   app_m3_gpio_set_direction(GPIO_PIN_13, GPIO_OUTPUT);
	
	
}


static void app_adc_sample_callback(void)
{
	   cobra_app_set_sample_adc_flag_task();
}

static uint32_t adc_sample_timeout;
void start_adc_sample(void)
{
	adc_sample_timeout = cobra_soft_timer_create(app_adc_sample_callback);

	 cobra_soft_timer_start(adc_sample_timeout, COBRA_SOFT_TIMER_TICK, 1);//COBRA_SOFT_TIMER_TICK
}
void app_adc_sample_task(void)
{
	   volatile unsigned short temp;
		 app_m3_gpio_write_pin(GPIO_PIN_13, GPIO_LOW);	
	

//	   cnt++;
	#if (COBRA_ADC_MODE_SETTING_SINGLE_END == 1)
	   rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN24);	
	   cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[0] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[0] = cobra4_gpadc_read_adc_data();
      while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[0] = cobra4_gpadc_read_adc_data();
//	   while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[0] = cobra4_gpadc_read_adc_data();

	   rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN25);	
	   cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[1] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[1] = cobra4_gpadc_read_adc_data();
     while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[1] = cobra4_gpadc_read_adc_data();
//     while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[1] = cobra4_gpadc_read_adc_data();

		 
	
	   rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN26);	
	   cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[2] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[2] = cobra4_gpadc_read_adc_data();
     while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[2] = cobra4_gpadc_read_adc_data();
//     while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[2] = cobra4_gpadc_read_adc_data();

		 
		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN27);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[3] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[3] = cobra4_gpadc_read_adc_data();
     while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[3] = cobra4_gpadc_read_adc_data();
//     while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[3] = cobra4_gpadc_read_adc_data();

		 
		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN28);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[4] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[4] = cobra4_gpadc_read_adc_data();
//     while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[4] = cobra4_gpadc_read_adc_data();
		 
		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN29);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[5] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[5] = cobra4_gpadc_read_adc_data();
//     while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[5] = cobra4_gpadc_read_adc_data();

		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN30);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[6] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[6] = cobra4_gpadc_read_adc_data();
//     while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[6] = cobra4_gpadc_read_adc_data();
		 
		 
		 rdw_adc_single_adc_mode_select_channel(DIFF_POS_DPIN31);	
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[7] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[7] = cobra4_gpadc_read_adc_data();
//		 while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[7] = cobra4_gpadc_read_adc_data();
		 
		#else

	   rdw_adc_diff_adc_mode_select_channel(DIFF_POS_DPIN28, DIFF_NEG_DPIN27);
	   cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[0] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[0] = cobra4_gpadc_read_adc_data();
//		 while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[0] = cobra4_gpadc_read_adc_data();


	   rdw_adc_diff_adc_mode_select_channel(DIFF_POS_DPIN29, DIFF_NEG_DPIN27);
	   cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[1] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[1] = cobra4_gpadc_read_adc_data();
//		 while(cobra4_gpadc_fifo_is_empty());
//	   adc_sampe_data[1] = cobra4_gpadc_read_adc_data();

		 
	
	   rdw_adc_diff_adc_mode_select_channel(DIFF_POS_DPIN30, DIFF_NEG_DPIN27);
	   cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[2] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[2] = cobra4_gpadc_read_adc_data();
     while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[2] = cobra4_gpadc_read_adc_data();


		 
		 rdw_adc_diff_adc_mode_select_channel(DIFF_POS_DPIN31, DIFF_NEG_DPIN27);
		 cobra4_gpadc_fifo_enable(0);
	   cobra4_gpadc_fifo_enable(1);
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[3] = cobra4_gpadc_read_adc_data();
	   while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[3] = cobra4_gpadc_read_adc_data();
     while(cobra4_gpadc_fifo_is_empty());
	   adc_sampe_data[3] = cobra4_gpadc_read_adc_data();

		#endif
      
		 
	   app_m3_gpio_write_pin(GPIO_PIN_13, GPIO_HIGH);
		 
//		 for(int i =0;i<6;i++)
//		 sum_data[i]+= adc_sampe_data[i];
//		 if(cnt ==1024)
//		 {
//			 cnt = 0;
//			 for(int i =0;i<6;i++)
//			 {
//		   adc_sampe_data[i] = sum_data[i]/1024;
//			 sum_data[i] = 0;
//			 }
		#if (COBRA_ADC_MODE_SETTING_SINGLE_END == 1)

			  float tttt;
			  tttt = adc_sampe_data[4];
			  tttt /= 4096.0;
			  tttt *= 3.4;
				tttt+= 0.15;
				cnt++;
			if(cnt == 1)
				{
				max_data = adc_sampe_data[4];
				min_data = adc_sampe_data[4];
				max_val =	 tttt;
				min_val = tttt;
				}
				else
				{
					if(adc_sampe_data[4] >  max_data)
						 max_data = adc_sampe_data[4];
					if(adc_sampe_data[4] < min_data)
						min_data = adc_sampe_data[4];
					
					if(tttt >  max_val)
						 max_val =	 tttt;;
					if(tttt < min_val)
						min_val = tttt;
					
				}
				
				total += tttt;
				number++;
				adcvalue += adc_sampe_data[4];

				if(number >= 8)
				{
					printf(" **********\n");
						printf(" %d\n", min_data);
						printf(" %f\n", min_val);
						printf(" %d\n", max_data);
						printf(" %f\n", max_val);
						printf(" %d\n",  adcvalue/8);
						printf(" %f\n", total/8);
					printf(" xxxxxxxxxx\n");
					number = 0;
					total = 0;
					adcvalue = 0;
					cnt  = 0;
					max_val = 0.0;
					min_val =0.0;
					max_data =0;
					min_data =0;
				}	
		 #else
			  float tttt;				
				cnt++;
			  tttt = adc_sampe_data[0];
			  tttt = tttt - 2048;
			  tttt /= 4096.0;
			  tttt *= 6.1;
				
				if(cnt == 1)
				{
				max_data = adc_sampe_data[0];
				min_data = adc_sampe_data[0];
				max_val =	 tttt;
				min_val = tttt;
				}
				else
				{
					if(adc_sampe_data[0] >  max_data)
						 max_data = adc_sampe_data[0];
					if(adc_sampe_data[0] < min_data)
						min_data = adc_sampe_data[0];
					
					if(tttt >  max_val)
						 max_val =	 tttt;;
					if(tttt < min_val)
						min_val = tttt;
					
				}
				
				total += tttt;
				number++;
				adcvalue += adc_sampe_data[0];

				if(number >= 8)
				{
					printf(" **********\n");
						printf(" %d\n", min_data);
						printf(" %f\n", min_val);
						printf(" %d\n", max_data);
						printf(" %f\n", max_val);
						printf(" %d\n",  adcvalue/8);
						printf(" %f\n", total/8);
					printf(" xxxxxxxxxx\n");
					adc_ret = adcvalue/8;
//       	printf("***v[0] = %f ,data[0] = %d***\n", total/8, adcvalue/8);
					 number = 0;
					 total = 0;
					adcvalue = 0;
					cnt  = 0;
					max_val = 0.0;
					min_val =0.0;
					max_data =0;
					min_data =0;
				}
//				cobra_soft_delay_ms(1000);
//			  printf("v[0] = %f ,data[0] = %d\n", tttt, adc_sampe_data[0]);
			  
	
		 #endif
//		 #if (COBRA_ADC_MODE_SETTING_SINGLE_END == 1)
////     printf("%04x %04x %04x %04x %04x %04x %04x %04x \n", adc_sampe_data[0], 
////		                                                      adc_sampe_data[1], 
////																													adc_sampe_data[2], 
////																													adc_sampe_data[3], 
////																													adc_sampe_data[4], 
////																													adc_sampe_data[5], 
////																													adc_sampe_data[6], 
////																													adc_sampe_data[7]);
//																												
//		 #else
//		 printf("%04x %04x %04x %04x  \n",  adc_sampe_data[0], 
//																				adc_sampe_data[1], 
//																				adc_sampe_data[2], 
//																				adc_sampe_data[3]); 
//		 #endif
//		 }
																													
																					
}
unsigned short get_adc_value(void)
{
	return 	adc_ret;
}

#endif

