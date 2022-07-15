#ifndef _COBRA_ADC__H
#define _COBRA_ADC__H
#include "cobra_config.h"
#if (COBRA_ADC_EN)
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "reg_access.h"

void adc_init(void);
void adc_dpin_change(uint8_t adc_no);
void get_adc_data_set(int gpadc_mode,int diff,int gain,int pin1,int pin2);
void get_adc_data(uint32_t *abc);
#endif

#endif

