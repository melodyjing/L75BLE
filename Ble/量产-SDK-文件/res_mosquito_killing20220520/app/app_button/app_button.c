/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_button.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/4/29

	Description :   Test Button Driver from github
	Changed Log	:
	  这个代码 只是为了让SDK 使用者了解如何使用 按键 Library
		

		Liujian  2020/4/29		- Creation
*****************************************************************************/
#include "app_button.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"

#if ((COBRA_APP_BUTTON_EN == 1) && (COBRA_BUTTON_EN == 1))

#if (APP_AIRCLEAN_ENABLE == 1)
#define BUTTON_KEY_NUMBER			GPIO_PIN_24
#define BUTTON_PIN_INDEX			PIN24_INDEX
#elif (APP_RAINBOW_ENABLE == 1)
#define BUTTON_KEY_NUMBER			GPIO_PIN_24
#define BUTTON_PIN_INDEX			PIN24_INDEX
#elif (APP_MOSQUITO_KILLING_ENABLE == 1)
#define BUTTON_KEY_NUMBER			GPIO_PIN_15
#define BUTTON_PIN_INDEX			PIN15_INDEX
#else
#define BUTTON_KEY_NUMBER			GPIO_PIN_29
#define BUTTON_PIN_INDEX			PIN29_INDEX
#endif

static struct Button cobra_key_SW9;


unsigned char read_cobra_key_SW9() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(BUTTON_KEY_NUMBER);
//	printf("key_value = 0x%x \n", key_value);
	return key_value;
}

void app_button_init(void)
{
	
		 app_m3_gpio_set_direction(BUTTON_KEY_NUMBER, GPIO_INPUT);
	
		 cobra_pin_pullup_enable(BUTTON_PIN_INDEX, 1);
	
		 button_init(&cobra_key_SW9, read_cobra_key_SW9, GPIO_LOW);
		
}

void app_button_start(void)
{
			button_start(&cobra_key_SW9);
}

void app_button_stop(void)
{
			button_stop(&cobra_key_SW9);
}

void app_button_sw9_attach_event(PressEvent event, BtnCallback cb)
{
		 button_attach(&cobra_key_SW9, event, cb);
}

#if 0
/*
    下面这段代码只是演示如何使用按键功能
*/

static void app_key_sw9_key_down_press(void * param)
{
    printf("1. Key press down message .... \n");
}

static void app_key_sw9_key_up_press(void * param)
{
    printf("2. Key press up message .... \n");
}

static void app_key_sw9_key_hold_press(void * param)
{
    printf("3. Key hold down message .... \n");
}

void example_test_button(void)
{
		app_button_init();
	
		app_button_sw9_attach_event(PRESS_DOWN, app_key_sw9_key_down_press);
		app_button_sw9_attach_event(PRESS_UP, app_key_sw9_key_up_press);
	  app_button_sw9_attach_event(LONG_PRESS_HOLD, app_key_sw9_key_hold_press);
	
  	app_button_start();
}

#endif

#endif

