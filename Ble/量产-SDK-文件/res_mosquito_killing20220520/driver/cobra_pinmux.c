/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_pinmux.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/27

	Description :   come from coney project


	Changed Log	:

		Liujian  2019/3/27			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_pinmux.h"


#if COBRA_PINMUX_EN

#if (COBRA_POWER_DOWN_ENABLE)
volatile COBRA_PINMUX_TypeDef *cobra_pinmux __attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_PINMUX_TypeDef * cobra_pinmux;
#endif

void cobra_pinmux_init(void)
{
		cobra_pinmux = (COBRA_PINMUX_TypeDef * )COBRA_PINMUX_BASE;
}

//static void pin0_function(PIN_MODE  mode)
//{
//	volatile unsigned int temp;
//	temp = cobra_pinmux->PIN0_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN0_MUX = temp;
////	cobra_pinmux->PIN0_MUX &= ~0x0f;
////	cobra_pinmux->PIN0_MUX |= mode;
//}

//static void pin1_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	  temp = cobra_pinmux->PIN1_MUX;
//    temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	  cobra_pinmux->PIN1_MUX = temp;
//	
////	cobra_pinmux->PIN1_MUX &= ~0x0f;
////	cobra_pinmux->PIN1_MUX |= mode;
//}

//static void pin2_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN2_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN2_MUX = temp;
//	
////	cobra_pinmux->PIN2_MUX &= ~0x0f;
////	cobra_pinmux->PIN2_MUX |= mode;
//}

//static void pin3_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN3_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN3_MUX = temp;
//	
////	cobra_pinmux->PIN3_MUX &= ~0x0f;
////	cobra_pinmux->PIN3_MUX |= mode;
//}

//static void pin4_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN4_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN4_MUX = temp;
//	
////	cobra_pinmux->PIN4_MUX &= ~0x0f;
////	cobra_pinmux->PIN4_MUX |= mode;
//}

//static void pin5_function(PIN_MODE mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN5_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN5_MUX = temp;
//	
////	cobra_pinmux->PIN5_MUX &= ~0x0f;
////	cobra_pinmux->PIN5_MUX |= mode;
//}

//static void pin6_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN6_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN6_MUX = temp;
//	
////	cobra_pinmux->PIN6_MUX &= ~0x0f;
////	cobra_pinmux->PIN6_MUX |= mode;
//}

//static void pin7_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN7_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN7_MUX = temp;
//	
////	cobra_pinmux->PIN7_MUX &= ~0x0f;
////	cobra_pinmux->PIN7_MUX |= mode;
//}


//static void pin8_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN8_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN8_MUX  = temp;
//	
////	cobra_pinmux->PIN8_MUX &= ~0x0f;
////	cobra_pinmux->PIN8_MUX |= mode;
//}

//static void pin9_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN9_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN9_MUX  = temp;
//	
////	cobra_pinmux->PIN9_MUX &= ~0x0f;
////	cobra_pinmux->PIN9_MUX |= mode;
//}

//static void pin10_function(PIN_MODE mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN10_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN10_MUX = temp;
//	
////	cobra_pinmux->PIN10_MUX &= ~0x0f;
////	cobra_pinmux->PIN10_MUX |= mode;
//}

//static void pin11_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN11_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN11_MUX = temp;
//	
////	cobra_pinmux->PIN11_MUX &= ~0x0f;
////	cobra_pinmux->PIN11_MUX |= mode;
//}

//static void pin12_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN12_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN12_MUX = temp;
//	
////	cobra_pinmux->PIN12_MUX &= ~0x0f;
////	cobra_pinmux->PIN12_MUX |= mode;
//}

//static void pin13_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN13_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN13_MUX = temp;
//	
////	cobra_pinmux->PIN13_MUX &= ~0x0f;
////	cobra_pinmux->PIN13_MUX |= mode;
//}

//static void pin14_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN14_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN14_MUX = temp;
//	
////	cobra_pinmux->PIN14_MUX &= ~0x0f;
////	cobra_pinmux->PIN14_MUX |= mode;
//}

//static void pin15_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN15_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN15_MUX  = temp;
//	
////	cobra_pinmux->PIN15_MUX &= ~0x0f;
////	cobra_pinmux->PIN15_MUX |= mode;
//}

//static void pin16_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN16_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN16_MUX = temp;
//	
////	cobra_pinmux->PIN16_MUX &= ~0x0f;
////	cobra_pinmux->PIN16_MUX |= mode;
//}

//static void pin17_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN17_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN17_MUX = temp;
//	
////	cobra_pinmux->PIN17_MUX &= ~0x0f;
////	cobra_pinmux->PIN17_MUX |= mode;
//}

//static void pin18_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN18_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN18_MUX = temp;
//	
////	cobra_pinmux->PIN18_MUX &= ~0x0f;
////	cobra_pinmux->PIN18_MUX |= mode;
//}

//static void pin19_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN19_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN19_MUX = temp;
//	
////	cobra_pinmux->PIN19_MUX &= ~0x0f;
////	cobra_pinmux->PIN19_MUX |= mode;
//}

//static void pin20_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN20_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN20_MUX = temp;
//	
//	
////	cobra_pinmux->PIN20_MUX &= ~0x0f;
////	cobra_pinmux->PIN20_MUX |= mode;
//}

//static void pin21_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN21_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN21_MUX = temp;
//	
////	cobra_pinmux->PIN21_MUX &= ~0x0f;
////	cobra_pinmux->PIN21_MUX |= mode;
//}

//static void pin22_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN22_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN22_MUX = temp;
//	
////	cobra_pinmux->PIN22_MUX &= ~0x0f;
////	cobra_pinmux->PIN22_MUX |= mode;
//}

//static void pin23_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN23_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN23_MUX = temp;
//	
////	cobra_pinmux->PIN23_MUX &= ~0x0f;
////	cobra_pinmux->PIN23_MUX |= mode;
//}

//static void pin24_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN24_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN24_MUX = temp;
//	
////	cobra_pinmux->PIN24_MUX &= ~0x0f;
////	cobra_pinmux->PIN24_MUX |= mode;
//}

//static void pin25_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN25_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN25_MUX = temp;
//	
////	cobra_pinmux->PIN25_MUX &= ~0x0f;
////	cobra_pinmux->PIN25_MUX |= mode;
//}

//static void pin26_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	  temp = cobra_pinmux->PIN26_MUX;
//	  temp &= ~0x1800;
//	  temp &= ~0x0f;
//	  temp |= (mode&0x0f);
//	  temp |= ((mode&0x30) << 7);
//	  cobra_pinmux->PIN26_MUX = temp;
//	
////	cobra_pinmux->PIN26_MUX &= ~0x0f;
////	cobra_pinmux->PIN26_MUX |= mode;
//}

//static void pin27_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	
//		temp = cobra_pinmux->PIN27_MUX;
//		temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//		cobra_pinmux->PIN27_MUX =temp;
//}

//static void pin28_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN28_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN28_MUX = temp;
//	
////	cobra_pinmux->PIN28_MUX &= ~0x0f;
////	cobra_pinmux->PIN28_MUX |= mode;
//}

#if (COBRA_LIGHT_WS2812B_ENABLE == 1)
void inline cobra_ws2812_set_high(void)
{
	 cobra_pinmux->PIN28_MUX = 0x1f;
}

void inline cobra_ws2812_set_low(void)
{
	 cobra_pinmux->PIN28_MUX = 0x1e;
}

void app_light_ws2812b_light_display(uint32_t data)
{
	  uint32_t i ;
	  volatile uint32_t k;
	  for(i = 0; i < 24; i++)
	  {
			  if(data & (1 << i))
				{
					  cobra_ws2812_set_high();
					  for(k = 0; k < 2; k++)
					  {
									__nop();
									__nop();
									__nop();
									__nop();
									__nop();
									__nop();
						}
					  cobra_ws2812_set_low();
					  __nop();
				}
				else
				{
					  cobra_ws2812_set_high();
					  __nop();
					  cobra_ws2812_set_low();
					  for(k = 0; k < 2; k++)
					  {
									__nop();
									__nop();
									__nop();
									__nop();
						}
				}
		}
}

#endif

//static void pin29_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	temp = cobra_pinmux->PIN29_MUX;
//	temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	cobra_pinmux->PIN29_MUX = temp;
//	
////	cobra_pinmux->PIN29_MUX &= ~0x0f;
////	cobra_pinmux->PIN29_MUX |= mode;
//}

//static void pin30_function(PIN_MODE  mode)
//{
//		volatile unsigned int temp;
//	  temp = cobra_pinmux->PIN30_MUX;
//	  temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	  cobra_pinmux->PIN30_MUX  = temp;
//	

//}

//static void pin31_function(PIN_MODE  mode)
//{
//	  volatile unsigned int temp;
//	  temp = cobra_pinmux->PIN31_MUX;
//	  temp &= ~0x1800;
//		temp &= ~0x0f;
//		temp |= (mode&0x0f);
//		temp |= ((mode&0x30) << 7);
//	  cobra_pinmux->PIN31_MUX = temp;
//	

//}


/*
	PINMUX_INDEX, 代表是哪一个 PIN 脚 PIN0_INDEX, PIN1_INDEX ....
  PIN_MODE  是每一个脚的功能，　在 include 下有每一个脚的功能定义

*/
void cobra_map_pinmodeset(PINMUX_INDEX  pin,  PIN_MODE mode)
{
		volatile unsigned int temp,*p;
	  p = &(cobra_pinmux->PIN0_MUX);
		temp =*(&(cobra_pinmux->PIN0_MUX)+pin);
		temp &= ~0x180f;
		temp |= (mode&0x0f);
		temp |= ((mode&0x30) << 7);
	  p = p+pin;
		*p = temp;
}

#if 0
void cobra_map_pinmodeset(PINMUX_INDEX  pin,  PIN_MODE mode)
{
	switch(pin)
	{
		case PIN0_INDEX:
			pin0_function(mode);
		break;
		case PIN1_INDEX:
		    pin1_function(mode);
		break;
		case PIN2_INDEX:
			pin2_function(mode);
		break;
		case PIN3_INDEX:
		    pin3_function(mode);
		break;
		case PIN4_INDEX:
			pin4_function(mode);
		break;
		case PIN5_INDEX:
			pin5_function(mode);
		break;
		case PIN6_INDEX:
			pin6_function(mode);
		break;
		case PIN7_INDEX:
			pin7_function(mode);
		break;
		case PIN8_INDEX:
			pin8_function(mode);
		break;
		case PIN9_INDEX:
			pin9_function(mode);
		break;
		case PIN10_INDEX:
			pin10_function(mode);
		break;
		case PIN11_INDEX:
			pin11_function(mode);
		break;
		case PIN12_INDEX:
			pin12_function(mode);
		break;
		case PIN13_INDEX:
			pin13_function(mode);
		break;
		case PIN14_INDEX:
			pin14_function(mode);
		break;
		case PIN15_INDEX:
			pin15_function(mode);
		break;
		case PIN16_INDEX:
			pin16_function(mode);
		break;
		case PIN17_INDEX:
			pin17_function(mode);
		break;
		case PIN18_INDEX:
			pin18_function(mode);
		break;
		case PIN19_INDEX:
			pin19_function(mode);
		break;
		case PIN20_INDEX:
			pin20_function(mode);
		break;
		case PIN21_INDEX:
			pin21_function(mode);
		break;
		case PIN22_INDEX:
			pin22_function(mode);
		break;
		case PIN23_INDEX:
			pin23_function(mode);
		break;
		case PIN24_INDEX:
			pin24_function(mode);
		break;
		case PIN25_INDEX:
		    pin25_function(mode);
		break;
		case PIN26_INDEX:
			pin26_function(mode);
		break;
		case PIN27_INDEX:
			pin27_function(mode);
		break;
		case PIN28_INDEX:
			pin28_function(mode);
		break;
		case PIN29_INDEX:
			pin29_function(mode);
		break;
		case PIN30_INDEX:
			pin30_function(mode);
		break;
		case PIN31_INDEX:
			pin31_function(mode);
		break;
		default:
		break;
	}
}
#endif

unsigned int cobra_pin_get_pin_mode(PINMUX_INDEX pin)
{
	  volatile unsigned  int *p_temp;
	  volatile unsigned  int temp,ret =0x00; 
		p_temp = &(cobra_pinmux->PIN0_MUX);		
	  temp = *(p_temp + pin);
	  ret = temp &0x0f;
	  ret |= (temp>>7)&0x30;
	
	  return ret;
}


void cobra_pin_input_enable(PINMUX_INDEX  pin,  unsigned int en)
{
		volatile unsigned  int *p_temp;
	  volatile unsigned  int temp; 
		p_temp = &(cobra_pinmux->PIN0_MUX);		
	  temp = *(p_temp + pin);

		if(en)
		{
//lianbo
//				temp &= ~0x0f;
//				temp |= 0x0d;         //output z			
				temp |= (1<<7);
				*(p_temp + pin) = temp;  //(1<<7);
		}
		else
		{
				*(p_temp + pin) &= ~(1<<7);
		}
	
}

void cobra_pin_pullup_enable(PINMUX_INDEX  pin,  unsigned int en)
{
		volatile unsigned  int *p_temp;
		p_temp = &(cobra_pinmux->PIN0_MUX);
		
		if(en)
		{
				*(p_temp + pin) |= (1<<4);
		}
		else
		{
				*(p_temp + pin) &= ~(1<<4);
		}
	
}


/*
 这个是对IO 口反转

*/
void cobra_pin_inverse_input_enable(PINMUX_INDEX pin, unsigned int en)
{
	volatile unsigned int *p_temp;
	p_temp = &(cobra_pinmux->PIN0_MUX);

	if (en)
	{
		*(p_temp + pin) |= (1 << 9);
	}
	else
	{
		*(p_temp + pin) &= ~(1 << 9);
	}
}


void cobra_pin_level_detect_enable(PINMUX_INDEX  pin,  unsigned int en)
{
	  volatile unsigned  int *p_temp;
		p_temp = &(cobra_pinmux->PIN0_MUX);
		
		if(en)
		{
				*(p_temp + pin) |= (1<<6);
		}
		else
		{
				*(p_temp + pin) &= ~(1<<6);
		}
}

void cobra_pin_level_detect(PINMUX_INDEX  pin,  unsigned int level)
{
	  volatile unsigned  int *p_temp;
		p_temp = &(cobra_pinmux->PIN0_MUX);
		
		if(level)   //level high
		{
				*(p_temp + pin) |= (1<<10);
		}
		else         //level low
		{
				*(p_temp + pin) &= ~(1<<10);
		}
}


unsigned int cobra_pin_level_detect_get(PINMUX_INDEX pin)
{
	volatile unsigned int *p_temp;
	p_temp = &(cobra_pinmux->PIN0_MUX);


	return (*(p_temp + pin) >> 10) & 0x1;
}


/*
Drain source enable
*/
void cobra_pin_drain_source_enable(PINMUX_INDEX  pin,  unsigned int enbale)
{
	  volatile unsigned  int *p_temp;
		p_temp = &(cobra_pinmux->PIN0_MUX);
		
		if(enbale)   //drain source enable
		{
				*(p_temp + pin) |= (1<<5);
		}
		else         //drain source disable
		{
				*(p_temp + pin) &= ~(1<<5);
		}
}




#endif














