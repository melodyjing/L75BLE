/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_calculate_box.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/11/9

	Description :   
	    台体端，用于接收脉冲信号，恢复信号

	Changed Log	:

		Liujian  2020/11/9			- Creation


*****************************************************************************/
#ifndef __APP_CALCULATE_BOX_H__
#define __APP_CALCULATE_BOX_H__

#if (DIANBIAO_PROTOCOL_REALTEK_ENABLE)

#include "def_self.h"
#include "app_realtek_calculate.h"

void app_iris_calculate_box_task(APP_CAL_FREQUENCY *p_cal_param, UINT address, UCHAR pluse_type);

#endif

#endif

