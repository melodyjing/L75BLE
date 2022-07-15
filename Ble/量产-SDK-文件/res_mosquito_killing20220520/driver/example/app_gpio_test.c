#include "rdw_gpio.h"
#include "cobra_m3_gpio.h"
#include "cobra_m4_gpio.h"
#include "cobra_pcrm.h"
#include "rdw_mcu.h"


void gpio_value_inverse(gpio_pin pin)
{
	bool value;
	value = pinX_inverse(pin);
  printf("gpio_value=%x",value);	
}

void gpio_inverse_test(gpio_pin pin)
{
	cobra_soft_delay_ms(100);
	gpio_value_inverse(GPIO_PIN_27);
}