/******************************************************************************
	File Name	:	wm8976_drv.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/07

	Description :   

	Changed Log	:
		Liujian  2020/03/07			- Creation

*****************************************************************************/

#ifndef __WM8976_DRV_H__
#define __WM8976_DRV_H__
#include "rdw_mcu.h"
#include "cobra_i2c.h"

unsigned int wm8978_init(void);
void wm8978_adda_cfg(uint8_t dacen,uint8_t adcen);
void wm8978_input_cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen);
void wm8978_i2s_cfg(uint8_t fmt,uint8_t len, uint8_t stero);
#endif
