/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_mouse.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/9

	Description :   
	  		

	Changed Log	:

		Liujian  2021/6/9		- Creation


*****************************************************************************/

#ifndef __APP_MOUSE_H__
#define __APP_MOUSE_H__

#if (APP_MOUSE_ENABLE == 1)


void app_mouse_init(void);



void app_mouse_adc_sample_task(void);

#endif

#endif


