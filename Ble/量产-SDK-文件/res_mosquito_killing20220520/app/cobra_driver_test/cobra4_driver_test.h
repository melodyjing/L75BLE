/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4_driver_test.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/13

	Description :   
        Test driver
	Changed Log	:

		Liujian  2019/8/13			- Creation



*****************************************************************************/
#ifndef __COBRA4_DRIVER_TEST_H__
#define __COBRA4_DRIVER_TEST_H__

#include <cobra_config.h>

#if COBRA4_DRIVER_TEST_ENABLE

void app_driver_test_gpio(void);

void app_driver_test_umcu(void);


void app_driver_test_jump_to_rom(void);

void app_drive_test_read_gpadc_data(void);

void app_drive_test_ppu_power_up(void);
#endif
#endif
