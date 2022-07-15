/******************************************************************************
	File Name	:	cobra4_new_uart.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/10/25

	Description :   
		cobra4 的 UART FIFO 处理方式
	Changed Log	:
		Liujian  2019/10/25			- Creation

*****************************************************************************/
#ifndef __COBRA4_NEW_UART_H__
#define __COBRA4_NEW_UART_H__

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <cobra_config.h>
#include "app_uart_fifo.h"	
	
#if COBRA4_NEW_UART_DRIVER

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */
#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED

typedef void (*uart_app_callback)(uint8_t ch);

void cobra4_new_uart_init(void);
void cobra4_new_uart_register_callback(uart_app_callback callback);
void cobra4_new_uart_unregister_callback(void);
	
	
	

#endif
	
#ifdef __cplusplus
}
#endif
#endif
