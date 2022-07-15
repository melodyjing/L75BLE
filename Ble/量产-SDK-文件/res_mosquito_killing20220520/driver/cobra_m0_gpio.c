/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_m0_gpio.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/27

	Description :   


	Changed Log	:

		Liujian  2019/5/27			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_m0_gpio.h"


#if COBRA_M0_GPIO_EN

static volatile COBRA_M0_GPIO_TypeDef * cobra_m0_gpio;


void cobra_m0_gpio_init(void) 
{
		cobra_m0_gpio = (COBRA_M0_GPIO_TypeDef *)M0_GPIO_BASE;

}


bool cobra_m0_gpio_get_value(m0_gpio_pin pin)
{
		if(cobra_m0_gpio->M0_DATA & ( 1<<pin))
		{
				return true;
		}
		else
		{
				return false;
		}		
}


void cobra_m0_gpio_set_value(m0_gpio_pin pin, bool value)
{
		if(pin <= M0_GPIO_PIN_7)
		{
				if(value)
				{
						cobra_m0_gpio->LB_MASKED[1<<pin] = 0xff;
				}
				else
				{
						cobra_m0_gpio->LB_MASKED[1<<pin] = 0x00;
				}
		}
		else
		{
				if(value)
				{
						cobra_m0_gpio->UB_MASKED[1<< (pin - M0_GPIO_PIN_8) ] = 0xff;
				}
				else
				{
						cobra_m0_gpio->UB_MASKED[1<<(pin - M0_GPIO_PIN_8)] = 0x00;
				}
		}
}


void cobra_m0_gpio_set_output(m0_gpio_pin pin)
{
		cobra_m0_gpio->OUTENABLESET |= (1<<pin);

}

void cobra_m0_gpio_set_input(m0_gpio_pin pin)
{
		cobra_m0_gpio->OUTENABLECLR |= (1<<pin);
}


#endif














