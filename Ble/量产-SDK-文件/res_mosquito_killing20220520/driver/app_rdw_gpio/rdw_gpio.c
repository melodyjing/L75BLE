/*
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
*/	
//liujian add
//2018/8/6
//GPIO DRIVER

//2019/4/3
//liujian modified again for cortex M3, not for m0 GPIO
#include "rdw_gpio.h"
#include "cobra_m3_gpio.h"
#include "cobra_m4_gpio.h"
#include "cobra_pcrm.h"
#include "rdw_mcu.h"
#include "ll.h"
#include "cobra_pinmux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cobra_config.h"

/// GPIO environment structure
struct gpio_env_tag
{
    /// Callback function pointer for level detection
    gpio_callback_t callback[GPIO_PIN_MAX];
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// GPIO environment
#if (COBRA_POWER_DOWN_ENABLE)
static struct gpio_env_tag  gpio_env		__attribute__((section("retention_area"),zero_init)); 
#else
static struct gpio_env_tag  gpio_env;
#endif



void app_m3_gpio_init(void)
{
    // Initialize environment
	  unsigned int i;
    memset(&gpio_env, 0, sizeof(gpio_env));
	
		for(i = 0; i < 31; i++)
		{
				app_m3_gpio_disable_interrupt(GPIO_PIN_0+i);
		}

}

gpio_level app_m3_gpio_read_pin(gpio_pin pin)
{
    gpio_level level;
 
		level = cobra_m4_gpio_get_value(pin)?GPIO_HIGH:GPIO_LOW;
		
    return level;
}

void app_m3_gpio_write_pin(gpio_pin pin,  gpio_level level)
{

		if(level)
		{
				cobra_m4_gpio_set_value(pin, 1);
		}
		else
		{
			 cobra_m4_gpio_set_value(pin, 0);
		}
}


void app_m3_gpio_toggle_pin(gpio_pin pin)
{
     if(cobra_m4_gpio_get_pin_mode(pin) == PIN_MODE_14)
	   {
		     cobra_m4_gpio_set_value(pin, 1);
	   }
	   else //if(cobra_m4_gpio_get_pin_mode(pin) == PIN_MODE_15)
		 {
			   cobra_m4_gpio_set_value(pin, 0);
		 }

}

void app_m3_gpio_set_direction(gpio_pin pin,  gpio_dir dir)
{
	
			if(dir == GPIO_OUTPUT)
			{
					cobra_m4_gpio_set_output(pin);
			 
			}
			else if(dir == GPIO_INPUT)
			{
					cobra_m4_gpio_set_input(pin);
			}
			else if( dir == GPIO_HIGH_Z)
			{
					cobra_m4_gpio_set_high_z(pin);
			}
}


void app_m3_gpio_enable_interrupt(gpio_pin pin, gpio_interrupt_type interrupt_type, gpio_callback_t p_callback)
{
			unsigned char type;

			type = interrupt_type;

		
			switch(type)
			{
					case GPIO_LEVEL_HIGH:
					{
							cobra_m4_gpio_interrupt_leve(pin,  1);
					}
					break;
					case GPIO_LEVEL_LOW:
					{
						 cobra_m4_gpio_interrupt_leve(pin,  0);						
					}
					break;
					default:					
					break;
			}
			
			cobra_m4_gpio_interrupt_enable(pin);
			// Store callback
			gpio_env.callback[pin] = p_callback;


}

void app_m3_gpio_disable_interrupt(gpio_pin pin)
{

		cobra_m4_gpio_interrupt_disable(pin);
    // Clear callback
    gpio_env.callback[pin] = NULL;

}

void cobra_m4_gpio_interrupt(void)
{
		uint8_t  value; 
		uint8_t pin = GPIO_PIN_0;
		volatile uint32_t	 interrupt_value; 

		interrupt_value = cobra_m4_get_interrupt_status();

    while (pin < GPIO_PIN_MAX)
    {
        // Check if level detection is enabled
        if(interrupt_value & 0x1)
        {
            // Check if line state has changed
						// Call handler
						if(gpio_env.callback[pin])
						{
								value = cobra_m4_gpio_get_value(pin);
								gpio_env.callback[pin]((uint8_t) (value));
						}
            
        }
        pin++;
        interrupt_value >>= 1;
    }
}









