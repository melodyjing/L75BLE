/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_network.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/7/13

	Description :   
	  用于IRIS网路的创建

	Changed Log	:

		Liujian  2019/7/13			- Creation


*****************************************************************************/

#ifndef __APP_NETWORK_H__
#define __APP_NETWORK_H__

typedef enum {
		NRF24L01_WAITING_PARE,
		NRF24L01_PARE_SUCCESS,
		NRF24L01_HAVING_PARE,
}NRF24L01_MODE_STATE;

#define 	CMD_SWITCH_CLOSE						0x01
#define 	CMD_SWITCH_OPEN							0x00
#define 	CMD_SWITCH_STATE						0x10
#define   CMD_SWITCH_NUMBER						0x20
#define   CMD_READ_SOFTWARE_VERSION		0x24
#define   CMD_READ_HARDWARE_VERSION		0x25
void app_state_grid_gateway_init(void);
void app_network_command_callback(unsigned char cmd);
void app_network_command_send_state(NRF24L01_MODE_STATE state);
void app_state_grid_led_controll(unsigned int index, unsigned int light);
#endif
