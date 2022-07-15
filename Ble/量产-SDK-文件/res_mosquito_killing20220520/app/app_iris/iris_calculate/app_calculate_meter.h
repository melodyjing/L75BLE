/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_calculate_meter.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/11/9

	Description :   
	  电表端, 用于校表		

	Changed Log	:

		Liujian  2020/11/9			- Creation


*****************************************************************************/

#ifndef __APP_CALCULATE_METER_H__
#define __APP_CALCULATE_METER_H__

#if((BLE_DIANBIAO_CALCUALTE_ENABLE == 1) && (DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE == 1))

#include "app_iris_calculate.h"
#include "device1_scan.h"


void app_iris_calculate_meter_task(APP_CAL_FREQUENCY *p_cal_param, unsigned int address, unsigned char pluse_type);

void app_meter_param_process(ble_meter_struct * p_meter_param);

void app_meter_param_process_uart_command(ble_meter_struct * p_meter_param);

void app_meter_ack_data_to_taiti(unsigned char * mac_address, unsigned char length, unsigned char *p_buffer);

void process_command_in_24g(ble_meter_struct *p_data);

void app_metetr_process_commucate_data(void);
#endif


#endif

