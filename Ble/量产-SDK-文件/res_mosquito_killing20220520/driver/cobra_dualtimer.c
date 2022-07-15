/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_dualtimer.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/10

	Description :   


	Changed Log	:

		Liujian  2019/4/10 		- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_pcrm.h"
#include "cobra_system.h"
#include "cobra_dualtimer.h"


#if (COBRA_DUALTIMER_EN==1 && COBRA_APP_SIXTIMER_ENABLE == 0)

//static volatile COBRA_DUALTIMER_BOTH_TypeDef *   cobra_dualtimer;
#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_DUALTIMER_SINGLE_TypeDef * cobra_singletimer1		__attribute__((section("retention_area"),zero_init)); 
static volatile COBRA_DUALTIMER_SINGLE_TypeDef * cobra_singletimer2		__attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_DUALTIMER_SINGLE_TypeDef * cobra_singletimer1;
static volatile COBRA_DUALTIMER_SINGLE_TypeDef * cobra_singletimer2;
#endif

static dual_timer_callback_t p_dualtimer1_callback; 

static dual_timer_callback_t p_dualtimer2_callback; 


void cobra_dualtimer_init(void)
{
	//	cobra_dualtimer = (COBRA_DUALTIMER_BOTH_TypeDef * )COBRA_DUALTIMER_BASE;
	
		cobra_singletimer1 = (COBRA_DUALTIMER_SINGLE_TypeDef * )COBRA_DUALTIMER_1_BASE;
	
		cobra_singletimer2 = (COBRA_DUALTIMER_SINGLE_TypeDef * )COBRA_DUALTIMER_2_BASE;
	
		cobra_singletimer1->TimerControl = 0;
	
		cobra_singletimer2->TimerControl = 0;
}


/* Start the timer */
void cobra_dualtimer_start(volatile COBRA_DUALTIMER_SINGLE_TypeDef * single_timer)
{
		single_timer->TimerControl |= CMSDK_DUALTIMER_CTRL_EN_Msk;
}
/* Stop the timer */
void cobra_dualtimer_stop(volatile COBRA_DUALTIMER_SINGLE_TypeDef *single_timer)
{
		single_timer->TimerControl &= ~CMSDK_DUALTIMER_CTRL_EN_Msk;
}
/* Clear the interrupt request */
void cobra_dualtimer_irq_clear(volatile COBRA_DUALTIMER_SINGLE_TypeDef *single_timer)
{
		single_timer->TimerIntClr = 0;
}






/*
 *
 * period minal (1/APB2_SYS_CLK) s
 */

void cobra_dualtimer_set(DUALTIME_INDEX time_index, DUALTIMER_COUNT_SIZE count_size, DUALTIME_MODE mode,  unsigned int cycle, unsigned int prescale, unsigned int interrupt)
{
	unsigned int ctrl_val;
	volatile COBRA_DUALTIMER_SINGLE_TypeDef *p_temp;

	cobra_pcrm_dualtimer_active_mode_clock_enable(1);
	cobra_pcrm_dualtimer_sleep_mode_clock_enable(1);
	switch(time_index)
	{
		case DUALTIMER1:
			p_temp = cobra_singletimer1;
			break;
		case DUALTIMER2:
			p_temp = cobra_singletimer2;
			break;
		default:
			break;
	}
	p_temp->TimerControl = 0;
	cobra_dualtimer_irq_clear(p_temp);
	p_temp->TimerLoad = cycle;

	ctrl_val = (prescale  & 0x3) << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos |
	             (interrupt & 0x1) << CMSDK_DUALTIMER_CTRL_INTEN_Pos |
	            CMSDK_DUALTIMER_CTRL_EN_Msk;

	 if(count_size == COUNT_32BIT)
	 {
		 ctrl_val |= CMSDK_DUALTIMER_CTRL_SIZE_Msk;
	 }

	 if(mode == DUALTIME_PERIODIC)
	 {
		 ctrl_val |= CMSDK_DUALTIMER_CTRL_MODE_Msk;
	 }
	 else if(mode == DUALTIME_ONE_SHOT)
	 {
		 ctrl_val |= CMSDK_DUALTIMER_CTRL_MODE_Msk|CMSDK_DUALTIMER_CTRL_ONESHOOT_Msk;
	 }
	 p_temp->TimerControl = ctrl_val;
	 p_temp->TimerLoad = cycle;
	 
	 NVIC_SetPriority(DUALTIMER2_IRQn, 1);
	 NVIC_ClearPendingIRQ(DUALTIMER2_IRQn);
	 NVIC_EnableIRQ(DUALTIMER2_IRQn);
	 
}

void cobra_dualtimer_set_frequency(DUALTIME_INDEX timer_index, float hz)
{
	unsigned int  cycle;
	unsigned int  apb2_sys_clk;
	float temp_cycle;
	float cycle_max1;
	float cycle_max2;
	float cycle_max3;
	
	apb2_sys_clk = cobra_system_get_m3_apb2_clk();

	cycle_max1 = (float)(0xFFFFFFFF);
	cycle_max2 = (float)(0xFFFFFFFFF);
	cycle_max3 = (float)(0xFFFFFFFFFF);

	temp_cycle = (apb2_sys_clk/hz) ;
	

	if(temp_cycle < cycle_max1)
	{
		cycle = (unsigned int)temp_cycle;
		cobra_dualtimer_set(timer_index, COUNT_32BIT, DUALTIME_PERIODIC, cycle, 0, 1);
	}
	else if(temp_cycle < cycle_max2)
	{
		cycle = (unsigned int)(temp_cycle/16);
		cobra_dualtimer_set(timer_index, COUNT_32BIT, DUALTIME_PERIODIC, cycle, 1, 1);
	}
	else if(temp_cycle < cycle_max3)
	{
		cycle = (unsigned int)(temp_cycle/256);
		cobra_dualtimer_set(timer_index, COUNT_32BIT, DUALTIME_PERIODIC, cycle, 2, 1);
	}

}

void cobra_dualtimer_interrupt(void)
{

	 if (cobra_singletimer1->TimerMIS != 0)
	 {
		//	printf("cobra_dualtimer_interrupt1 \n");
			cobra_dualtimer_irq_clear(cobra_singletimer1);
		  if(p_dualtimer1_callback != NULL)
			{
				 p_dualtimer1_callback();
			}
	 }
	 
	 if (cobra_singletimer2->TimerMIS != 0)
	 {
		//	printf("cobra_dualtimer_interrupt2 \n");
			cobra_dualtimer_irq_clear(cobra_singletimer2);
		  if(p_dualtimer2_callback != NULL)
			{
				 p_dualtimer2_callback();
			}
	 }

}


void app_cobra_init_dualtimer1(int hz)
{
	 cobra_dualtimer_set_frequency(DUALTIMER1, hz);
}


void app_cobra_init_dualtimer2(int hz)
{
	 cobra_dualtimer_set_frequency(DUALTIMER2, hz);
}

void app_cobra_dualtimer1_set_callback(dual_timer_callback_t func)
{
	   p_dualtimer1_callback = func;
}

void app_cobra_dualtimer2_set_callback(dual_timer_callback_t func)
{
	   p_dualtimer2_callback = func;
}


#if (APP_RAINBOW_ENABLE == 1)

void cobra_stop_dualtimer1(void)
{
		cobra_dualtimer_stop(cobra_singletimer1);
	  cobra_dualtimer_irq_clear(cobra_singletimer1);
}

void cobra_stop_dualtimer2(void)
{
		cobra_dualtimer_stop(cobra_singletimer2);
	  cobra_dualtimer_irq_clear(cobra_singletimer2);
}

uint32_t cobra_dualtimer_get_value(DUALTIME_INDEX time_index)
{
	uint32_t temp;
	float temp_clk;

		switch(time_index)
	{
		case DUALTIMER1:
				temp_clk = cobra_singletimer1->TimerLoad - cobra_singletimer1->TimerValue;
		   // printf("clk1 value = %d\n",temp_clk);
	      temp = cobra_system_get_m3_apb2_clk();
		    temp_clk = temp_clk/temp*1000000;
		
			break;
		case DUALTIMER2:
				temp_clk = cobra_singletimer2->TimerLoad - cobra_singletimer2->TimerValue;
	     	//printf("clk2 value = %d\n",temp_clk);
	      temp = cobra_system_get_m3_apb2_clk();
		    temp_clk = temp_clk/temp*1000000;
			break;
		default:
			break;
	}
//	return temp_clk;
	return (temp_clk+10);	
}


uint32_t cobra_dualtimer_set_value(DUALTIME_INDEX time_index)
{
	switch(time_index)
	{
		case DUALTIMER1:
				cobra_singletimer1->TimerLoad = 0xFFFFFFFF;		
			break;
			
		case DUALTIMER2:
			cobra_singletimer2->TimerLoad = 0xFFFFFFFF;	
			break;
			
		default:
			break;
	}	
}

#endif


#endif














