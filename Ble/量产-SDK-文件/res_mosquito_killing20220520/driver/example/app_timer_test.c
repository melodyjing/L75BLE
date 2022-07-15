/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
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
#include "app_timer_test.h"


#if COBRA_DUALTIMER_EN

//static volatile COBRA_DUALTIMER_BOTH_TypeDef *   cobra_dualtimer;

static volatile COBRA_DUALTIMER_SINGLE_TypeDef * cobra_singletimer1;

static volatile COBRA_DUALTIMER_SINGLE_TypeDef * cobra_singletimer2;

static volatile COBRA_DUALTIMER_SINGLE_TypeDef * cobra_dualtime20singletimer1;

static volatile COBRA_DUALTIMER_SINGLE_TypeDef * cobra_dualtime20singletimer2;


void cobra_dualtimer_init(void)
{
	//	cobra_dualtimer = (COBRA_DUALTIMER_BOTH_TypeDef * )COBRA_DUALTIMER_BASE;
	
		cobra_singletimer1 = (COBRA_DUALTIMER_SINGLE_TypeDef * )COBRA_DUALTIMER_1_BASE;
	
		cobra_singletimer2 = (COBRA_DUALTIMER_SINGLE_TypeDef * )COBRA_DUALTIMER_2_BASE;
    
		cobra_dualtime20singletimer1 = (COBRA_DUALTIMER_SINGLE_TypeDef * )COBRA_DUALTIMER20_1_BASE;
	
		cobra_dualtime20singletimer2 = (COBRA_DUALTIMER_SINGLE_TypeDef * )COBRA_DUALTIMER20_2_BASE;    
	
		cobra_singletimer1->TimerControl = 0;
	
		cobra_singletimer2->TimerControl = 0;
    
    cobra_dualtime20singletimer1->TimerControl = 0;
	
		cobra_dualtime20singletimer2->TimerControl = 0;
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
		case DUALTIMER20_1:
			p_temp = cobra_dualtime20singletimer1;
	    break;
		case DUALTIMER20_2:
			p_temp = cobra_dualtime20singletimer2;
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
	 printf("ctrl_val %x %x\n",ctrl_val,cycle);
	 switch(time_index)
     {
         case DUALTIMER1:
         case DUALTIMER2:   
            	 NVIC_SetPriority(DUALTIMER2_IRQn, 1);
                 NVIC_ClearPendingIRQ(DUALTIMER2_IRQn);
                 NVIC_EnableIRQ(DUALTIMER2_IRQn);
              break;
         case DUALTIMER20_1:
         case DUALTIMER20_2:
                 NVIC_SetPriority(AON_EVENT4_IRQn, 1);
                 NVIC_ClearPendingIRQ(AON_EVENT4_IRQn);
                 NVIC_EnableIRQ(AON_EVENT4_IRQn);
         default:
			break;
     }

	 
}

void cobra_dualtimer_set_frequency(DUALTIME_INDEX timer_index, float hz)
{
	unsigned int  cycle;
	unsigned int  apb2_sys_clk;
	float temp_cycle;
	float cycle_max1;
	float cycle_max2;
	float cycle_max3;
    switch(timer_index)
    {
         case DUALTIMER1:
         case DUALTIMER2:   
                apb2_sys_clk = cobra_system_get_m3_apb2_clk();
                break;
         case DUALTIMER20_1:
         case DUALTIMER20_2:
					      cobra4_aon_pd_top_aon2mcu_source4_selection(AON_DTIMER2);
                apb2_sys_clk = cobra_system_get_m3_apb0_clk();
                break;
         default:
			break;
     }
//	apb2_sys_clk = cobra_system_get_m3_apb2_clk();
   printf("DUALTIMER20_1 %x %x\n",cobra_system_get_m3_apb0_clk(),cobra_system_get_m3_apb2_clk());
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
			printf("cobra_dualtimer_interrupt1 \n");
			cobra_dualtimer_irq_clear(cobra_singletimer1);
	 }
	 
	 if (cobra_singletimer2->TimerMIS != 0)
	 {
			printf("cobra_dualtimer_interrupt2 \n");
			cobra_dualtimer_irq_clear(cobra_singletimer2);
	 }

}

void cobra_aon_event4_interrupt(void)
{
	 if (cobra_dualtime20singletimer1->TimerMIS != 0)
	 {
			printf("cobra_dualtimer20_interrupt1 \n");
			cobra_dualtimer_irq_clear(cobra_dualtime20singletimer1);
	 }
	 
	 if (cobra_dualtime20singletimer2->TimerMIS != 0)
	 {
			printf("cobra_dualtimer20_interrupt2 \n");
			cobra_dualtimer_irq_clear(cobra_dualtime20singletimer2);
	 }
}



int timer_test (uint8_t timer_type,uint32_t time_tick) {
    if(timer_type == 1)
    { 
        app_cobra_init_timer1(cobra_system_get_m3_apb2_clk()/time_tick);  
        app_cobra_timer1_enable(1);
    }else
    {
		cobra_pcrm_timer0_active_mode_clock_enable(1);
		cobra_pcrm_timer0_sleep_mode_clock_enable(1);
		
		cobra_timer_set_frequency(APB2_TIMER0, cobra_system_get_m3_apb2_clk()/time_tick);    //100hz  timer0   10ms
		cobra_timer_enable_set(APB2_TIMER0, 1);        //Enable timer0
		cobra_timer_interrupt_set(APB2_TIMER0, 1) ;    //Enale interrupt
		
		NVIC_SetPriority(COBRA4_TIMER0_IRQn, 1);
		NVIC_ClearPendingIRQ(COBRA4_TIMER0_IRQn);
		NVIC_EnableIRQ(COBRA4_TIMER0_IRQn);  
    }
    return 0;
}
void cobra_timers_test(void)  //  Çå¹Ø±ÕCOBRA_SOFT_TIMER_EN£¬ÓÐ³åÍ»
{
	static uint8_t cishu=0;
	  if(cishu == 0)
		{
//	  printf("cobra_dualtimer_test \n");
    cobra_dualtimer_set_frequency(DUALTIMER1,1);
    cobra_dualtimer_set_frequency(DUALTIMER2,0.5);
		
    cobra_dualtimer_set_frequency(DUALTIMER20_1,1);
    cobra_dualtimer_set_frequency(DUALTIMER20_2,1);	
			
	  timer_test(1,cobra_system_get_m3_apb2_clk());
	  timer_test(0,cobra_system_get_m3_apb2_clk()/2);
		cishu = 1;	
		}
}
#endif














