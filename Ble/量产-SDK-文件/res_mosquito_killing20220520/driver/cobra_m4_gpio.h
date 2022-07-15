/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_m4_gpio.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/5

	Description :   
	
	Changed Log	:

		Liujian  2019/8/5		- Creation



*****************************************************************************/
#ifndef __COBRA_M4_GPIO_H__
#define __COBRA_M4_GPIO_H__
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA4_GPIO_EN



/// Enumeration of GPIO pins
typedef enum
{
    GPIO_PIN_0 = 0,				//cobra4 GPIO 
    GPIO_PIN_1 = 1,
    GPIO_PIN_2 = 2,
    GPIO_PIN_3 = 3,
    GPIO_PIN_4 = 4,
    GPIO_PIN_5 = 5,
    GPIO_PIN_6 = 6,
    GPIO_PIN_7 = 7,
	
		GPIO_PIN_8 = 8,               // cobra4 GPIO 
    GPIO_PIN_9 = 9,
    GPIO_PIN_10 = 10,
    GPIO_PIN_11 = 11,
    GPIO_PIN_12 = 12,
    GPIO_PIN_13 = 13,
    GPIO_PIN_14 = 14,
    GPIO_PIN_15 = 15,
	
		GPIO_PIN_16 = 16,               // cobra4 GPIO 
    GPIO_PIN_17 = 17,
    GPIO_PIN_18 = 18,
    GPIO_PIN_19 = 19,
    GPIO_PIN_20 = 20,
    GPIO_PIN_21 = 21,
    GPIO_PIN_22 = 22,
    GPIO_PIN_23 = 23,
		
		GPIO_PIN_24 = 24,               // cobra4 GPIO 
    GPIO_PIN_25 = 25,
    GPIO_PIN_26 = 26,
    GPIO_PIN_27 = 27,
    GPIO_PIN_28 = 28,
    GPIO_PIN_29 = 29,
    GPIO_PIN_30 = 30,
    GPIO_PIN_31 = 31,
		

    GPIO_PIN_MAX = 32
	
}gpio_pin;


typedef enum
{
    GPIO_LOW = 0,
    GPIO_HIGH  = 1
}gpio_level;

typedef enum
{
		GPIO_OUTPUT = 0,
		GPIO_INPUT = 1,
	  GPIO_HIGH_Z = 2,
}gpio_dir;


typedef enum
{
	  GPIO_LEVEL_HIGH = 0,
	  GPIO_LEVEL_LOW  = 1,
}gpio_interrupt_type;

typedef struct {
		volatile __attribute__ ((aligned (4))) unsigned int resever1[0x30];
		
		volatile __attribute__ ((aligned (4))) unsigned int  PAD_TRIGGER_NUMBER;  //0x30, 哪一个IO 口产生的中端 
	
	  volatile __attribute__ ((aligned (4))) unsigned int  ADDR_GPIO_INPUT;  	//0x31
	
		volatile __attribute__ ((aligned (4))) unsigned int  resever2;   //0x32
		volatile __attribute__ ((aligned (4))) unsigned int  resever3;   //0x33
		volatile __attribute__ ((aligned (4))) unsigned int  resever4;   //0x34
		volatile __attribute__ ((aligned (4))) unsigned int  resever5;   //0x35
	
		volatile __attribute__ ((aligned (4))) unsigned int  POWER_OFF_CTRL;   //0x36
		
}COBRA_M4_GPIO_TypeDef;

#define M4_GPIO_BASE 	(0x40080000UL + 0x9000UL)



void cobra_m4_gpio_init(void) ;
void cobra_m4_gpio_set_value(gpio_pin pin,bool value);
bool cobra_m4_gpio_get_value(gpio_pin pin);

void cobra_m4_gpio_set_output(gpio_pin pin);
void cobra_m4_gpio_set_input(gpio_pin pin);
void cobra_m4_gpio_set_high_z(gpio_pin pin);

void cobra_m4_gpio_interrupt_disable(gpio_pin pin);
void cobra_m4_gpio_interrupt_enable(gpio_pin pin);

void cobra_m4_gpio_interrupt_leve(gpio_pin pin, unsigned int level);


unsigned int cobra_m4_gpio_interrupt_level_get(gpio_pin pin);


uint32_t cobra_m4_get_interrupt_status(void);
uint32_t cobra_m4_get_current_io_state(void);

void cobra_m4_power_off_enable(unsigned int en);

unsigned int cobra_m4_gpio_get_pin_mode(gpio_pin pin);

#endif

#endif

