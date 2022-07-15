/******************************************************************************
	File Name	:	cobra_soft_uart.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/04/21

	Description :   

	Changed Log	:
		Liujian  2020/04/21			- Creation

*****************************************************************************/
#ifndef __COBRA_SOFT_UART_H__
#define __COBRA_SOFT_UART_H__
#include "cobra_pinmux.h"
#include "rdw_gpio.h"

#if (COBRA_SOFT_UART_ENABLE == 1)
void app_soft_uart_init(void);

void app_soft_uart_test(void);

void app_soft_send_data(unsigned char data);

#endif

#endif

