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

#if(BLE_DIANBIAO_CALCUALTE_ENABLE  && DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE)
#include "app_iris_calculate.h"

void app_iris_calculate_box_task(APP_CAL_FREQUENCY *p_cal_param, unsigned int address, unsigned char pluse_type);

#endif

#endif

