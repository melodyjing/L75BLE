/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_adc_sample.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/9

	Description :   
	  		

	Changed Log	:

		Liujian  2021/6/9		- Creation


*****************************************************************************/

#ifndef __APP_ADC_SAMPLE_H__
#define __APP_ADC_SAMPLE_H__
#include <stdint.h>               // standard integer definitions
#include <stdbool.h>              // standard boolean definitions
#if (APP_ADC_SAMPLE_ENABLE == 1)
//enum ADC_INIT_TYPE {
//TYPE_SINGLE_END = 0,
//TYPE_DIFF,

//};
//struct adc_init_state {
//uint8_t adc_type_dpin28;
//uint8_t adc_type_dpin29;
//uint8_t adc_type_dpin30;
//uint8_t adc_type_dpin31;

//} state_adc_init_type;

void app_adc_sample_init(void);



void app_adc_sample_task(void);

#endif

#endif


