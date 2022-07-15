/******************************************************************************
	File Name	:	cobra_soft_uart.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/04/21

	Description :   

	Changed Log	:
		Liujian  2020/04/21			- Creation

*****************************************************************************/

#include "cobra_soft_uart.h"
#include "ll.h"
#include "cobra_timer.h"


#if (COBRA_SOFT_UART_ENABLE == 1)

#define SOFT_UART_PIN_INDEX  PIN27_INDEX
#define SOFT_UART_GPIO       GPIO_PIN_27

#define SOFT_USE_BAUD_RATE_VALUE   660    // 115200Kbps
/*
    初始化 使用那个 IO 口作为 软件串口
*/
void cobra_soft_uart_init(void)
{
		cobra_pin_pullup_enable(SOFT_UART_PIN_INDEX, 1);
	
		app_m3_gpio_set_direction(SOFT_UART_GPIO, GPIO_OUTPUT);
	
		app_m3_gpio_write_pin(SOFT_UART_GPIO, GPIO_HIGH);	
	
}

/*
  软件发送数据
*/
void cobra_soft_send_data(unsigned char data)
{
		gpio_level temp_data[11];
	
		unsigned char i;
	
	  volatile unsigned int j;
	
		unsigned char even = 0;
	
		temp_data[0] = GPIO_LOW;
	
		for(i = 0; i < 8; i++)
		{
				if(data & 0x01)
				{
					temp_data[1+i] = GPIO_HIGH;
					even += 1;
				}
				else
				{
					temp_data[1+i] = GPIO_LOW;
				}
				data >>= 1;
		}
		
		if(even&0x01)
		{
				temp_data[9] = GPIO_HIGH;
		}
		else
		{
				temp_data[9] = GPIO_LOW;
		}
		
		temp_data[10] = GPIO_HIGH;
		
		GLOBAL_INT_DISABLE();	
		for(i = 0; i < 11; i++)
		{
				app_m3_gpio_write_pin(SOFT_UART_GPIO, temp_data[i]);
				for(j = 0; j < SOFT_USE_BAUD_RATE_VALUE; j++);
		}
		GLOBAL_INT_RESTORE();
		
}

/*
  测试使用
*/
void app_test_soft_uart_send(void)
{
		while(1)
		{
				cobra_soft_send_data(0x55);
				cobra_soft_delay_ms(1000);
		}
}

#endif

