/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  rdw_adc.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/8

	Description :   


	Changed Log	:

		Liujian  2021/6/8	- Creation

*****************************************************************************/

#ifndef __RDW_ADC_H__
#define __RDW_ADC_H__
#if (COBRA_APP_ADC_DATA_EN)
#include "cobra4_aon_top.h"
#include "cobra4_aon_wk.h"
#include "rdw_gpio.h"
#include "cobra4_gpadc.h"


/*
pd0_sel:
  0: 1.0v by ldo_perm with bgp_lp
  1: 1.2v by ldo_dig0 with bgp_ln
ref_sel:
  0:ldo_adc ref from bgp_lp
  1:ldo_adc ref from bgp_ln
*/
void  rdw_adc_set_adc_power(ADC_PD0_SEL_TYPE pd0_sel, ADC_REF_SEL_TYPE ref_sel);
/*
clk_src:
    0:RCO  
    1:XO

adc_rate:
    0:8M     clk_src 1: XO48M    div=6 to get 8M
             clk_src 0: RCO24M   div=3 to get 8M

    1:32K    clk_src 1: XO32K
             clk_src 0: RCO32K

*/
void  rdw_adc_set_adc_clk(ADC_RATE_TYPE adc_rate, ADC_CLK_SRC_TYPE clk_src);


void rdw_adc_get_adc_data_init( GPADC_MODE gpadc_mode, 
	                                   DIFF_MODE diff, \
                                     ADC_GAIN  gain,
	                                   DIFFERENTIAL_POS_INPUT_DPIN pin1, \
                                     DIFFERENTIAL_NEG_INPUT_DPIN pin2);

void rdw_adc_fifo_init(void);

void cobra4_gpadc_get_adc_data_init_gpio_pos(DIFFERENTIAL_POS_INPUT_DPIN pin);

void rdw_adc_single_adc_mode_select_channel(DIFFERENTIAL_POS_INPUT_DPIN pin);

void rdw_adc_diff_adc_mode_select_channel(DIFFERENTIAL_POS_INPUT_DPIN pin1, 
                                     DIFFERENTIAL_NEG_INPUT_DPIN pin2);

void rdw_adc_test(void);

#endif
#endif
