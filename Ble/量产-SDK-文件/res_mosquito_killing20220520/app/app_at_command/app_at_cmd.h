/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_at_cmd.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/11/2

	Description :  AT command
	Changed Log	:

		Liujian  2020/11/2		- Creation
*****************************************************************************/

#ifndef __APP_AT_CMD_H__
#define __APP_AT_CMD_H__
#include "cobra_config.h"
#include "cobra_cat.h"

#if(APP_AT_COMMAND_ENABLE == 1)

void app_at_cmd_init(void);

void app_at_cmd_loop(void);

#endif

#endif
