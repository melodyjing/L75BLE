/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	:	  app_uart_module.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/29
	Description :   

	Changed Log	:

		Liujian  2021/6/29		- Creation


*****************************************************************************/

#if (UART_MODEL_TIMEOUT_ENABLE == 1)

#include "app_uart_module.h"
#include "app_new_uart.h"
#include "cobra_soft_timer.h"
#include "gattc.h"
#include "att.h"
#include "co_math.h"
#include "app.h"

#define MAX_UART_TIMEOUT_NUMBER  512

static uint8_t rx_enable;
static uint16_t rx_index;
static uint8_t rx_buffer[MAX_UART_TIMEOUT_NUMBER];

static uint32_t uart_module_timeout;

void app_uart_module_timeout_function(unsigned int temp)
{
	   if(rx_index > 0)
		 {
	      uart_send_to_phone(rx_buffer, rx_index);
		    rx_index = 0;
		 }
}

void  app_uart_module_timeout_init(void)
{
	#if (COBRA_SOFT_TIMER_EN)
	 uart_module_timeout = cobra_soft_timer_create(app_uart_module_timeout_function);
	#endif
}

void app_uart_module_timeout_begin_timeout(void)
{
	#if (COBRA_SOFT_TIMER_EN)
	 cobra_soft_timer_start(uart_module_timeout, COBRA_SOFT_TIMER_TICK/10, 0);
	#endif
}

void app_uart_module_timeout_stop_timeout(void)
{
	#if (COBRA_SOFT_TIMER_EN)
	 cobra_soft_timer_stop(uart_module_timeout);
	#endif
}

void app_uart_module_process_uart_rx_init(void)
{
    	rx_index = 0;
	    rx_enable = 0;
}

void app_uart_module_process_uart_rx_data(uint8_t rx_ch)
{
	  uint16_t mtu_size;
	  uint16_t value_len;
	
	  if(!rx_enable)   //没有使能, 丢掉
		{
			 return;
		}
		
	  app_uart_module_timeout_stop_timeout();
		
	  rx_buffer[rx_index] =  rx_ch;
	  rx_index++;
		
		mtu_size = gattc_get_mtu(app_env.conidx);
		
		value_len = co_min(MAX_UART_TIMEOUT_NUMBER, (mtu_size-(ATT_CODE_LEN + ATT_HANDLE_LEN)));

	  if(rx_index >= value_len)
		{
			 uart_send_to_phone(rx_buffer, rx_index);
			 rx_index = 0;
		}
		
		app_uart_module_timeout_begin_timeout();
		
}


void app_uart_module_process_uart_rx_enable(uint8_t en)
{
	 rx_enable = en;	
}

//////////////////////////////////////
///解决主动发送 MTU 的问题
static uint32_t  mtu_timeout_index;
static uint32_t  mtu_ind_ready;


void app_uart_mtu_timer_callback(uint32_t temp)
{
	  if(!mtu_ind_ready)
		{
			 appm_ble_update_mtu(app_env.conidx);
		}
}

void app_uart_mtu_ready(void)
{	
	  mtu_ind_ready = 1;
}

void app_uart_mtu_timer_init(void)
{
	   mtu_timeout_index = cobra_soft_timer_create(app_uart_mtu_timer_callback);
	   
	   cobra_soft_timer_start(mtu_timeout_index, COBRA_SOFT_TIMER_TICK/5, 0);
	
	   mtu_ind_ready = 0;
	
}

#endif
