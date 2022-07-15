/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	Author:Liujian
	cobra Drivers
	File Name	:	rdw_uart_tx.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/7/3
	Description :   
           used to uart tx data, using task 
	Changed Log	:
		Liujian  2020/7/3			- Creation

*****************************************************************************/
#include "rdw_uart_tx.h"


#if (APP_UART_TASK_ENABLE)

static 	uint8_t uart_tx_init_flag = 0;

UART_QUEUE_TYPE   rdw_uart_tx_buffer;

void rdw_uart_tx_init(void)
{
		memset((uint8_t *)&rdw_uart_tx_buffer, 0, sizeof(rdw_uart_tx_buffer));
		QueueInit(rdw_uart_tx_buffer);
		uart_tx_init_flag =  1;
}


void rdw_uart_tx_output_ch(uint8_t ch)
{
		if(uart_tx_init_flag)
		{
			  QueueIn(rdw_uart_tx_buffer, ch);
			  ke_event_set(KE_EVENT_UART_TX);
		}
}

void rdw_uart_tx_output_task(void)
{
	  uint8_t cChar;		
	
		while(1)
		{
			
			if(cobra_uart_tx_in_progress(M3_UART0))
				continue;
			
			if(true == QueueOut(rdw_uart_tx_buffer, cChar)) 
			{
				 cobra_uart_send_tx_data(M3_UART0, cChar);
			}
			else
			{
					break;
			}
			
		}
		ke_event_clear(KE_EVENT_UART_TX);
			
}



void rdw_uart_tx_task_init(void)
{
    ke_event_callback_set(KE_EVENT_UART_TX, &rdw_uart_tx_output_task);
}








#endif
