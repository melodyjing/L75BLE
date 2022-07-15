/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_easylogger.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/11/2

	Description :  ¼ÇÂ¼
	Changed Log	:

		Liujian  2020/11/2		- Creation
*****************************************************************************/

#ifndef __APP_EASYLOGGER_H__
#define __APP_EASYLOGGER_H__

#if (COBRA_LOGGER_ENABLE == 1)
#include <elog.h>

void app_easylogger_init(void);

#endif

#endif