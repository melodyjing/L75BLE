/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	:	  app_uart_module.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/29
	Description :   
	 		  为做的模组
	Changed Log	:

		Liujian  2021/6/29		- Creation


*****************************************************************************/

#ifndef __APP_UART_MODULE_H__
#define __APP_UART_MODULE_H__
#include <stdint.h>

#if (UART_MODEL_TIMEOUT_ENABLE == 1)

void app_uart_module_process_uart_rx_init(void);

void app_uart_module_process_uart_rx_data(uint8_t rx_ch);

void app_uart_module_process_uart_rx_enable(uint8_t en);

void  app_uart_module_timeout_init(void);

void app_uart_mtu_ready(void);

void app_uart_mtu_timer_init(void);

#endif

#endif
