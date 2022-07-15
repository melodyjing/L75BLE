/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.	
	Author:Liujian
	cobra Drivers
	File Name	:	cobra4_gpadc.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/19
	Description :   
	Changed Log	:
		Liujian  2019/8/19			- Creation
*****************************************************************************/
#ifndef __COBRA4_GPADC_H__
#define __COBRA4_GPADC_H__
#include <cobra_config.h>

#if COBRA4_GPADC_EN
#define COBRA4_GPADC_FIFO_BASE		(0x40083080UL)
#define COBRA4_GPADC_FIFO_CTROL		(0x40083010UL)    //UMCU uc_r32_0
#define COBRA4_GPADC_FIFO_STATE   (0x40083048UL)
typedef enum {
	 PD0_SEL_BGP_LP,    //low power 1.0v
	 PD0_SEL_BGP_LN,   //low noise  1.2v
}ADC_PD0_SEL_TYPE;

typedef enum {
	REF_SEL_BGP_LP,
	REF_SEL_BLP_LN,
}ADC_REF_SEL_TYPE;

typedef enum {
	CLK_SRC_RCO,
	CLK_SRC_XO,
}ADC_CLK_SRC_TYPE;

typedef enum {
	 ADC_RATE_8M,
	 ADC_RATE_32K
}ADC_RATE_TYPE;

void cobra4_gpadc_init(void);
void cobra4_gpadc_fifo_enable(unsigned en);
void cobra4_gpadc_fifo_wen_enable(unsigned en);
void cobra4_gpadc_fifo_underflow_clr(unsigned int en);
void cobra4_gpadc_fifo_overflow_clr(unsigned int en);
void cobra4_gpadc_cfg_debounce_sample_points(unsigned int debounce);
void cobra4_gpadc_cfg_debounce_filter_en(unsigned en);
void cobra4_gpadc_cfg_debounce_filter_clk_divide_vld(unsigned en);
void cobra4_gpadc_cfg_debounce_filter_clock_count_max(unsigned int max_count);
void cobra4_gpadc_cfg_comparator_b_in_mode(unsigned int mode);
unsigned int cobra4_gpadc_read_adc_data(void);

unsigned int cobra4_gpadc_fifo_is_empty(void);

#if APP_COBRA4_GPADC_EN
void app_cobra4_gpadc_init(void);
#endif

#endif

#endif
