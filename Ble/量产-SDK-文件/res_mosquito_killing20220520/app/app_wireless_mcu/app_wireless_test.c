/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_wireless_test.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/7/9

	Description :   
	  used to test the electricity meter, one press key (TX), and light another board led (RX)   				

	Changed Log	:

		Liujian  2019/7/9			- Creation


*****************************************************************************/

#include "app_wireless_test.h"
#include "cobra_pinmux.h"
#include "rdw_gpio.h"
#include "cobra_soft_timer.h"

#if(APP_RF_NORMAL_PROTOCOL_ENABLE == 1)

#define  CSEM_DEBUG  1

#if CSEM_DEBUG
static int32_t csem_test_timer_index;
#endif

static int32_t key3_timer_index;

static unsigned char electricity_buffer[5];

static void app_key_pin3_function(unsigned char  param)
{
			cobra_soft_timer_start(key3_timer_index, 10, 0);
}


void app_elecrticity_rf_send_callback(unsigned char state, unsigned char ack_length, unsigned char * ack_buffer)
{
		unsigned char i;
		if(state)
		{
			  printf("send ok !!!!! \n");
		}
		else
		{
				printf("send failer !!!! \n");
		}
}

static unsigned char key_test = 0;
void app_key_pin3_debounce_callback(unsigned int param)
{
				if(app_m3_gpio_read_pin(GPIO_PIN_3) == GPIO_LEVEL_LOW)
				{
						printf("key 3 pressed \n");
						if(key_test)
						{
								key_test = 0;
						}
						else
						{
								key_test = 1;
						}
						electricity_buffer[0] = 0x55;
						electricity_buffer[1] = key_test;
						
						app_csem_send_setting_have_ack(electricity_buffer, 5, 9, 0x12341234, app_elecrticity_rf_send_callback);
				}	
}

#if CSEM_DEBUG
void app_test_send_callback(unsigned int param)
{
	//			printf("app_test_send_callback .... \n");
				if(key_test)
				{
						key_test = 0;
				}
				else
				{
						key_test = 1;
				}
				electricity_buffer[0] = 0x55;
				electricity_buffer[1] = key_test;
				
				app_csem_send_setting_have_ack(electricity_buffer, 5, 9, 0x12341234, app_elecrticity_rf_send_callback);
						
}
#endif
void app_cobra_electricity_gate_task_init(void)
{
//		cobra_map_pinmodeset(PIN3_INDEX, PIN3_GPIO0_PIN3);
		app_m3_gpio_set_direction(GPIO_PIN_3, GPIO_OUTPUT);
		app_m3_gpio_write_pin(GPIO_PIN_3, GPIO_HIGH);
	
		cobra_pin_pullup_enable(PIN3_INDEX, 1);
		cobra_pin_input_enable(PIN3_INDEX, 1);
		app_m3_gpio_set_direction(GPIO_PIN_3, GPIO_INPUT);
		app_m3_gpio_enable_interrupt(GPIO_PIN_3, GPIO_LEVEL_LOW, app_key_pin3_function);		
		key3_timer_index = cobra_soft_timer_create(app_key_pin3_debounce_callback);		
		
//		cobra_map_pinmodeset(PIN4_INDEX, PIN4_GPIO0_PIN4);
//		app_m3_gpio_set_direction(GPIO_PIN_4, GPIO_OUTPUT);
//		app_m3_gpio_write_pin(GPIO_PIN_4, GPIO_HIGH);
//	
//		cobra_pin_pullup_enable(PIN4_INDEX, 1);
//		cobra_pin_input_enable(PIN4_INDEX, 1);
//		app_m3_gpio_set_direction(GPIO_PIN_4, GPIO_INPUT);
//		app_m3_gpio_enable_interrupt(GPIO_PIN_4, GPIO_LEVEL_LOW, app_key_pin3_function);		
//		key3_timer_index = cobra_soft_timer_create(app_key_pin3_debounce_callback);		
	
	
#if CSEM_DEBUG	
		csem_test_timer_index = cobra_soft_timer_create(app_test_send_callback);
	
		cobra_soft_timer_start(csem_test_timer_index, 100, 1);
#endif	
	
}

///---------------------------------recevier -----------------------------------

static void cobra_electricity_device_rf_receive_callback(unsigned char *p_buffer, unsigned char length)
{
		if((p_buffer[0] == 0x55) && (p_buffer[1] == 0x01))
		{
//				app_m3_gpio_write_pin(GPIO_PIN_6, GPIO_HIGH);
//				app_m3_gpio_write_pin(GPIO_PIN_8, GPIO_HIGH);
		}
		
		if((p_buffer[0] == 0x55) && (p_buffer[1] == 0x00))
		{
//				app_m3_gpio_write_pin(GPIO_PIN_6, GPIO_LOW);
//				app_m3_gpio_write_pin(GPIO_PIN_8, GPIO_LOW);
		}
		
		printf("receive ok \n");
		
}
void app_cobra_electricity_device_task_init(void)
{
//LED    µÄ Io ¿ÚµÄÅäÖÃ	
//		cobra_map_pinmodeset(PIN6_INDEX, PIN6_GPIO0_PIN6);
//		app_m3_gpio_set_direction(GPIO_PIN_6, GPIO_OUTPUT);
//		app_m3_gpio_write_pin(GPIO_PIN_6, GPIO_HIGH);
//	
//		cobra_map_pinmodeset(PIN8_INDEX, PIN8_GPIO1_PIN0);
//		app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_OUTPUT);
//		app_m3_gpio_write_pin(GPIO_PIN_8, GPIO_HIGH);
	
		app_csem_receive_setting(9, 0x12341234, 0, NULL, cobra_electricity_device_rf_receive_callback);
	
}



#endif