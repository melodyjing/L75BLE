/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_button.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/4/29

	Description :   Test Button Driver from github
	Changed Log	:

		Liujian  2020/4/29		- Creation
*****************************************************************************/

#ifndef 	__APP_BUTTON_H__
#define 	__APP_BUTTON_H__

#include "cobra_button.h"

#if ((COBRA_APP_BUTTON_EN == 1) && (COBRA_BUTTON_EN == 1))
void app_button_init(void);

void app_button_start(void);
void app_button_stop(void);

void app_button_sw9_attach_event(PressEvent event, BtnCallback cb);

#endif

#endif
