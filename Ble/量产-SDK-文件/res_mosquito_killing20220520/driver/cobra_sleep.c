/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_sleep.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/3/4

	Description :   


	Changed Log	:

		Liujian  2021/3/4			- Creation

*****************************************************************************/

#include "cobra_sleep.h"
#include "cobra_system.h"
#include "rwble.h"

#if (APP_SLEEP_MODE_ENABLE == 1)
// 程序检测射频模块是否开始工作, 然后设置 umuc_int 中断, 唤醒 M4,
// PPU 同时释放权限, 让 M4 控制总线
static uint16_t ppu_sleep[] = {
0x6c02,
0x4000,
0x004c,
0x0a0c,
0x6a06,
0x4004,
0x004a,
0x008a,
0x0a0c,
0x8c00,
0xc000,
0x8081,
0x440e,
0x400c,
0x4000,
	
};
void cobra_sleep_init(void)
{
	   cobra4_aon_pd_top_protection_en(1);
		 cobra4_aon_pd_top_ucpu_pclk_en(1);
		 cobra4_aon_pd_top_umcu_srst_req_en(1);
		 cobra4_aon_pd_top_protection_en(0);
	   
	   app_cobra_umcu_clk_enable(1);
		 app_cobra4_umcu_enable_sram();
	
	   cobra_umcu_clear_intr_wevent();    //清中断
		 cobra_umcu_clear_intr_done();
	
	   cobra4_aon_pd_top_protection_en(1);
		 cobra4_aon_pd_top_aon2mcu_source0_selection(AON_UMCU_INT);
		 cobra4_aon_pd_top_protection_en(0);
	   
	   cobra_umcu_copy_program(ppu_sleep, sizeof(ppu_sleep)/sizeof(uint16_t));
	
	   NVIC_SetPriority(AON_EVENT0_IRQn, 1);
		 NVIC_ClearPendingIRQ(AON_EVENT0_IRQn);
		 NVIC_EnableIRQ(AON_EVENT0_IRQn);	 
}

void cobra_sleep_enter(void)
{
  	 SCB->SCR |= 0x04; 
		 app_cobra_umcu_run();	
}

void cobra_sleep_exit(void)
{
	  rwip_prevent_sleep_set(RW_WAKE_UP_ONGOING);
	  SCB->SCR &= ~0x04; 
}



void cobra_aon_event0_interrupt(void)
{
	//    rwble_sleep_wakeup_end();
	
	    cobra_umcu_clear_intr_wevent();    //清中断
		  cobra_umcu_clear_intr_done();
	    
	
}





#endif

