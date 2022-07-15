/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.	
	Author:Liujian
	cobra Drivers
	File Name	:	cobra4_gpadc.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/19
	Description :   
	Changed Log	:
		Liujian  2019/8/19			- Creation
*****************************************************************************/
#include "cobra4_gpadc.h"
#include "cobra_soft_timer.h"
#include "cobra4_aon_top.h"
#include "cobra_umcu.h"

#if COBRA4_GPADC_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile unsigned int * p_cobra4_gpadc_fifo		__attribute__((section("retention_area"),zero_init)); 
static volatile unsigned int * p_cobra4_gpadc_ctrol		__attribute__((section("retention_area"),zero_init)); 
static volatile unsigned int * p_cobra4_gpadc_fifo_state		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile unsigned int * p_cobra4_gpadc_fifo;
static volatile unsigned int * p_cobra4_gpadc_ctrol;
static volatile unsigned int * p_cobra4_gpadc_fifo_state;
#endif

void cobra4_gpadc_init(void)
{
	p_cobra4_gpadc_fifo = (volatile unsigned int *)COBRA4_GPADC_FIFO_BASE;
	p_cobra4_gpadc_ctrol = (volatile unsigned int *)COBRA4_GPADC_FIFO_CTROL;
	p_cobra4_gpadc_fifo_state =  (volatile unsigned int *)COBRA4_GPADC_FIFO_STATE;
}

void cobra4_gpadc_fifo_enable(unsigned en)
{
	if(en)
	{
		*p_cobra4_gpadc_ctrol |= 0x01;
	}
	else
	{
		*p_cobra4_gpadc_ctrol &= ~0x01;
	}	
}

void cobra4_gpadc_fifo_wen_enable(unsigned en)
{
	if(en)
	{
		*p_cobra4_gpadc_ctrol |= 0x02;
	}
	else
	{
		*p_cobra4_gpadc_ctrol &= ~0x02;
	}	
}

unsigned int cobra4_gpadc_fifo_is_empty(void)
{
	  return ((*p_cobra4_gpadc_fifo_state)  & (1 << 2));
}

void cobra4_gpadc_fifo_overflow_clr(unsigned int en)
{
	if(en)
	{
			*p_cobra4_gpadc_ctrol |= 0x04;
	}
	else
	{
			*p_cobra4_gpadc_ctrol &= ~0x04;
	}
}

void cobra4_gpadc_fifo_underflow_clr(unsigned int en)
{
	if(en)
	{
			*p_cobra4_gpadc_ctrol |= 0x08;
	}
	else
	{
			*p_cobra4_gpadc_ctrol &= ~0x08;
	}
}

void cobra4_gpadc_cfg_debounce_sample_points(unsigned int debounce)
{
	unsigned int temp;
	temp = *p_cobra4_gpadc_ctrol;
	temp &= ~(0xff << 4);
	temp |= ((debounce&0xff) << 4);
	*p_cobra4_gpadc_ctrol = temp;
}

void cobra4_gpadc_cfg_debounce_filter_en(unsigned en)
{
	if(en)
	{
		*p_cobra4_gpadc_ctrol |= (1 << 12);
	}
	else
	{
		*p_cobra4_gpadc_ctrol &= ~(1 << 12);
	}	
}

void cobra4_gpadc_cfg_debounce_filter_clk_divide_vld(unsigned en)
{
	if(en)
	{
		*p_cobra4_gpadc_ctrol |= (1 << 13);
	}
	else
	{
		*p_cobra4_gpadc_ctrol &= ~(1 << 13);
	}	
}

void cobra4_gpadc_cfg_debounce_filter_clock_count_max(unsigned int max_count)
{
	unsigned int temp;
	temp = *p_cobra4_gpadc_ctrol;
	temp &= ~(0xff << 14);
	temp |= ((max_count&0xff) << 14);
	*p_cobra4_gpadc_ctrol = temp;
}

void cobra4_gpadc_cfg_comparator_b_in_mode(unsigned int mode)
{
	unsigned int temp;
	temp = *p_cobra4_gpadc_ctrol;
	temp &= ~(0x3 << 22);
	temp |= ((mode&0x03) << 22);
	*p_cobra4_gpadc_ctrol = temp;
}

unsigned int cobra4_gpadc_read_adc_data(void)
{
	return *p_cobra4_gpadc_fifo;	
}





#if APP_COBRA4_GPADC_EN
static unsigned int app_test_gpadc_time_index;

void app_test_gpadc_callback(unsigned int param)
{
		unsigned int temp;
	  temp = cobra4_gpadc_read_adc_data();
	  printf("adc = 0x%04x \n", temp);
	
}



void app_cobra4_gpadc_init(void)
{
		unsigned int temp;
	
	  printf("GPADC Test \n");
	
		cobra4_aon_pd_top_protection_en(1);
		cobra4_aon_pd_top_an_clk_gpadc_32k_gate_en(1);
	  cobra4_aon_pd_top_protection_en(0);
	
		app_cobra_umcu_clk_enable(1);
		app_cobra4_umcu_enable_sram();
	
		cobra4_aon_pd_top_protection_en(1);
		cobra4_aon_pd_top_gpadc_clock_selection(GPADC_XTAL48M);	
		cobra4_aon_pd_top_protection_en(0);
	
		cobra4_gpadc_fifo_overflow_clr( 1);
	  cobra4_gpadc_fifo_underflow_clr( 1);
	
		cobra4_gpadc_fifo_underflow_clr( 0);
		cobra4_gpadc_fifo_overflow_clr( 0);
		cobra4_gpadc_cfg_debounce_sample_points(4);
		cobra4_gpadc_cfg_debounce_filter_en(1);
		cobra4_gpadc_cfg_debounce_filter_clk_divide_vld(1);
		cobra4_gpadc_cfg_debounce_filter_clock_count_max(8);
	  cobra4_gpadc_cfg_comparator_b_in_mode(0x00);
		cobra4_gpadc_fifo_wen_enable(1);
		cobra4_gpadc_fifo_enable(1);
	
		while(1)
		{
			temp = cobra4_gpadc_read_adc_data();
			//printf("adc = 0x%04x \n", temp);			
		}
		
	
//   	app_test_gpadc_time_index = cobra_soft_timer_create(app_test_gpadc_callback);
//	
//		cobra_soft_timer_start(app_test_gpadc_time_index, COBRA_SOFT_TIMER_TICK, 1);
	
}



#endif





#endif