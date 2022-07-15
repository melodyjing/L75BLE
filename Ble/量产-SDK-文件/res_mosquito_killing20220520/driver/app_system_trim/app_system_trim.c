/*
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
//liujian
//2019/5/14 add for trim the 8M RC clock

*/

#include "rdw_mcu.h"
#include "cobra_config.h"
#include "cobra_cortex_m3_nvic.h"
#include "cobra_cortex_m4_nvic.h"
#include "app_bk_task.h"
#include "cobra_system.h"
#include "app_system_trim.h"
#include "cobra_pcrm.h"
#include "cobra_pinmux.h"
#include "cobra_aon_pmu.h"
#include "rdw_gpio.h"
#include "app_sleep.h"
#include "cobra_uart.h"


#if (COBRA_TRIM_EN)
static unsigned int interrupt_number = 0;
static unsigned int systick_stop_time = 0;

void app_uart_rx_pin_interrupt_function(unsigned char  param)
{
	
		//printf("app_uart_rx_pin_interrupt_function \n");
	
		if(interrupt_number == 0)
		{
				app_m3_gpio_write_pin(GPIO_PIN_11, GPIO_HIGH);
				interrupt_number++;
				SysTick_Config(0xffffff);
		}
		else
		{
				app_m3_gpio_write_pin(GPIO_PIN_11, GPIO_LOW);
				NVIC_Systick_enable(0);
				systick_stop_time = NVIC_Systick_get_current_value();
			  NVIC_Systick_set_value(0);
				interrupt_number = 0;
				app_m3_gpio_disable_interrupt(GPIO_PIN_9);
				cobra_app_set_trim_task();
		}
}

void app_uart_rx_pin_interrupt_init(void)
{
#if 1
		cobra_map_pinmodeset(PIN9_INDEX, PIN9_GPIO1_PIN1);
		cobra_pin_pullup_enable(PIN9_INDEX, 1);
		cobra_pin_input_enable(PIN9_INDEX, 1);
		app_m3_gpio_set_direction(GPIO_PIN_9, GPIO_INPUT);
		app_m3_gpio_enable_interrupt(GPIO_PIN_9, GPIO_LEVEL_LOW, app_uart_rx_pin_interrupt_function);		
#else	
	  cobra_map_pinmodeset(PIN11_INDEX, PIN11_GPIO1_PIN3);
		cobra_pin_pullup_enable(PIN11_INDEX, 1);
		cobra_pin_input_enable(PIN11_INDEX, 1);
		app_m3_gpio_set_direction(GPIO_PIN_11, GPIO_INPUT);
		app_m3_gpio_enable_interrupt(GPIO_PIN_11, GPIO_EDGE_RISE, app_uart_rx_pin_interrupt_function);		
#endif	
		interrupt_number = 0;
}

void app_trim_init(void)
{
		cobra_aon_pmu_protection_en(1);
		cobra_aon_pmu_aon_hf_clk_cfg(HF_CLK_RCOSC8M);
		cobra_aon_pmu_protection_en(0);	

		cobra_system_clock_reset_setting();
	
		NVIC_Systick_enable(0);
		NVIC_Systick_interrupt_enable(0);
		NVIC_Systick_system_clock_enable(1);
	
		app_uart_rx_pin_interrupt_init();
	
		NVIC_Systick_enable(1);
			
}

/*
Readme
Main program no WFI
*/

void cobra_app_trim_task_loop(void)
{  
		int8_t trim_value;
		uint32_t trim_hz;
		app_trim_cobra_uart_baud_rate(M3_UART0, ((0xffffff - systick_stop_time)/8));
	  cobra_uart_write (M3_UART0, 0x55);
		cobra_uart_write (M3_UART0, 0xAA);
		cobra_uart_write (M3_UART0, 0x0D);
		cobra_uart_write (M3_UART0, 0x0A);	
		trim_hz = ((0xffffff - systick_stop_time)/8)*600;
		cobra_aon_pmu_protection_en(1);
		trim_value = cobra_an_d_rco8m_ftune_read();
	
		if(!(trim_value & 0x80))
		{
				trim_value |= 0x20;
		}
		
		trim_value &= 0x7f;
		
		
		if(trim_hz > 8000000 + 1000000)
		{
				 trim_value += 5;
		}
		else if(trim_hz > 8000000 + 500000)
		{
				 trim_value += 2;
		}
		else if(trim_hz > 8000000 + 50000)
		{
					trim_value += 1;
		}
		else if(trim_hz < 8000000 - 1000000)
		{
					trim_value -= 5;
		}
		else if(trim_hz < 8000000 - 500000)
		{
				 trim_value -= 2;
		}
		else if(trim_hz < (8000000 - 50000))
		{
				 trim_value -= 1;
		}
		else
		{
				printf("Trim OK !!!! \n");
		}
		
		if(trim_value <= 0)
		{
				trim_value = 0;
		}
		else if(trim_value >= 0x7f)
		{			
				trim_value = 0x7f;
		}
		
		printf("hz = %d, value = %d \n", trim_hz, trim_value);
		
		cobra_an_d_rco8m_ftune_set((unsigned char)(trim_value) + 0x80);
		
		cobra_aon_pmu_protection_en(0);
		
	 
		app_m3_gpio_enable_interrupt(GPIO_PIN_9, GPIO_EDGE_RISE, app_uart_rx_pin_interrupt_function);		
		interrupt_number = 0;
		NVIC_Systick_enable(1);
	
}

#endif

