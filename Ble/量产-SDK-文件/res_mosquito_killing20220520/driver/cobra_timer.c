/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_timer.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/9

	Description :   


	Changed Log	:

		Liujian  2019/4/9			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include <math.h>
#include "cobra_timer.h"
#include "cobra_system.h"
#include "cobra_pcrm.h"
#include "cobra_soft_timer.h"

#if (COBRA_POWER_DOWN_ENABLE)
#include "cobra4_power_down.h"
#endif

#if COBRA_TIMER_EN


#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_TIMER_TypeDef *cobra_apb2_timer0 __attribute__((section("retention_area"),zero_init));

static volatile COBRA_TIMER_TypeDef *cobra_apb2_timer1 __attribute__((section("retention_area"),zero_init));
static timer_callback_t p_timer1_callback __attribute__((section("retention_area"),zero_init)); 
static timer_callback_t p_timer0_callback __attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_TIMER_TypeDef * cobra_apb2_timer0;

static volatile COBRA_TIMER_TypeDef * cobra_apb2_timer1;
static timer_callback_t p_timer1_callback; 

#if (DIANBIAO_MODULE_ENABLE | DWT_MIC_BREAKER_ENABLE | RADAR_MODUL_ENABLE)
static timer_callback_t p_timer0_callback;
#endif

#endif


//static volatile COBRA_TIMER_TypeDef * cobra_apb0_timer1;

//static volatile COBRA_TIMER_TypeDef * cobra_apb10_timer10;

#if APP_MESH_CWHSL_LIGHT_FUNTION
extern uint16_t publish_time_count;
#endif	
#if CHECK_SCAN_FOR_RESTARE
extern uint16_t js_restare;
#endif
void cobra_timer_init(void)
{
		cobra_apb2_timer0 = (COBRA_TIMER_TypeDef * )COBRA_APB2_TIMER0_BASE;
	
		cobra_apb2_timer1 = (COBRA_TIMER_TypeDef * )COBRA_APB2_TIMER1_BASE;
	
//		cobra_apb0_timer1 = (COBRA_TIMER_TypeDef * )COBRA_APB0_TIMER1_BASE;
//	
//		cobra_apb10_timer10 = (COBRA_TIMER_TypeDef * )COBRA_APB10_TIMER10_BASE;
	 p_timer1_callback = NULL;
	
#if (DIANBIAO_MODULE_ENABLE |DWT_MIC_BREAKER_ENABLE)	
	 p_timer0_callback = NULL;
#endif
}



/**
*
* @param *COBRA_TIMER_TypeDef Timer Pointer
* @return none
*
* @brief  Enable the microcontroller timer interrupts.
*/

void rdw_timer_enable_irq(volatile COBRA_TIMER_TypeDef * rdw_timer)
{
		 rdw_timer->CTRL |= (1 << 3);
}


/**
*
* @param *COBRA_TIMER_TypeDef Timer Pointer
* @return none
*
* @brief  Disable the microcontroller timer interrutps.
*/

void rdw_timer_disable_irq(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 rdw_timer->CTRL &= ~(1 << 3);
}

/**
*
* @param *COBRA_TIMER_TypeDef Timer Pointer
* @return none
*
* @brief  Start the Timer.
*/

void rdw_timer_start_timer(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 rdw_timer->CTRL |= 0x01;
}


/**
*
* @param *COBRA_TIMER_TypeDef Timer Pointer
* @return none
*
* @brief  Stop the Timer.
*/

void rdw_timer_stop_timer(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 rdw_timer->CTRL &= ~0x01;
}


/**
*
* @param *RDW_TIMER Timer Pointer
* @return TimerValue
*
* @brief  Returns the current value of the timer.
*/

uint32_t rdw_timer_get_value(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 return rdw_timer->VALUE;
}

/**
*
* @param *RDW_TIMER Timer Pointer
* @param value the value to which the timer is to be set
* @return TimerValue
*
* @brief  Sets the timer to the specified value.
*/

void rdw_timer_set_value(volatile COBRA_TIMER_TypeDef *rdw_timer, uint32_t value)
{
		 rdw_timer->VALUE = value;
}

/**
*
* @param *RDW_TIMER Timer Pointer
* @return TimerReload
*
* @brief  Returns the reload value of the timer. The reload value is the value which the timer is set to after an underflow occurs.
*/

uint32_t rdw_timer_get_reload_value(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 return rdw_timer->RELOAD;
}

/**
*
* @param *RDW_TIMER Timer Pointer
* @param value Value to be loaded
* @return none
*
* @brief  Sets the reload value of the timer to the specified value. The reload value is the value which the timer is set to after an underflow occurs.
*/

void rdw_timer_set_reload_value(volatile COBRA_TIMER_TypeDef *rdw_timer, uint32_t value)
{
		 rdw_timer->RELOAD = value;
}

/**
*
* @param *RDW_TIMER Timer Pointer
* @return none
*
* @brief  Clears the timer IRQ if set.
*/

void rdw_timer_clear_irq(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 rdw_timer->INT_UINON.INTCLEAR = (0x1ul << 0);
}

/**
*
* @param *RDW_TIMER Timer Pointer
* @return none
*
* @brief  Returns the IRQ status of the timer in question.
*/

uint32_t  rdw_timer_get_irq_status(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 return rdw_timer->INT_UINON.INTSTATUS;
}
 
 
void rdw_timer_enable_ctrl(volatile COBRA_TIMER_TypeDef *rdw_timer, unsigned int en)
{
	 if(en)
	 {
		rdw_timer->CTRL |= 0x01;
	 }
	 else
	 {
		 rdw_timer->CTRL &= ~0x01;
	 }
}
 

/**
*
* @param *RDW_TIMER Timer Pointer
* @param reload The value to which the timer is to be set after an underflow has occurred
* @param irq_en Defines whether the timer IRQ is to be enabled
* @return none
*
* @brief  Initialises the timer to use the internal clock and specifies the timer reload value and whether IRQ is enabled or not.
*/

void rdw_timer_init_internal_clock(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 rdw_timer->CTRL = 0;
		 rdw_timer->VALUE = 0;
		 rdw_timer->RELOAD = 0;
}

/**
*
* @param *RDW_TIMER Timer Pointer
* @param reload The value to which the timer is to be set after an underflow has occurred
* @param irq_en Defines whether the timer IRQ is to be enabled
* @return none
*
* @brief  Initialises the timer to use the external clock and specifies the timer reload value and whether IRQ is enabled or not.
*/

void rdw_timer_init_ext_clock(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 rdw_timer->CTRL = 0;
		 rdw_timer->VALUE = 0;
		 rdw_timer->RELOAD = 0;
}

/**
*
* @brief  Initialises the timer to use the internal clock but with an external enable. It also specifies the timer reload value and whether IRQ is enabled or not.
*
* @param *RDW_TIMER Timer Pointer
* @param reload The value to which the timer is to be set after an underflow has occurred
* @param irq_en Defines whether the timer IRQ is to be enabled
* @return none
*
*
*/

void rdw_timer_init_ext_enable(volatile COBRA_TIMER_TypeDef *rdw_timer)
{
		 rdw_timer->CTRL = 0;
		 rdw_timer->VALUE = 0;
		 rdw_timer->RELOAD = 0;
	 
}


void cobra_timer_set_frequency(time_id  timer_index, float hz)
{
	uint32_t reload;
	uint32_t temp_apb_clk;
	float temp;


	if(hz == 0) return;
	
	if(timer_index == APB2_TIMER0)
	{
			temp_apb_clk = cobra_system_get_m3_apb2_clk();
			temp = (float)temp_apb_clk;
			temp /= hz;
			reload = temp;
	
			rdw_timer_set_reload_value(cobra_apb2_timer0, reload);
	}
	else if(timer_index == APB2_TIMER1)
	{
			temp_apb_clk = cobra_system_get_m3_apb2_clk();
			temp = (float)temp_apb_clk;
			temp /= hz;
			reload = temp;
			
			rdw_timer_set_reload_value(cobra_apb2_timer1, reload);
	}
//	else if(timer_index == APB0_TIMER1)   //always on timer1
//	{
//			temp_apb_clk = cobra_system_get_m3_apb0_clk();
//			temp = (float)temp_apb_clk;
//			temp /= hz;
//			reload = temp;
//			
//			rdw_timer_set_reload_value(cobra_apb0_timer1, reload);
//	}
//	else if(timer_index == APB10_TIMER10)
//	{
//			temp_apb_clk = cobra_system_get_m0_apb10_clk();
//			temp = (float)temp_apb_clk;
//			temp /= hz;
//			reload = temp;
//			
//			rdw_timer_set_reload_value(cobra_apb10_timer10, reload);
//	}
	
		
}
void cobra_timer_enable_set(time_id timer_index, unsigned int en)
{

	
	if(timer_index == APB2_TIMER0)
	{
			rdw_timer_enable_ctrl(cobra_apb2_timer0, en);
	}
	else if(timer_index == APB2_TIMER1)
	{
			rdw_timer_enable_ctrl(cobra_apb2_timer1, en);
	}
//	else if(timer_index == APB0_TIMER1)
//	{
//			rdw_timer_enable_ctrl(cobra_apb0_timer1, en);
//	}
//	else if(timer_index == APB10_TIMER10)
//	{
//			rdw_timer_enable_ctrl(cobra_apb10_timer10, en);
//	}
	
	
}
#ifdef COBRA_TIMER_NEW
/**
 *
 * @param *RDW_TIMER Timer Pointer
 * @param reload The value to which the timer is to be set after an underflow has occurred
 * @param irq_en Defines whether the timer IRQ is to be enabled
 * @return none
 *
 * @brief  Initialises the timer to use the internal clock and specifies the timer reload value and whether IRQ is enabled or not.
 */

void cobra_timer_Init_IntClock(uint8_t timer_type, uint32_t reload,
                             uint32_t irq_en)
{
    if(timer_type)
    {
        cobra_apb2_timer1->CTRL = 0;
        cobra_apb2_timer1->VALUE = reload;
        cobra_apb2_timer1->RELOAD = reload;
        if (irq_en!=0)                                                                          /* non zero - enable IRQ */
            cobra_apb2_timer1->CTRL = (0x08 | 0x01);
        else {                                                                                  /* zero - do not enable IRQ */
            cobra_apb2_timer1->CTRL = (0x01);                                       /* enable timer */
        }
    }
    else
    {
        cobra_apb2_timer0->CTRL = 0;
        cobra_apb2_timer0->VALUE = reload;
        cobra_apb2_timer0->RELOAD = reload;
        if (irq_en!=0)                                                                          /* non zero - enable IRQ */
            cobra_apb2_timer0->CTRL = (0x08 | 0x01);
        else {                                                                                  /* zero - do not enable IRQ */
            cobra_apb2_timer0->CTRL = (0x01);                                       /* enable timer */       
        }
    }
}

void cobra_timer_irq_clear(uint8_t timer_type)
{
    if(timer_type)
    {
        rdw_timer_clear_irq(cobra_apb2_timer1);
    }else
    {
        rdw_timer_clear_irq(cobra_apb2_timer0);
    }
}
#endif
void cobra_timer_interrupt_set(time_id timer_index, unsigned int en)
{

	
	if(timer_index == APB2_TIMER0)
	{
		if(en)
		{
			rdw_timer_enable_irq(cobra_apb2_timer0);	
		}
		else
		{
			rdw_timer_disable_irq(cobra_apb2_timer0);
		}
	}
	else if(timer_index == APB2_TIMER1)
	{
		if(en)
		{
			rdw_timer_enable_irq(cobra_apb2_timer1);	
		}
		else
		{
			rdw_timer_disable_irq(cobra_apb2_timer1);
		}
	}

}

void app_cobra_init_timer0_clock(void)
{
#if (!APP_SLEEP_MODE_ENABLE)		
		cobra_pcrm_timer0_active_mode_clock_enable(1);
		cobra_pcrm_timer0_sleep_mode_clock_enable(1);
#endif
	
}


void app_cobra_timer1_set_callback(timer_callback_t func)
{
	   p_timer1_callback = func;
}

#if (DIANBIAO_MODULE_ENABLE | DWT_MIC_BREAKER_ENABLE | RADAR_MODUL_ENABLE)

void app_cobra_timer0_set_callback(timer_callback_t func)
{
	   p_timer0_callback = func;
}

void app_cobra_init_timer0(float hz)
{
	  cobra_pcrm_timer0_active_mode_clock_enable(1);
		cobra_pcrm_timer0_sleep_mode_clock_enable(1);
		
		cobra_timer_set_frequency(APB2_TIMER0, hz);    //
//		cobra_timer_enable_set(APB2_TIMER1, 1);        //Enable timer1
		cobra_timer_interrupt_set(APB2_TIMER0, 1) ;    //Enale timer1 interrupt
		
		NVIC_SetPriority(COBRA4_TIMER0_IRQn, 1);
		NVIC_ClearPendingIRQ(COBRA4_TIMER0_IRQn);
		NVIC_EnableIRQ(COBRA4_TIMER0_IRQn);
	
}
void app_cobra_timer0_enable(unsigned int en)
{
	 if(en)
	 {
		   cobra_timer_enable_set(APB2_TIMER0, 1);  
	 }
	 else
	 {
		   cobra_timer_enable_set(APB2_TIMER0, 0); 
	 }
}

#endif

void app_cobra_init_timer1(int hz)
{
	  cobra_pcrm_timer1_active_mode_clock_enable(1);
		cobra_pcrm_timer1_sleep_mode_clock_enable(1);
		
		cobra_timer_set_frequency(APB2_TIMER1, hz);    //
//		cobra_timer_enable_set(APB2_TIMER1, 1);        //Enable timer1
		cobra_timer_interrupt_set(APB2_TIMER1, 1) ;    //Enale timer1 interrupt
		
		NVIC_SetPriority(COBRA4_TIMER1_IRQn, 1);
		NVIC_ClearPendingIRQ(COBRA4_TIMER1_IRQn);
		NVIC_EnableIRQ(COBRA4_TIMER1_IRQn);
	
}

void app_cobra_timer1_enable(unsigned int en)
{
	 if(en)
	 {
		   cobra_timer_enable_set(APB2_TIMER1, 1);  
	 }
	 else
	 {
		   cobra_timer_enable_set(APB2_TIMER1, 0); 
	 }
}

void cobra_m3_timer0_interrupt(void)
{
		if(rdw_timer_get_irq_status(cobra_apb2_timer0))
	  {
			 // printf("m4 tick1 \n");
				rdw_timer_clear_irq(cobra_apb2_timer0);
			
#if (DIANBIAO_MODULE_ENABLE | RADAR_MODUL_ENABLE)
        if(p_timer0_callback != NULL)
				{
					 p_timer0_callback();
					 return;
				}	
#endif
				
#if (COBRA_POWER_DOWN_ENABLE && LOW_POWER_WAKE_UP_BY_UART)
		     cobra_low_power_uart0_receive_handle();
#else
        #if COBRA_SOFT_TIMER_EN
				  cobra_soft_timer_update();
        #endif		
#endif			
#if APP_MESH_CWHSL_LIGHT_FUNTION
		if(publish_time_count > 0)
        publish_time_count--;
#endif	
#if CHECK_SCAN_FOR_RESTARE
		if(js_restare !=0xFFFF)
    js_restare++;
#endif	  
	  }
		
}

void cobra_m3_timer1_interrupt(void)
{	  
		if(rdw_timer_get_irq_status(cobra_apb2_timer1))
	  {
				rdw_timer_clear_irq(cobra_apb2_timer1);
			  if(p_timer1_callback != NULL)
				{
					 p_timer1_callback();
				}
	  }
}


volatile COBRA_TIMER_TypeDef * cobra_timer_get_apb2_timer0(void)
{
     return (volatile COBRA_TIMER_TypeDef *)cobra_apb2_timer0;
}	


void cobra_soft_delay_ms(unsigned int count)
{
		unsigned int f_apb2_clk;
		
		unsigned int ms_count ;
		
		uint32_t reload;
	
	  uint32_t i ;
		
		cobra_pcrm_timer1_active_mode_clock_enable(1);
	
		f_apb2_clk = cobra_system_get_m3_apb2_clk();
	
	  rdw_timer_stop_timer(cobra_apb2_timer1);

		ms_count = f_apb2_clk/1000;

		for(i = 0; i < count; i++)
		{
				rdw_timer_disable_irq(cobra_apb2_timer1);

			  rdw_timer_set_reload_value(cobra_apb2_timer1, ms_count);  //1ms
			
			  rdw_timer_start_timer(cobra_apb2_timer1);

				while(rdw_timer_get_value(cobra_apb2_timer1) > 10);
			
				rdw_timer_stop_timer(cobra_apb2_timer1);
		}
			
}

/*
软件等待 us,  使用的是外设 TIMER1 ,如果客户使用到了外设 Timer1 ,那么 需要
在使用完这个函数后, 要自己重新配置 Timer1

*/
void cobra_soft_delay_us(unsigned int count)
{
	unsigned int f_apb2_clk;

	unsigned int ms_count;

	uint32_t reload;

	uint32_t i;

	cobra_pcrm_timer1_active_mode_clock_enable(1);

	f_apb2_clk = cobra_system_get_m3_apb2_clk();

	ms_count = f_apb2_clk / 1000000;
	
	rdw_timer_stop_timer(cobra_apb2_timer1);

	for (i = 0; i < count; i++)
	{
		rdw_timer_disable_irq(cobra_apb2_timer1);
		
    rdw_timer_set_reload_value(cobra_apb2_timer1, ms_count); //1ms
		
		rdw_timer_start_timer(cobra_apb2_timer1);

		while (rdw_timer_get_value(cobra_apb2_timer1) > 10)
			;

		rdw_timer_stop_timer(cobra_apb2_timer1);
	}
}

#endif










