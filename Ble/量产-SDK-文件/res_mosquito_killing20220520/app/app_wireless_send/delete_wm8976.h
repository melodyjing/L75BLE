/*
liujian
2018/7/2
version 1.0


*/

#ifndef __WM8976_H__
#define __WM8976_H__

#include "rdw_mcu.h"
#include "cobra_i2c.h"

void WM8978_ADDA_Cfg(uint8_t dacen,uint8_t adcen);
void WM8978_Input_Cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen);
void WM8978_LINEIN_Gain(uint8_t gain);
void WM8978_AUX_Gain(uint8_t gain);
void WM8978_Output_Cfg(uint8_t dacen,uint8_t bpsen);
void WM8978_HPvol_Set(uint8_t voll,uint8_t volr);
void WM8978_SPKvol_Set(uint8_t volx);
void WM8976_init(void);
void WM8978_I2S_Cfg(uint8_t fmt,uint8_t len, uint8_t stero);
void WM8978_ADDA_Cfg(uint8_t dacen,uint8_t adcen);

unsigned int wm8978_init(void);
#endif 
