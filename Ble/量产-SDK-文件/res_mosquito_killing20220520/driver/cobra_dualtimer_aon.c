/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_dualtimer_aon.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/10

	Description :   
       Aon 上总线的双时钟
			 因为 Aon  是在Always 总线上， 它的中断是共用的， 目前我们选择的是 AON_EVENT4 上 

	Changed Log	:

		Liujian  2019/4/10 		- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_pcrm.h"
#include "cobra_system.h"
#include "cobra_dualtimer_aon.h"


#if (COBRA_DUALTIMER_AON_EN==1)

//static volatile COBRA_DUALTIMER_BOTH_TypeDef *   cobra_dualtimer;
#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_AON_DUALTIMER_SINGLE_TypeDef * cobra_singletimer1		__attribute__((section("retention_area"),zero_init)); 
static volatile COBRA_AON_DUALTIMER_SINGLE_TypeDef * cobra_singletimer2		__attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_AON_DUALTIMER_SINGLE_TypeDef * cobra_singletimer1;
static volatile COBRA_AON_DUALTIMER_SINGLE_TypeDef * cobra_singletimer2;
#endif

static aon_dual_timer_callback_t p_aon_dualtimer1_callback;

static aon_dual_timer_callback_t p_aon_dualtimer2_callback;


void cobra_aon_dualtimer_init(void)
{
	//	cobra_dualtimer = (COBRA_DUALTIMER_BOTH_TypeDef * )COBRA_DUALTIMER_BASE;
	
		cobra_singletimer1 = (COBRA_AON_DUALTIMER_SINGLE_TypeDef * )COBRA_AON_DUALTIMER_1_BASE;
	
		cobra_singletimer2 = (COBRA_AON_DUALTIMER_SINGLE_TypeDef * )COBRA_AON_DUALTIMER_2_BASE;
	
		cobra_singletimer1->TimerControl = 0;
	
		cobra_singletimer2->TimerControl = 0;
	
	  p_aon_dualtimer1_callback = NULL;
	
	  p_aon_dualtimer2_callback = NULL;
}


/* Start the timer */
static void cobra_aon_dualtimer_start(volatile COBRA_AON_DUALTIMER_SINGLE_TypeDef * single_timer)
{
		single_timer->TimerControl |= CMSDK_DUALTIMER_CTRL_EN_Msk;
}
/* Stop the timer */
static void cobra_aon_dualtimer_stop(volatile COBRA_AON_DUALTIMER_SINGLE_TypeDef *single_timer)
{
		single_timer->TimerControl &= ~CMSDK_DUALTIMER_CTRL_EN_Msk;
}
/* Clear the interrupt request */
static void cobra_aon_dualtimer_irq_clear(volatile COBRA_AON_DUALTIMER_SINGLE_TypeDef *single_timer)
{
		single_timer->TimerIntClr = 0;
}


/*
 *
 * period minal (1/APB2_SYS_CLK) s
 */

static void cobra_aon_dualtimer_set(AON_DUALTIME_INDEX time_index, AON_DUALTIMER_COUNT_SIZE count_size, AON_DUALTIME_MODE mode,  unsigned int cycle, unsigned int prescale, unsigned int interrupt)
{
	unsigned int ctrl_val;
	volatile COBRA_AON_DUALTIMER_SINGLE_TypeDef *p_temp;

//	cobra_pcrm_dualtimer_active_mode_clock_enable(1);
//	cobra_pcrm_dualtimer_sleep_mode_clock_enable(1);
	switch(time_index)
	{
		case AON_DUALTIMER1:
			p_temp = cobra_singletimer1;
			break;
		case AON_DUALTIMER2:
			p_temp = cobra_singletimer2;
			break;
		default:
			break;
	}
	p_temp->TimerControl = 0;
	cobra_aon_dualtimer_irq_clear(p_temp);
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
	 
	 cobra4_aon_pd_top_protection_en(1);
	
		cobra4_aon_pd_top_aon2mcu_source4_selection(AON_DTIMER2);
		cobra4_aon_pd_top_protection_en(0);
	 
	 NVIC_ClearPendingIRQ(AON_EVENT4_IRQn);
	 NVIC_EnableIRQ(AON_EVENT4_IRQn);
	 NVIC_SetPriority(AON_EVENT4_IRQn, 1);
	 
}

void cobra_aon_dualtimer_set_frequency(AON_DUALTIME_INDEX timer_index, float hz)
{
	unsigned int  cycle;
	unsigned int  apb2_sys_clk;
	float temp_cycle;
	float cycle_max1;
	float cycle_max2;
	float cycle_max3;
	
	apb2_sys_clk = cobra_system_get_m3_apb0_clk();

	cycle_max1 = (float)(0xFFFFFFFF);
	cycle_max2 = (float)(0xFFFFFFFFF);
	cycle_max3 = (float)(0xFFFFFFFFFF);

	temp_cycle = (apb2_sys_clk/hz) ;
	

	if(temp_cycle < cycle_max1)
	{
		cycle = (unsigned int)temp_cycle;
		cobra_aon_dualtimer_set(timer_index, AON_COUNT_32BIT, AON_DUALTIME_PERIODIC, cycle, 0, 1);
	}
	else if(temp_cycle < cycle_max2)
	{
		cycle = (unsigned int)(temp_cycle/16);
		cobra_aon_dualtimer_set(timer_index, AON_COUNT_32BIT, AON_DUALTIME_PERIODIC, cycle, 1, 1);
	}
	else if(temp_cycle < cycle_max3)
	{
		cycle = (unsigned int)(temp_cycle/256);
		cobra_aon_dualtimer_set(timer_index, AON_COUNT_32BIT, AON_DUALTIME_PERIODIC, cycle, 2, 1);
	}

}

void cobra_aon_dualtimer_interrupt(void)
{

	 if (cobra_singletimer1->TimerMIS != 0)
	 {
		 
		  cobra_aon_dualtimer_irq_clear(cobra_singletimer1);
		 
		  if(p_aon_dualtimer1_callback != NULL)
		  {
			    p_aon_dualtimer1_callback();
		  }
			
	 }
	 
	 if (cobra_singletimer2->TimerMIS != 0)
	 {
		//	printf("cobra_dualtimer_interrupt2 \n");
			cobra_aon_dualtimer_irq_clear(cobra_singletimer2);
		 
		  if(p_aon_dualtimer2_callback != NULL)
		  {
			    p_aon_dualtimer2_callback();
		  }
			
	 }

}

void app_cobra_init_aon_dualtimer1(int hz)
{
	  cobra_aon_dualtimer_set_frequency(AON_DUALTIMER1, hz);
}


void app_cobra_init_aon_dualtimer2(int hz)
{
	  cobra_aon_dualtimer_set_frequency(AON_DUALTIMER2, hz);
}


void app_cobra_aon_dualtimer1_set_callback(aon_dual_timer_callback_t func)
{
	   p_aon_dualtimer1_callback = func;
}


void app_cobra_aon_dualtimer2_set_callback(aon_dual_timer_callback_t func)
{
	   p_aon_dualtimer2_callback = func;
}

#endif














