/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_led_button.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/6/28

	Description :   use Led and button service 
	Changed Log	:

		Liujian  2020/6/28		- Creation
*****************************************************************************/
#ifndef __APP_LED_BUTTON_H__
#define __APP_LED_BUTTON_H__

#include "rwip_config.h"     // SW configuration
#if (BLE_APP_LED_BUTTON)
#include <stdint.h>

struct app_led_button_env_tag
{

//   uint8_t  led_value;
	
	 uint8_t  button_value;
	
};



void app_led_button_init(void);

void app_led_button_add_service(void);


void app_led_button_enable_prf(uint8_t conidx);

void app_led_button_send_button_value(uint8_t button_value);

/// Table of message handlers
extern const struct app_subtask_handlers app_led_button_handlers;







#endif
#endif
