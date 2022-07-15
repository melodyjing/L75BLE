/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_m3_gpio.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/2

	Description :   


	Changed Log	:

		Liujian  2019/4/2			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_m3_gpio.h"


#if COBRA_M3_GPIO_EN

static volatile COBRA_M3_GPIO_TypeDef * cobra_m3_gpio0;
static volatile COBRA_M3_GPIO_TypeDef * cobra_m3_gpio1;

void cobra_m3_gpio_init(void) 
{
		cobra_m3_gpio0 = (COBRA_M3_GPIO_TypeDef *)M3_GPIO0_BASE;
		cobra_m3_gpio1 = (COBRA_M3_GPIO_TypeDef *)M3_GPIO1_BASE;
}

void cobra_m3_gpio_interrupt_sense_set(GPIO_INDEX io_index, gpio_pin pin, GPIO_INT_TYPE type)
{
		if(io_index == M3_GPIO0)
		{
				if(type == INT_LEVEL)
				{
						cobra_m3_gpio0->IS.val |= (1<<pin);		//level
				}
				else if(type == INT_EDGE)
				{
						cobra_m3_gpio0->IS.val &=~(1<<pin);		//edge
				}
		}
		else if(io_index == M3_GPIO1)
		{
				if(type == INT_LEVEL)
				{
						cobra_m3_gpio1->IS.val |= (1<<pin);		//level
				}
				else if(type == INT_EDGE)
				{
						cobra_m3_gpio1->IS.val &=~(1<<pin);		//edge
				}
		}
		
		return;
}

void cobra_m3_gpio_interrupt_disable(GPIO_INDEX io_index, gpio_pin pin)
{
		if(io_index == M3_GPIO0)
		{
				cobra_m3_gpio0->IE.val &= ~(1<<pin);
		}
		else if(io_index == M3_GPIO1)
		{
				cobra_m3_gpio1->IE.val &= ~(1<<pin);
		}
		
		return;
}

bool cobra_m3_gpio_get_value(GPIO_INDEX io_index, gpio_pin pin)
{
	
		if(io_index == M3_GPIO0)
		{
				return ((1<<pin)&(cobra_m3_gpio0->DATA[1<<(pin+2)].val));
		}
		else if(io_index == M3_GPIO1)
		{
				return ((1<<pin)&(cobra_m3_gpio1->DATA[1<<(pin+2)].val));
		}
		
		return 0;
}


void cobra_m3_gpio_set_value(GPIO_INDEX io_index, gpio_pin pin,bool value)
{
		if(io_index == M3_GPIO0)
		{
				cobra_m3_gpio0->DATA[1<<(pin+2)].val=(value<<pin);
		}
		else if(io_index == M3_GPIO1)
		{
				cobra_m3_gpio1->DATA[1<<(pin+2)].val=(value<<pin);
		}
}


void cobra_m3_gpio_set_output(GPIO_INDEX io_index, gpio_pin pin)
{

	if(io_index == M3_GPIO0)
	{
			cobra_m3_gpio0->AFSEL.val=0;
			cobra_m3_gpio0->DIR.val |= 1 << pin;
	}
	else if(io_index == M3_GPIO1)
	{
			cobra_m3_gpio1->AFSEL.val=0;
			cobra_m3_gpio1->DIR.val |= 1 << pin;
	}
}


void cobra_m3_gpio_set_input(GPIO_INDEX io_index, gpio_pin pin)
{
		if(io_index == M3_GPIO0)
		{
				cobra_m3_gpio0->AFSEL.val=0;
				cobra_m3_gpio0->DIR.val &=~( 1 << pin );
		}
		else if(io_index == M3_GPIO1)
		{
				cobra_m3_gpio1->AFSEL.val=0;
				cobra_m3_gpio1->DIR.val &=~( 1 << pin );
		}
}


void cobra_m3_gpio_interrupt_edge_set(GPIO_INDEX io_index, gpio_pin pin, bool type)
{
		if(io_index == M3_GPIO0)
		{
				if(type)
					cobra_m3_gpio0->IBE.val |= (1<<pin);		//edge
				else
					cobra_m3_gpio0->IBE.val &=~(1<<pin);		//even
		}
		else if(io_index == M3_GPIO1)
		{
				if(type)
					cobra_m3_gpio1->IBE.val |= (1<<pin);		//edge
				else
					cobra_m3_gpio1->IBE.val &=~(1<<pin);		//even
		}
}

void cobra_m3_gpio_interrupt_even_set(GPIO_INDEX io_index, gpio_pin pin, bool type)
{
		if(io_index == M3_GPIO0)
		{
				if(type)
					cobra_m3_gpio0->IEV.val |= (1<<pin);		//rising
				else
					cobra_m3_gpio0->IEV.val &=~(1<<pin);		//falling
		}
		else if(io_index == M3_GPIO1)
		{
				if(type)
					cobra_m3_gpio1->IEV.val |= (1<<pin);		//rising
				else
					cobra_m3_gpio1->IEV.val &=~(1<<pin);		//falling
		}

}

void cobra_m3_gpio_interrupt_enable(GPIO_INDEX io_index, gpio_pin pin)
{	
		if(io_index == M3_GPIO0)
		{
				cobra_m3_gpio0->IE.val |= (1<<pin);
		}
		else if(io_index == M3_GPIO1)
		{
				cobra_m3_gpio1->IE.val |= (1<<pin);
		}
}



void cobra_m3_gpio_interrupt_clear(GPIO_INDEX io_index,   gpio_pin pin)
{

		if(io_index == M3_GPIO0)
		{
				cobra_m3_gpio0->IC.val |= (1<<pin);
		}
		else if(io_index == M3_GPIO1)
		{
				cobra_m3_gpio1->IC.val |= (1<<pin);
		}
}

uint32_t cobra_m3_gpio_interrupt_status(GPIO_INDEX io_index, gpio_pin pin,bool bMask)
{

		if(io_index == M3_GPIO0)
		{
			if(bMask)
				return (cobra_m3_gpio0->RIS.val & (1<<pin));
			else
				return (cobra_m3_gpio0->MIS.val & (1<<pin));
		}
		else if(io_index == M3_GPIO1)
		{
			if(bMask)
				return (cobra_m3_gpio1->RIS.val & (1<<pin));
			else
				return (cobra_m3_gpio1->MIS.val & (1<<pin));
		}
		
		return 0;
}


void cobra_m3_gpio_set_interrupt_status_value(GPIO_INDEX io_index, uint32_t value)
{
			if(io_index == M3_GPIO0)
			{
					cobra_m3_gpio0->IC.val = value;
			}
			else if(io_index == M3_GPIO1)
			{
					cobra_m3_gpio1->IC.val = value;
			}
}


uint32_t cobra_m3_gpio_get_interrupt_status(GPIO_INDEX io_index)
{
		if(io_index == M3_GPIO0)
		{
				return cobra_m3_gpio0->MIS.val;
		}
		else if(io_index == M3_GPIO1)
		{
				return cobra_m3_gpio1->MIS.val;
		}
		return 0;
}


uint32_t cobra_m3_gpio_get_mask_status(GPIO_INDEX io_index)
{

		if(io_index == M3_GPIO0)
		{
				return cobra_m3_gpio0->IE.val ;
		}
		else if(io_index == M3_GPIO1)
		{
				return cobra_m3_gpio1->IE.val ;
		}
		return 0;
}


#endif














