/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	Author:Liujian
	cobra Drivers
	File Name	:	rdw_uart_tx.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/7/3
	Description :   
           used to uart tx data, using task 
	Changed Log	:
		Liujian  2020/7/3			- Creation

*****************************************************************************/
#ifndef __RDW_UART_TX_H__
#define __RDW_UART_TX_H__
#if (APP_UART_TASK_ENABLE)

#include "cobra_system.h"
#include "rdw_queue.h"	

typedef struct uart_tx_queue {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[2048+1];
}UART_QUEUE_TYPE;


void rdw_uart_tx_output_task(void);

void rdw_uart_tx_init(void);

void rdw_uart_tx_output_ch(uint8_t ch);

void rdw_uart_tx_task_init(void);
#endif

#endif

