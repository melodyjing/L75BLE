/*
 * sysctl.c
 *
 *  Created on: 2017?5?10?
 *      Author: lichun
 */

#include <stdio.h>
#include <string.h>
#include "sysctrl.h"
//#include "interrupt.h"
#include "reg_access.h"
#include "arch.h"
#if 0
void rfm0_remap(bool en)
{
	REG_PL_WR(RFM0_REMAP_REG,en);
}

void rfm0_lockup_reset_en(bool en)
{
	REG_PL_WR(RFM0_LOCKUP_RESET_REG,en);
}

uint32_t read_rfm0_reset_info(void)
{
	return REG_PL_RD(RFM0_RESET_INFO_REG);
}

void rfm0_clk_set(char value)
{
	REG_PL_WR(RFM0_BT_CTRL_REG,(REG_PL_RD(RFM0_BT_CTRL_REG)&(~RFM0_BT_CLK_MASK))|value);
}

void rfm0_wakeup_set(bool en)
{
	REG_PL_WR(RFM0_BT_CTRL_REG,(REG_PL_RD(RFM0_BT_CTRL_REG)&(~RFM0_BT_CLK_MASK))|(en<<RFM0_BT_WAKUP_POS));
}


//2018/7/23
//move from main.c

void NMI_HandlerC(unsigned long *nmi_args)
{
	printf("NMI_H:%lx %lx %lx\n",nmi_args[0],nmi_args[1],nmi_args[2]);	
}

void HardFault_HandlerC(unsigned long *hardfault_args)
{
	//printf("HardFault_H: %lx %lx %lx\n",hardfault_args[0],hardfault_args[1],hardfault_args[2]);
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

__asm void reboot_jump(uint32_t sp,uint32_t pc)
{
	msr 	msp, r0
	msr 	psp, r0
	bx 		r1
}
#endif


/// @}
