/******************************************************************************
	File Name	:	app_i2s_record.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/3/14

	Description :   

	Changed Log	:
		Liujian  2020/3/14		- Creation

*****************************************************************************/

#ifndef __APP_I2S_RECORD_H__
#define __APP_I2S_RECORD_H__

#include "app_voice_buffer.h"
#include "cobra_wm8978.h"
#if USE_WM8976_SAMPLE

#define  WM8976_BCLK   64


void app_adpcm_init(void);

void  app_i2s_record_init(void);

#endif

#endif

