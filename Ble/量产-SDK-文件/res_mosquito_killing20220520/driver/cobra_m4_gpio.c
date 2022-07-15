/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_m4_gpio.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/5

	Description :   


	Changed Log	:

		Liujian  2019/8/5			- Creation



*****************************************************************************/
#include "cobra_m4_gpio.h"
#include "cobra_pinmux.h"

#if COBRA4_GPIO_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_M4_GPIO_TypeDef *cobra_m4_gpio0 __attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_M4_GPIO_TypeDef * cobra_m4_gpio0;
#endif

void cobra_m4_gpio_init(void) 
{
		cobra_m4_gpio0 = (volatile COBRA_M4_GPIO_TypeDef *)(M4_GPIO_BASE);
		cobra_m4_gpio0->PAD_TRIGGER_NUMBER = 0;
}

uint32_t cobra_m4_get_interrupt_status(void)
{
		return cobra_m4_gpio0->PAD_TRIGGER_NUMBER;
}

uint32_t cobra_m4_get_current_io_state(void)
{
		return cobra_m4_gpio0->ADDR_GPIO_INPUT;
}


bool cobra_m4_gpio_get_value(gpio_pin pin)
{
		volatile unsigned int temp_io_value = cobra_m4_gpio0->ADDR_GPIO_INPUT;
		if(temp_io_value & (1 << pin)) return true;
		return false;
	
}

unsigned int cobra_m4_gpio_get_pin_mode(gpio_pin pin)
{
	        return cobra_pin_get_pin_mode((PINMUX_INDEX)pin);
//	
//	      if(pin == GPIO_PIN_0)
//				{
//						return cobra_pin_get_pin_mode(PIN0_INDEX);
//				}
//				else if(pin == GPIO_PIN_1)
//				{
//						return cobra_pin_get_pin_mode(PIN1_INDEX);
//				}
//				else if(pin == GPIO_PIN_2)
//				{
//						return cobra_pin_get_pin_mode(PIN2_INDEX);
//				}
//				else if(pin == GPIO_PIN_3)
//				{
//						return cobra_pin_get_pin_mode(PIN3_INDEX);
//				}
//				else if(pin == GPIO_PIN_4)
//				{
//						return cobra_pin_get_pin_mode(PIN4_INDEX);
//				}
//				else if(pin == GPIO_PIN_5)
//				{
//						return cobra_pin_get_pin_mode(PIN5_INDEX);
//				}
//				else if(pin == GPIO_PIN_6)
//				{
//						return cobra_pin_get_pin_mode(PIN6_INDEX);
//				}
//				else if(pin == GPIO_PIN_7)
//				{
//						return cobra_pin_get_pin_mode(PIN7_INDEX);
//				}
//				else if(pin == GPIO_PIN_8)
//				{
//						return cobra_pin_get_pin_mode(PIN8_INDEX);
//				}
//				else if(pin == GPIO_PIN_9)
//				{
//						return cobra_pin_get_pin_mode(PIN9_INDEX);
//				}
//				else if(pin == GPIO_PIN_10)
//				{
//						return cobra_pin_get_pin_mode(PIN10_INDEX);
//				}
//				else if(pin == GPIO_PIN_11)
//				{
//						return cobra_pin_get_pin_mode(PIN11_INDEX);
//				}
//				else if(pin == GPIO_PIN_12)
//				{
//						return cobra_pin_get_pin_mode(PIN12_INDEX);
//				}
//				else if(pin == GPIO_PIN_13)
//				{
//						return cobra_pin_get_pin_mode(PIN13_INDEX);
//				}
//				else if(pin == GPIO_PIN_14)
//				{
//						return cobra_pin_get_pin_mode(PIN14_INDEX);
//				}
//				else if(pin == GPIO_PIN_15)
//				{
//						return cobra_pin_get_pin_mode(PIN15_INDEX);
//				}
//				else if(pin == GPIO_PIN_16)
//				{
//						return cobra_pin_get_pin_mode(PIN16_INDEX);
//				}
//				else if(pin == GPIO_PIN_17)
//				{
//						return cobra_pin_get_pin_mode(PIN17_INDEX);
//				}
//				else if(pin == GPIO_PIN_18)
//				{
//						return cobra_pin_get_pin_mode(PIN18_INDEX);
//				}
//				else if(pin == GPIO_PIN_19)
//				{
//						return cobra_pin_get_pin_mode(PIN19_INDEX);
//				}
//				else if(pin == GPIO_PIN_20)
//				{
//						return cobra_pin_get_pin_mode(PIN20_INDEX);
//				}
//				else if(pin == GPIO_PIN_21)
//				{
//						return cobra_pin_get_pin_mode(PIN21_INDEX);
//				}
//				else if(pin == GPIO_PIN_22)
//				{
//						return cobra_pin_get_pin_mode(PIN22_INDEX);
//				}
//				else if(pin == GPIO_PIN_23)
//				{
//						return cobra_pin_get_pin_mode(PIN23_INDEX);
//				}
//				else if(pin == GPIO_PIN_24)
//				{
//						return cobra_pin_get_pin_mode(PIN24_INDEX);
//				}
//				else if(pin == GPIO_PIN_25)
//				{
//						return cobra_pin_get_pin_mode(PIN25_INDEX);
//				}
//				else if(pin == GPIO_PIN_26)
//				{
//						return cobra_pin_get_pin_mode(PIN26_INDEX);
//				}
//				else if(pin == GPIO_PIN_27)
//				{
//						return cobra_pin_get_pin_mode(PIN27_INDEX);
//				}
//				else if(pin == GPIO_PIN_28)
//				{
//						return cobra_pin_get_pin_mode(PIN28_INDEX);
//				}
//				else if(pin == GPIO_PIN_29)
//				{
//						return cobra_pin_get_pin_mode(PIN29_INDEX);
//				}
//				else if(pin == GPIO_PIN_30)
//				{
//						return cobra_pin_get_pin_mode(PIN30_INDEX);
//				}
//				else //if(pin == GPIO_PIN_31)
//				{
//						return cobra_pin_get_pin_mode(PIN31_INDEX);
//				}
}

void cobra_m4_gpio_set_value(gpio_pin pin,bool value)
{
		if(value)
		{
			  cobra_map_pinmodeset((PINMUX_INDEX)pin, PIN_MODE_15);
			
//				if(pin == GPIO_PIN_0)
//				{
//						cobra_map_pinmodeset(PIN0_INDEX, PIN0_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_1)
//				{
//						cobra_map_pinmodeset(PIN1_INDEX, PIN1_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_2)
//				{
//						cobra_map_pinmodeset(PIN2_INDEX, PIN2_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_3)
//				{
//						cobra_map_pinmodeset(PIN3_INDEX, PIN3_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_4)
//				{
//						cobra_map_pinmodeset(PIN4_INDEX, PIN4_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_5)
//				{
//						cobra_map_pinmodeset(PIN5_INDEX, PIN5_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_6)
//				{
//						cobra_map_pinmodeset(PIN6_INDEX, PIN6_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_7)
//				{
//						cobra_map_pinmodeset(PIN7_INDEX, PIN7_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_8)
//				{
//						cobra_map_pinmodeset(PIN8_INDEX, PIN8_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_9)
//				{
//						cobra_map_pinmodeset(PIN9_INDEX, PIN9_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_10)
//				{
//						cobra_map_pinmodeset(PIN10_INDEX, PIN10_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_11)
//				{
//						cobra_map_pinmodeset(PIN11_INDEX, PIN11_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_12)
//				{
//						cobra_map_pinmodeset(PIN12_INDEX, PIN12_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_13)
//				{
//						cobra_map_pinmodeset(PIN13_INDEX, PIN13_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_14)
//				{
//						cobra_map_pinmodeset(PIN14_INDEX, PIN14_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_15)
//				{
//						cobra_map_pinmodeset(PIN15_INDEX, PIN15_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_16)
//				{
//						cobra_map_pinmodeset(PIN16_INDEX, PIN16_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_17)
//				{
//						cobra_map_pinmodeset(PIN17_INDEX, PIN17_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_18)
//				{
//						cobra_map_pinmodeset(PIN18_INDEX, PIN18_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_19)
//				{
//						cobra_map_pinmodeset(PIN19_INDEX, PIN19_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_20)
//				{
//						cobra_map_pinmodeset(PIN20_INDEX, PIN20_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_21)
//				{
//						cobra_map_pinmodeset(PIN21_INDEX, PIN21_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_22)
//				{
//						cobra_map_pinmodeset(PIN22_INDEX, PIN22_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_23)
//				{
//						cobra_map_pinmodeset(PIN23_INDEX, PIN23_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_24)
//				{
//						cobra_map_pinmodeset(PIN24_INDEX, PIN24_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_25)
//				{
//						cobra_map_pinmodeset(PIN25_INDEX, PIN25_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_26)
//				{
//						cobra_map_pinmodeset(PIN26_INDEX, PIN26_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_27)
//				{
//						cobra_map_pinmodeset(PIN27_INDEX, PIN27_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_28)
//				{
//						cobra_map_pinmodeset(PIN28_INDEX, PIN28_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_29)
//				{
//						cobra_map_pinmodeset(PIN29_INDEX, PIN29_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_30)
//				{
//						cobra_map_pinmodeset(PIN30_INDEX, PIN30_IO_OUTPUT_1);
//				}
//				else if(pin == GPIO_PIN_31)
//				{
//						cobra_map_pinmodeset(PIN31_INDEX, PIN31_IO_OUTPUT_1);
//				}
		}
		else
		{
			  cobra_map_pinmodeset((PINMUX_INDEX)pin, PIN_MODE_14);
			
//				if(pin == GPIO_PIN_0)
//				{
//						cobra_map_pinmodeset(PIN0_INDEX, PIN0_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_1)
//				{
//						cobra_map_pinmodeset(PIN1_INDEX, PIN1_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_2)
//				{
//						cobra_map_pinmodeset(PIN2_INDEX, PIN2_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_3)
//				{
//						cobra_map_pinmodeset(PIN3_INDEX, PIN3_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_4)
//				{
//						cobra_map_pinmodeset(PIN4_INDEX, PIN4_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_5)
//				{
//						cobra_map_pinmodeset(PIN5_INDEX, PIN5_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_6)
//				{
//						cobra_map_pinmodeset(PIN6_INDEX, PIN6_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_7)
//				{
//						cobra_map_pinmodeset(PIN7_INDEX, PIN7_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_8)
//				{
//						cobra_map_pinmodeset(PIN8_INDEX, PIN8_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_9)
//				{
//						cobra_map_pinmodeset(PIN9_INDEX, PIN9_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_10)
//				{
//						cobra_map_pinmodeset(PIN10_INDEX, PIN10_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_11)
//				{
//						cobra_map_pinmodeset(PIN11_INDEX, PIN11_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_12)
//				{
//						cobra_map_pinmodeset(PIN12_INDEX, PIN12_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_13)
//				{
//						cobra_map_pinmodeset(PIN13_INDEX, PIN13_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_14)
//				{
//						cobra_map_pinmodeset(PIN14_INDEX, PIN14_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_15)
//				{
//						cobra_map_pinmodeset(PIN15_INDEX, PIN15_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_16)
//				{
//						cobra_map_pinmodeset(PIN16_INDEX, PIN16_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_17)
//				{
//						cobra_map_pinmodeset(PIN17_INDEX, PIN17_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_18)
//				{
//						cobra_map_pinmodeset(PIN18_INDEX, PIN18_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_19)
//				{
//						cobra_map_pinmodeset(PIN19_INDEX, PIN19_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_20)
//				{
//						cobra_map_pinmodeset(PIN20_INDEX, PIN20_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_21)
//				{
//						cobra_map_pinmodeset(PIN21_INDEX, PIN21_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_22)
//				{
//						cobra_map_pinmodeset(PIN22_INDEX, PIN22_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_23)
//				{
//						cobra_map_pinmodeset(PIN23_INDEX, PIN23_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_24)
//				{
//						cobra_map_pinmodeset(PIN24_INDEX, PIN24_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_25)
//				{
//						cobra_map_pinmodeset(PIN25_INDEX, PIN25_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_26)
//				{
//						cobra_map_pinmodeset(PIN26_INDEX, PIN26_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_27)
//				{
//						cobra_map_pinmodeset(PIN27_INDEX, PIN27_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_28)
//				{
//						cobra_map_pinmodeset(PIN28_INDEX, PIN28_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_29)
//				{
//						cobra_map_pinmodeset(PIN29_INDEX, PIN29_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_30)
//				{
//						cobra_map_pinmodeset(PIN30_INDEX, PIN30_IO_OUTPUT_0);
//				}
//				else if(pin == GPIO_PIN_31)
//				{
//						cobra_map_pinmodeset(PIN31_INDEX, PIN31_IO_OUTPUT_0);
//				}
		}
	
}

void cobra_m4_gpio_set_output(gpio_pin pin)
{
	      cobra_pin_input_enable((PINMUX_INDEX)pin, 0);	
	
//			  PINMUX_INDEX pin_index;
//	
//			  if(pin == GPIO_PIN_0)
//				{
//						pin_index = PIN0_INDEX;
//				}
//				else if(pin == GPIO_PIN_1)
//				{
//						pin_index = PIN1_INDEX;
//				}
//				else if(pin == GPIO_PIN_2)
//				{
//						pin_index = PIN2_INDEX;
//				}
//				else if(pin == GPIO_PIN_3)
//				{
//						pin_index = PIN3_INDEX;
//				}
//				else if(pin == GPIO_PIN_4)
//				{
//						pin_index = PIN4_INDEX;
//				}
//				else if(pin == GPIO_PIN_5)
//				{
//						pin_index = PIN5_INDEX;
//				}
//				else if(pin == GPIO_PIN_6)
//				{
//						pin_index = PIN6_INDEX;
//				}
//				else if(pin == GPIO_PIN_7)
//				{
//						pin_index = PIN7_INDEX;
//				}
//				else if(pin == GPIO_PIN_8)
//				{
//						pin_index = PIN8_INDEX;
//				}
//				else if(pin == GPIO_PIN_9)
//				{
//						pin_index = PIN9_INDEX;
//				}
//				else if(pin == GPIO_PIN_10)
//				{
//						pin_index = PIN10_INDEX;
//				}
//				else if(pin == GPIO_PIN_11)
//				{
//						pin_index = PIN11_INDEX;
//				}
//				else if(pin == GPIO_PIN_12)
//				{
//						pin_index = PIN12_INDEX;
//				}
//				else if(pin == GPIO_PIN_13)
//				{
//						pin_index = PIN13_INDEX;;
//				}
//				else if(pin == GPIO_PIN_14)
//				{
//						pin_index = PIN14_INDEX;
//				}
//				else if(pin == GPIO_PIN_15)
//				{
//						pin_index = PIN15_INDEX;
//				}
//				else if(pin == GPIO_PIN_16)
//				{
//						pin_index = PIN16_INDEX;
//				}
//				else if(pin == GPIO_PIN_17)
//				{
//						pin_index = PIN17_INDEX;
//				}
//				else if(pin == GPIO_PIN_18)
//				{
//						pin_index = PIN18_INDEX;
//				}
//				else if(pin == GPIO_PIN_19)
//				{
//						pin_index = PIN19_INDEX;
//				}
//				else if(pin == GPIO_PIN_20)
//				{
//						pin_index = PIN20_INDEX;
//				}
//				else if(pin == GPIO_PIN_21)
//				{
//						pin_index = PIN21_INDEX;
//				}
//				else if(pin == GPIO_PIN_22)
//				{
//						pin_index = PIN22_INDEX;
//				}
//				else if(pin == GPIO_PIN_23)
//				{
//						pin_index = PIN23_INDEX;
//				}
//				else if(pin == GPIO_PIN_24)
//				{
//						pin_index = PIN24_INDEX;
//				}
//				else if(pin == GPIO_PIN_25)
//				{
//						pin_index = PIN25_INDEX;
//				}
//				else if(pin == GPIO_PIN_26)
//				{
//						pin_index = PIN26_INDEX;
//				}
//				else if(pin == GPIO_PIN_27)
//				{
//						pin_index = PIN27_INDEX;
//				}
//				else if(pin == GPIO_PIN_28)
//				{
//						pin_index = PIN28_INDEX;
//				}
//				else if(pin == GPIO_PIN_29)
//				{
//						pin_index = PIN29_INDEX;
//				}
//				else if(pin == GPIO_PIN_30)
//				{
//						pin_index = PIN30_INDEX;
//				}
//				else if(pin == GPIO_PIN_31)
//				{
//						pin_index = PIN31_INDEX;
//				}
//	
//			  cobra_pin_input_enable(pin_index, 0);	
}

void cobra_m4_gpio_set_input(gpio_pin pin)
{
	     cobra_map_pinmodeset((PINMUX_INDEX)pin,PIN_MODE_13);
	     cobra_pin_input_enable((PINMUX_INDEX)pin, 1);	
	
//			  PINMUX_INDEX pin_index;
//	
//			  if(pin == GPIO_PIN_0)
//				{
//						pin_index = PIN0_INDEX;
//				}
//				else if(pin == GPIO_PIN_1)
//				{
//						pin_index = PIN1_INDEX;
//				}
//				else if(pin == GPIO_PIN_2)
//				{
//						pin_index = PIN2_INDEX;
//				}
//				else if(pin == GPIO_PIN_3)
//				{
//						pin_index = PIN3_INDEX;
//				}
//				else if(pin == GPIO_PIN_4)
//				{
//						pin_index = PIN4_INDEX;
//				}
//				else if(pin == GPIO_PIN_5)
//				{
//						pin_index = PIN5_INDEX;
//				}
//				else if(pin == GPIO_PIN_6)
//				{
//						pin_index = PIN6_INDEX;
//				}
//				else if(pin == GPIO_PIN_7)
//				{
//						pin_index = PIN7_INDEX;
//				}
//				else if(pin == GPIO_PIN_8)
//				{
//						pin_index = PIN8_INDEX;
//				}
//				else if(pin == GPIO_PIN_9)
//				{
//						pin_index = PIN9_INDEX;
//				}
//				else if(pin == GPIO_PIN_10)
//				{
//						pin_index = PIN10_INDEX;
//				}
//				else if(pin == GPIO_PIN_11)
//				{
//						pin_index = PIN11_INDEX;
//				}
//				else if(pin == GPIO_PIN_12)
//				{
//						pin_index = PIN12_INDEX;
//				}
//				else if(pin == GPIO_PIN_13)
//				{
//						pin_index = PIN13_INDEX;;
//				}
//				else if(pin == GPIO_PIN_14)
//				{
//						pin_index = PIN14_INDEX;
//				}
//				else if(pin == GPIO_PIN_15)
//				{
//						pin_index = PIN15_INDEX;
//				}
//				else if(pin == GPIO_PIN_16)
//				{
//						pin_index = PIN16_INDEX;
//				}
//				else if(pin == GPIO_PIN_17)
//				{
//						pin_index = PIN17_INDEX;
//				}
//				else if(pin == GPIO_PIN_18)
//				{
//						pin_index = PIN18_INDEX;
//				}
//				else if(pin == GPIO_PIN_19)
//				{
//						pin_index = PIN19_INDEX;
//				}
//				else if(pin == GPIO_PIN_20)
//				{
//						pin_index = PIN20_INDEX;
//				}
//				else if(pin == GPIO_PIN_21)
//				{
//						pin_index = PIN21_INDEX;
//				}
//				else if(pin == GPIO_PIN_22)
//				{
//						pin_index = PIN22_INDEX;
//				}
//				else if(pin == GPIO_PIN_23)
//				{
//						pin_index = PIN23_INDEX;
//				}
//				else if(pin == GPIO_PIN_24)
//				{
//						pin_index = PIN24_INDEX;
//				}
//				else if(pin == GPIO_PIN_25)
//				{
//						pin_index = PIN25_INDEX;
//				}
//				else if(pin == GPIO_PIN_26)
//				{
//						pin_index = PIN26_INDEX;
//				}
//				else if(pin == GPIO_PIN_27)
//				{
//						pin_index = PIN27_INDEX;
//				}
//				else if(pin == GPIO_PIN_28)
//				{
//						pin_index = PIN28_INDEX;
//				}
//				else if(pin == GPIO_PIN_29)
//				{
//						pin_index = PIN29_INDEX;
//				}
//				else if(pin == GPIO_PIN_30)
//				{
//						pin_index = PIN30_INDEX;
//				}
//				else if(pin == GPIO_PIN_31)
//				{
//						pin_index = PIN31_INDEX;
//				}
////lianbo
//              cobra_map_pinmodeset(pin_index,PIN_MODE_13);
//                
//			  cobra_pin_input_enable(pin_index, 1);	
}

/*

 ÉèÖÃÎª¸ß×è×´Ì¬
*/
void cobra_m4_gpio_set_high_z(gpio_pin pin)
{
   cobra_map_pinmodeset((PINMUX_INDEX)pin, PIN_MODE_13);
	
//    if(pin == GPIO_PIN_0)
//	{
//		cobra_map_pinmodeset(PIN0_INDEX, PIN0_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_1)
//	{
//			
//		cobra_map_pinmodeset(PIN1_INDEX, PIN1_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_2)
//	{
//		cobra_map_pinmodeset(PIN2_INDEX, PIN2_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_3)
//	{
//		cobra_map_pinmodeset(PIN3_INDEX, PIN3_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_4)
//	{
//		cobra_map_pinmodeset(PIN4_INDEX, PIN4_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_5)
//	{
//		cobra_map_pinmodeset(PIN5_INDEX, PIN5_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_6)
//	{
//		cobra_map_pinmodeset(PIN6_INDEX, PIN6_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_7)
//	{
//		cobra_map_pinmodeset(PIN7_INDEX, PIN7_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_8)
//	{
//		cobra_map_pinmodeset(PIN8_INDEX, PIN8_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_9)
//	{
//		cobra_map_pinmodeset(PIN9_INDEX, PIN9_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_10)
//	{
//		cobra_map_pinmodeset(PIN10_INDEX, PIN10_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_11)
//	{
//		cobra_map_pinmodeset(PIN11_INDEX, PIN11_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_12)
//	{
//		cobra_map_pinmodeset(PIN12_INDEX, PIN12_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_13)
//	{
//		cobra_map_pinmodeset(PIN13_INDEX, PIN13_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_14)
//	{
//		cobra_map_pinmodeset(PIN14_INDEX, PIN14_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_15)
//	{
//		cobra_map_pinmodeset(PIN15_INDEX, PIN15_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_16)
//	{
//		cobra_map_pinmodeset(PIN16_INDEX, PIN16_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_17)
//	{
//		cobra_map_pinmodeset(PIN17_INDEX, PIN17_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_18)
//	{
//		cobra_map_pinmodeset(PIN18_INDEX, PIN18_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_19)
//	{
//		cobra_map_pinmodeset(PIN19_INDEX, PIN19_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_20)
//	{
//		cobra_map_pinmodeset(PIN20_INDEX, PIN20_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_21)
//	{
//		cobra_map_pinmodeset(PIN21_INDEX, PIN21_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_22)
//	{
//		cobra_map_pinmodeset(PIN22_INDEX, PIN22_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_23)
//	{
//		cobra_map_pinmodeset(PIN23_INDEX, PIN23_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_24)
//	{
//		cobra_map_pinmodeset(PIN24_INDEX, PIN24_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_25)
//	{
//		cobra_map_pinmodeset(PIN25_INDEX, PIN25_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_26)
//	{
//		cobra_map_pinmodeset(PIN26_INDEX, PIN26_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_27)
//	{
//		cobra_map_pinmodeset(PIN27_INDEX, PIN27_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_28)
//	{
//		cobra_map_pinmodeset(PIN28_INDEX, PIN28_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_29)
//	{
//		cobra_map_pinmodeset(PIN29_INDEX, PIN29_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_30)
//	{
//		cobra_map_pinmodeset(PIN30_INDEX, PIN30_IO_HIGH_Z_STATE);
//	}
//	else if(pin == GPIO_PIN_31)
//	{
//		cobra_map_pinmodeset(PIN31_INDEX, PIN31_IO_HIGH_Z_STATE);
//	}
	
 
	
}

void cobra_m4_gpio_interrupt_enable(gpio_pin pin)
{
	      cobra_pin_level_detect_enable((PINMUX_INDEX)pin , 1);	
	
//		    PINMUX_INDEX pin_index;
//	
//			  if(pin == GPIO_PIN_0)
//				{
//						pin_index = PIN0_INDEX;
//				}
//				else if(pin == GPIO_PIN_1)
//				{
//						pin_index = PIN1_INDEX;
//				}
//				else if(pin == GPIO_PIN_2)
//				{
//						pin_index = PIN2_INDEX;
//				}
//				else if(pin == GPIO_PIN_3)
//				{
//						pin_index = PIN3_INDEX;
//				}
//				else if(pin == GPIO_PIN_4)
//				{
//						pin_index = PIN4_INDEX;
//				}
//				else if(pin == GPIO_PIN_5)
//				{
//						pin_index = PIN5_INDEX;
//				}
//				else if(pin == GPIO_PIN_6)
//				{
//						pin_index = PIN6_INDEX;
//				}
//				else if(pin == GPIO_PIN_7)
//				{
//						pin_index = PIN7_INDEX;
//				}
//				else if(pin == GPIO_PIN_8)
//				{
//						pin_index = PIN8_INDEX;
//				}
//				else if(pin == GPIO_PIN_9)
//				{
//						pin_index = PIN9_INDEX;
//				}
//				else if(pin == GPIO_PIN_10)
//				{
//						pin_index = PIN10_INDEX;
//				}
//				else if(pin == GPIO_PIN_11)
//				{
//						pin_index = PIN11_INDEX;
//				}
//				else if(pin == GPIO_PIN_12)
//				{
//						pin_index = PIN12_INDEX;
//				}
//				else if(pin == GPIO_PIN_13)
//				{
//						pin_index = PIN13_INDEX;;
//				}
//				else if(pin == GPIO_PIN_14)
//				{
//						pin_index = PIN14_INDEX;
//				}
//				else if(pin == GPIO_PIN_15)
//				{
//						pin_index = PIN15_INDEX;
//				}
//				else if(pin == GPIO_PIN_16)
//				{
//						pin_index = PIN16_INDEX;
//				}
//				else if(pin == GPIO_PIN_17)
//				{
//						pin_index = PIN17_INDEX;
//				}
//				else if(pin == GPIO_PIN_18)
//				{
//						pin_index = PIN18_INDEX;
//				}
//				else if(pin == GPIO_PIN_19)
//				{
//						pin_index = PIN19_INDEX;
//				}
//				else if(pin == GPIO_PIN_20)
//				{
//						pin_index = PIN20_INDEX;
//				}
//				else if(pin == GPIO_PIN_21)
//				{
//						pin_index = PIN21_INDEX;
//				}
//				else if(pin == GPIO_PIN_22)
//				{
//						pin_index = PIN22_INDEX;
//				}
//				else if(pin == GPIO_PIN_23)
//				{
//						pin_index = PIN23_INDEX;
//				}
//				else if(pin == GPIO_PIN_24)
//				{
//						pin_index = PIN24_INDEX;
//				}
//				else if(pin == GPIO_PIN_25)
//				{
//						pin_index = PIN25_INDEX;
//				}
//				else if(pin == GPIO_PIN_26)
//				{
//						pin_index = PIN26_INDEX;
//				}
//				else if(pin == GPIO_PIN_27)
//				{
//						pin_index = PIN27_INDEX;
//				}
//				else if(pin == GPIO_PIN_28)
//				{
//						pin_index = PIN28_INDEX;
//				}
//				else if(pin == GPIO_PIN_29)
//				{
//						pin_index = PIN29_INDEX;
//				}
//				else if(pin == GPIO_PIN_30)
//				{
//						pin_index = PIN30_INDEX;
//				}
//				else if(pin == GPIO_PIN_31)
//				{
//						pin_index = PIN31_INDEX;
//				}
//				cobra_pin_level_detect_enable(pin_index , 1);	
}

void cobra_m4_gpio_interrupt_disable(gpio_pin pin)
{
	     cobra_pin_level_detect_enable((PINMUX_INDEX)pin , 0);	
	
//			  PINMUX_INDEX pin_index;
//	
//			  if(pin == GPIO_PIN_0)
//				{
//						pin_index = PIN0_INDEX;
//				}
//				else if(pin == GPIO_PIN_1)
//				{
//						pin_index = PIN1_INDEX;
//				}
//				else if(pin == GPIO_PIN_2)
//				{
//						pin_index = PIN2_INDEX;
//				}
//				else if(pin == GPIO_PIN_3)
//				{
//						pin_index = PIN3_INDEX;
//				}
//				else if(pin == GPIO_PIN_4)
//				{
//						pin_index = PIN4_INDEX;
//				}
//				else if(pin == GPIO_PIN_5)
//				{
//						pin_index = PIN5_INDEX;
//				}
//				else if(pin == GPIO_PIN_6)
//				{
//						pin_index = PIN6_INDEX;
//				}
//				else if(pin == GPIO_PIN_7)
//				{
//						pin_index = PIN7_INDEX;
//				}
//				else if(pin == GPIO_PIN_8)
//				{
//						pin_index = PIN8_INDEX;
//				}
//				else if(pin == GPIO_PIN_9)
//				{
//						pin_index = PIN9_INDEX;
//				}
//				else if(pin == GPIO_PIN_10)
//				{
//						pin_index = PIN10_INDEX;
//				}
//				else if(pin == GPIO_PIN_11)
//				{
//						pin_index = PIN11_INDEX;
//				}
//				else if(pin == GPIO_PIN_12)
//				{
//						pin_index = PIN12_INDEX;
//				}
//				else if(pin == GPIO_PIN_13)
//				{
//						pin_index = PIN13_INDEX;;
//				}
//				else if(pin == GPIO_PIN_14)
//				{
//						pin_index = PIN14_INDEX;
//				}
//				else if(pin == GPIO_PIN_15)
//				{
//						pin_index = PIN15_INDEX;
//				}
//				else if(pin == GPIO_PIN_16)
//				{
//						pin_index = PIN16_INDEX;
//				}
//				else if(pin == GPIO_PIN_17)
//				{
//						pin_index = PIN17_INDEX;
//				}
//				else if(pin == GPIO_PIN_18)
//				{
//						pin_index = PIN18_INDEX;
//				}
//				else if(pin == GPIO_PIN_19)
//				{
//						pin_index = PIN19_INDEX;
//				}
//				else if(pin == GPIO_PIN_20)
//				{
//						pin_index = PIN20_INDEX;
//				}
//				else if(pin == GPIO_PIN_21)
//				{
//						pin_index = PIN21_INDEX;
//				}
//				else if(pin == GPIO_PIN_22)
//				{
//						pin_index = PIN22_INDEX;
//				}
//				else if(pin == GPIO_PIN_23)
//				{
//						pin_index = PIN23_INDEX;
//				}
//				else if(pin == GPIO_PIN_24)
//				{
//						pin_index = PIN24_INDEX;
//				}
//				else if(pin == GPIO_PIN_25)
//				{
//						pin_index = PIN25_INDEX;
//				}
//				else if(pin == GPIO_PIN_26)
//				{
//						pin_index = PIN26_INDEX;
//				}
//				else if(pin == GPIO_PIN_27)
//				{
//						pin_index = PIN27_INDEX;
//				}
//				else if(pin == GPIO_PIN_28)
//				{
//						pin_index = PIN28_INDEX;
//				}
//				else if(pin == GPIO_PIN_29)
//				{
//						pin_index = PIN29_INDEX;
//				}
//				else if(pin == GPIO_PIN_30)
//				{
//						pin_index = PIN30_INDEX;
//				}
//				else if(pin == GPIO_PIN_31)
//				{
//						pin_index = PIN31_INDEX;
//				}
//				cobra_pin_level_detect_enable(pin_index , 0);	
		
}

void cobra_m4_gpio_interrupt_leve(gpio_pin pin, unsigned int level)
{
	      cobra_pin_level_detect((PINMUX_INDEX)pin , level);	
	
//				PINMUX_INDEX pin_index;
//	
//			  if(pin == GPIO_PIN_0)
//				{
//						pin_index = PIN0_INDEX;
//				}
//				else if(pin == GPIO_PIN_1)
//				{
//						pin_index = PIN1_INDEX;
//				}
//				else if(pin == GPIO_PIN_2)
//				{
//						pin_index = PIN2_INDEX;
//				}
//				else if(pin == GPIO_PIN_3)
//				{
//						pin_index = PIN3_INDEX;
//				}
//				else if(pin == GPIO_PIN_4)
//				{
//						pin_index = PIN4_INDEX;
//				}
//				else if(pin == GPIO_PIN_5)
//				{
//						pin_index = PIN5_INDEX;
//				}
//				else if(pin == GPIO_PIN_6)
//				{
//						pin_index = PIN6_INDEX;
//				}
//				else if(pin == GPIO_PIN_7)
//				{
//						pin_index = PIN7_INDEX;
//				}
//				else if(pin == GPIO_PIN_8)
//				{
//						pin_index = PIN8_INDEX;
//				}
//				else if(pin == GPIO_PIN_9)
//				{
//						pin_index = PIN9_INDEX;
//				}
//				else if(pin == GPIO_PIN_10)
//				{
//						pin_index = PIN10_INDEX;
//				}
//				else if(pin == GPIO_PIN_11)
//				{
//						pin_index = PIN11_INDEX;
//				}
//				else if(pin == GPIO_PIN_12)
//				{
//						pin_index = PIN12_INDEX;
//				}
//				else if(pin == GPIO_PIN_13)
//				{
//						pin_index = PIN13_INDEX;;
//				}
//				else if(pin == GPIO_PIN_14)
//				{
//						pin_index = PIN14_INDEX;
//				}
//				else if(pin == GPIO_PIN_15)
//				{
//						pin_index = PIN15_INDEX;
//				}
//				else if(pin == GPIO_PIN_16)
//				{
//						pin_index = PIN16_INDEX;
//				}
//				else if(pin == GPIO_PIN_17)
//				{
//						pin_index = PIN17_INDEX;
//				}
//				else if(pin == GPIO_PIN_18)
//				{
//						pin_index = PIN18_INDEX;
//				}
//				else if(pin == GPIO_PIN_19)
//				{
//						pin_index = PIN19_INDEX;
//				}
//				else if(pin == GPIO_PIN_20)
//				{
//						pin_index = PIN20_INDEX;
//				}
//				else if(pin == GPIO_PIN_21)
//				{
//						pin_index = PIN21_INDEX;
//				}
//				else if(pin == GPIO_PIN_22)
//				{
//						pin_index = PIN22_INDEX;
//				}
//				else if(pin == GPIO_PIN_23)
//				{
//						pin_index = PIN23_INDEX;
//				}
//				else if(pin == GPIO_PIN_24)
//				{
//						pin_index = PIN24_INDEX;
//				}
//				else if(pin == GPIO_PIN_25)
//				{
//						pin_index = PIN25_INDEX;
//				}
//				else if(pin == GPIO_PIN_26)
//				{
//						pin_index = PIN26_INDEX;
//				}
//				else if(pin == GPIO_PIN_27)
//				{
//						pin_index = PIN27_INDEX;
//				}
//				else if(pin == GPIO_PIN_28)
//				{
//						pin_index = PIN28_INDEX;
//				}
//				else if(pin == GPIO_PIN_29)
//				{
//						pin_index = PIN29_INDEX;
//				}
//				else if(pin == GPIO_PIN_30)
//				{
//						pin_index = PIN30_INDEX;
//				}
//				else if(pin == GPIO_PIN_31)
//				{
//						pin_index = PIN31_INDEX;
//				}
//				cobra_pin_level_detect(pin_index , level);	
}


unsigned int cobra_m4_gpio_interrupt_level_get(gpio_pin pin)
{
    return cobra_pin_level_detect_get((PINMUX_INDEX)pin);
}

void cobra_m4_power_off_enable(unsigned int en)
{
		if(en)
		{
				cobra_m4_gpio0->POWER_OFF_CTRL |= 0x01;
		}
		else
		{
				cobra_m4_gpio0->POWER_OFF_CTRL &= ~0x01;
		}
	
}

#endif
