/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_m0_gpio.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/27

	Description :   
	
	Changed Log	:

		Liujian  2019/5/27		- Creation



*****************************************************************************/

#ifndef __COBRA_M0_GPIO__
#define __COBRA_M0_GPIO__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_M0_GPIO_EN

typedef struct cobra_m0_gpio_type
{
		volatile __attribute__ ((aligned (4))) uint32_t   M0_DATA;
		volatile __attribute__ ((aligned (4))) uint32_t   M0_DATAOUT;
																					 uint32_t   RESERVED0[2];
	
		volatile __attribute__ ((aligned (4))) uint32_t   OUTENABLESET;
	  volatile __attribute__ ((aligned (4))) uint32_t   OUTENABLECLR;
	
	  volatile __attribute__ ((aligned (4))) uint32_t   ALTFUNCSET;
		volatile __attribute__ ((aligned (4))) uint32_t   ALTFUNCCLR;
		
		volatile __attribute__ ((aligned (4))) uint32_t   INTENSET;
	  volatile __attribute__ ((aligned (4))) uint32_t   INTENCLR;
		volatile __attribute__ ((aligned (4))) uint32_t   INTTYPESET;
	  volatile __attribute__ ((aligned (4))) uint32_t   INTTYPECLR;
		volatile __attribute__ ((aligned (4))) uint32_t   INTPOLSET;
		volatile __attribute__ ((aligned (4))) uint32_t   INTPOLCLR;
	
		union {
					volatile    uint32_t  INTSTATUS;      /*!< Offset: 0x038 Interrupt Status Register (R/ ) */
					volatile    uint32_t  INTCLEAR;       /*!< Offset: 0x038 Interrupt Clear Register ( /W) */
    };
		
		uint32_t 		RESERVED1[241];
		volatile   	uint32_t 	  LB_MASKED[256];    /*!< Offset: 0x400 - 0x7FC Lower byte Masked Access Register (R/W) */
		volatile   	uint32_t    UB_MASKED[256];    /*!< Offset: 0x800 - 0xBFC Upper byte Masked Access Register (R/W) */
		
} COBRA_M0_GPIO_TypeDef; 



typedef enum
{
    M0_GPIO_PIN_0 = 0,				
    M0_GPIO_PIN_1 = 1,
    M0_GPIO_PIN_2 = 2,
    M0_GPIO_PIN_3 = 3,
    M0_GPIO_PIN_4 = 4,
    M0_GPIO_PIN_5 = 5,
    M0_GPIO_PIN_6 = 6,
    M0_GPIO_PIN_7 = 7,
	
		M0_GPIO_PIN_8 = 8,              
    M0_GPIO_PIN_9 = 9,
    M0_GPIO_PIN_10 = 10,
    M0_GPIO_PIN_11 = 11,
    M0_GPIO_PIN_12 = 12,
    M0_GPIO_PIN_13 = 13,
    M0_GPIO_PIN_14 = 14,
    M0_GPIO_PIN_15 = 15,
	

    M0_GPIO_PIN_MAX = 16
	
}m0_gpio_pin;

#define M0_GPIO_BASE 	(0x40110000UL)



void cobra_m0_gpio_init(void) ;
void cobra_m0_gpio_set_output(m0_gpio_pin pin);
void cobra_m0_gpio_set_input(m0_gpio_pin pin);
void cobra_m0_gpio_set_value(m0_gpio_pin pin, bool value);
bool cobra_m0_gpio_get_value(m0_gpio_pin pin);

#endif   //COBRA_M0_GPIO_EN



#endif

