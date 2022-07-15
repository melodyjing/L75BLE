/******************************************************************************
	File Name	:	app_pdm_record.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/3/14

	Description :   

	Changed Log	:
		Liujian  2020/3/14		- Creation

*****************************************************************************/
#ifndef __APP_PDM_RECORD_H__
#define __APP_PDM_RECORD_H__

#include "app_rdw_voice.h"
#if USE_PDM_SAMPLE

void app_pdm_record_init(void);


void app_pdm_record_uninit(void);


void app_pdm_record_init_dma(void);

void app_adpcm_init(void);


#endif

#endif

