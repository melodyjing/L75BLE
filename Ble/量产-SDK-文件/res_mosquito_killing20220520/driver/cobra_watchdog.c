/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_watchdog.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/20

	Description :   


	Changed Log	:

		Liujian  2019/5/20			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include "cobra_watchdog.h"
#include "cobra_aon_pmu.h"
#include "cobra_pcrm.h"
#include "cobra_system.h"

#if COBRA_WATCHDOG_EN


#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_WATCHDOG_TypeDef * cobra_watchdog		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_WATCHDOG_TypeDef * cobra_watchdog; 
#endif

void cobra_watchdog_init(void)
{
		cobra_watchdog = (COBRA_WATCHDOG_TypeDef * )COBRA_WATCHDOG_BASE;
}

void cobra_watchdog_lock_enable(unsigned int en)
{
		if(en)
		{
				cobra_watchdog->LOCK = 0;
		}
		else
		{
				cobra_watchdog->LOCK = 0x1ACCE551;
		}
}

//void pppp(void)
//{
//		cobra4_aon_wk_protection_en(1);
//		cobra4_aon_wk_aon_soft_sysrst_en(1);
//		cobra4_aon_wk_protection_en(0);
//	
//	  cobra_pcrm_soft_reset_whole_chip(1);
//	
//}

void app_cobra_watchdog_init(unsigned int ms, WATCHDOG_MODE mode)
{
			unsigned int temp;
		
	    temp = cobra_system_get_m3_apb0_clk();
			temp /= 1000;
			temp *= ms;
	
			cobra4_aon_pd_top_protection_en(1);
    	cobra4_aon_pd_top_watchdog_freq_enable(1);
		  cobra4_aon_pd_top_watchdog_freq_div(0);
	    //cobra4_aon_pd_top_watchdog_freq_enable(0);
	    cobra4_aon_pd_top_protection_en(0);
	
      cobra4_aon_wk_protection_en(1);
			cobra4_aon_wk_aon_warm_sysrst_en(1);
			cobra4_aon_wk_protection_en(0);
	
	    cobra_pcrm_m3_warm_request_en(1);
			cobra_pcrm_whole_chip_warm_request_en(1);
	
			cobra_watchdog_unlock();   //can modified register
			cobra_watchdog->watchdog_LOAD = temp;
			if(mode == WATCHDOG_NO_ACTION)
			{
					cobra_watchdog->watchdog_CTRL = 0x00;
			}
			else if(mode == WATCHDOG_NMI)
			{
					cobra_watchdog->watchdog_CTRL = 0x01;
			}
			else if(mode == WATCHDOG_RESET)
			{
					cobra_watchdog->watchdog_CTRL = 0x03;
			}
			
	    cobra_watchdog_lock();  //can not modified register, locked
			
		
}

/* Update watchdog counter */
void cobra_watchdog_set(unsigned int ms)
{
		unsigned int temp;
		temp = cobra_system_get_m3_apb0_clk();
		temp /= 1000;
		temp *= ms;
	
		cobra_watchdog_lock_enable(0);
		cobra_watchdog->watchdog_LOAD = temp;
		cobra_watchdog_lock_enable(1);
}


void cobra_watchdog_lock(void)
{
		cobra_watchdog->LOCK = 0x00;
}

void cobra_watchdog_unlock(void)
{
		cobra_watchdog->LOCK = 0x1ACCE551;
}

/* Clear watchdog interrupt request */
void cobra_watchdog_irq_clear(void)
{
		cobra_watchdog_unlock();
		cobra_watchdog->INTCLR = 0x01;
		cobra_watchdog_lock(); 
}


void NMI_Handler(void)
{
//	   printf("NMI ....... \n");
//#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#if 0
	{
		extern void app_mosquito_killing_setting_data_write_debug(uint32_t param);
		app_mosquito_killing_setting_data_write_debug(0x55);
		printf("NMI triger...\n");
		app_cobra_watchdog_init(1000, WATCHDOG_RESET);
	}
#else

	   app_cobra_watchdog_init(10, WATCHDOG_RESET);
#endif
	   while(1);
}




#endif














