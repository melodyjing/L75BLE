/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_cortex_m4_nvic.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/5

	Description :   
					

	Changed Log	:

		Liujian  2019/8/5 		- Creation


*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_system.h"
#include "cobra_watchdog.h"
#include "cobra_cortex_m4_nvic.h"
#include "cobra_aon_pmu.h"
#include "cobra4_aon_top.h"
//2019/8/5
//move from main.c
#if MCU_RUN_SYSTEM == MCU_M4

void NMI_HandlerC(unsigned long *nmi_args)
{
//		 	cobra_aon_pmu_protection_en(1);
//			cobra_aon_pmu_aon_hf_clk_cfg(HF_CLK_RCOSC8M);
//			cobra_aon_pmu_protection_en(0);		
			cobra4_aon_pd_top_protection_en(1);
			cobra4_aon_pd_top_hf_clock_selection(HF_RC_OSC24M);
			cobra4_aon_pd_top_protection_en(0);
			while(1);
}

void HardFault_HandlerC(unsigned long *hardfault_args)
{
	//  printf("HardFault_H: %lx %lx %lx\n",hardfault_args[0],hardfault_args[1],hardfault_args[2]);
		printf("r0 = %lx \n", hardfault_args[0]);
		printf("r1 = %lx \n", hardfault_args[1]);
	  printf("r2 = %lx \n", hardfault_args[2]);
	  printf("r3 = %lx \n", hardfault_args[3]);
	  printf("r12 = %lx \n", hardfault_args[4]);
	  printf("lr = %lx \n", hardfault_args[5]);
	  printf("pc = %lx \n", hardfault_args[6]);
	  printf("psr = %lx \n", hardfault_args[7]);

	
	while(1);
}

void SVC_HandlerC(void)
{
			printf("SVC_H\n");
}

void PendSV_HandlerC(void)
{
			printf("PendSV_H\n");
}

void MemoryManagement_HandlerC(void)
{
			printf("Memory H\n");	
}	

void BusFault_HandlerC(void)
{
			printf("BusFault_H\n");	
}	

void UsageFault_HandlerC(void)
{
			printf("UsageFault_H\n");	
}	

void DebugMon_HandlerC(void)
{
			printf("DebugMon_H\n");	
}	




/**
  * @brief  Sets the vector table location and Offset.
  * @param  NVIC_VectTab: specifies if the vector table is in RAM or FLASH memory.
  *   This parameter can be one of the following values:
  *     @arg NVIC_VectTab_RAM
  *     @arg NVIC_VectTab_FLASH
  * @param  Offset: Vector Table base offset field. This value must be a multiple 
  *         of 0x200.
  * @retval None
  */
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{ 
  /* Check the parameters */
//  assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
//  assert_param(IS_NVIC_OFFSET(Offset));  
   
  SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}


void NVIC_SetDeepSleep(void)
{
		SCB->SCR |= 0x04;
}


void NVIC_Systick_enable(unsigned int en)
{
		if(en)
		{
				SysTick->CTRL |= 0x01;
		}
		else
		{
				SysTick->CTRL &= ~0x01;
		}
}


void NVIC_Systick_set_value(unsigned int value)
{
		SysTick->VAL = value&0x00ffffff;
}

unsigned int NVIC_Systick_get_current_value(void)
{
		return SysTick->VAL ;		
}

void NVIC_Systick_system_clock_enable(unsigned int en)
{
		if(en)
		{
				SysTick->CTRL |= 0x04;
		}
		else
		{
				SysTick->CTRL &= ~0x04;
		}
}

void NVIC_Systick_interrupt_enable(unsigned int en)
{
		if(en)
		{
				SysTick->CTRL |= 0x02;
		}
		else
		{
				SysTick->CTRL &= ~0x02;
		}
}

#endif









