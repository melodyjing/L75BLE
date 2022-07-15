/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_m0_sysctrl.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/27

		Description :   from cobra digital document
	
		if (reg_addr[11:5] == 7'h00) begin
         case(reg_addr[4:2])
          3'b000: read_mux_le ={{31{1'b0}}, reg_remap} ;     // m0 remap
          3'b001: read_mux_le ={{31{1'b0}}, reg_pmuenable} ; //no use
          3'b010: read_mux_le ={{31{1'b0}}, reg_lockupreset} ; // Enable reset if lockup

          // reset reason of the last time
          // reg_resetinfo[0] : SYSRESETREQ;
          // reg_resetinfo[1] : WDOGRESETREQ;
          // reg_resetinfo[2] : (reg_lockupreset & LOCKUP);
          3'b100: read_mux_le ={{29{1'b0}}, reg_resetinfo[2:0]} ;
          3'b101: read_mux_le ={{22{1'b0}}, btbb_connect_pad,reg_deflt_dma_rstn,reg_icytrx_connect_pad, reg_bt_wakeup_req, reg_bt_clk_sel[5:0]} ;
          3'b110: read_mux_le ={{31{1'b0}}, reg_rfm0_bus_pwr_req} ;
          3'b011,3'b111: read_mux_le = {32{1'b0}};
          default: read_mux_le = {32{1'bx}};
         endcase
      end
			
	Changed Log	:

		Liujian  2019/3/27			- Creation


*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_m0_sysctrl.h"


#if COBRA_M0_SYSCTRL_EN

static volatile COBRA_M0_SYSCTRL_T * cobra_m0_sysctrl;

void cobra_m0_sysctrl_init(void)
{
		cobra_m0_sysctrl = (COBRA_M0_SYSCTRL_T *)COBRA_M0_SYSCTRL_BASE; 
}



/*
if en = 1
 RAM_BANK0  is at address 0x0000

if en = 0
	M0_ROM    is at address 0x0000

|start_address			|			m0_remap = 1     |    m0_remap = 0	 |
|--------------------------------------------------------------|
|0x00							  |			RAM_BANK0				 |		M0_ROM				 |
|--------------------------------------------------------------|
|0x0010_0000        |     VIMS_ROM         |    VIMS_ROM       |
|--------------------------------------------------------------|
|0x0020_0000        |     VIMS_RAM         |    VIMS_RAM       |
|--------------------------------------------------------------|
|0x0030_0000        |     RAM_BANK1        |    RAM_BANK1      |
|--------------------------------------------------------------|
|0x0040_0000        |     M0_ROM           |    M0_ROM         |
|--------------------------------------------------------------|
|.....                                                         |
|--------------------------------------------------------------|
|0x0080_0000        |    eflash            |    eflash         |
|--------------------------------------------------------------|
|0x0084_0000        |    spiFlash          |    spiFlash       |
|--------------------------------------------------------------|
|0x2000_0000        |    RAM_BANK0         |    RAM_BANK0      |
|--------------------------------------------------------------|
|0x2001_0000        |    RAM_BANK1         |    RAM_BANK1      |
|--------------------------------------------------------------|
|0x2002_0000        |    RAM_BANK2         |    RAM_BANK2      |
|--------------------------------------------------------------|
|0x2003_0000        |    RAM_BANK3         |    RAM_BANK3      |
|--------------------------------------------------------------|
|............................................................. |
|--------------------------------------------------------------|
|0x2008_0000        |    M0_ROM            |     M0_ROM        |
|--------------------------------------------------------------|
|...............                                               |
|--------------------------------------------------------------|
|0x4008_1800        |    RMT_RAM           |     RMT_RAM       |
|--------------------------------------------------------------|
|..............................                                |
|--------------------------------------------------------------|
|0x4010_4000        |     BT_EM            |     BT_EM         |
|--------------------------------------------------------------|

*/
void cobra_m0_sysctrl_remap_enable(unsigned int en)
{
		if(en)
		{
				cobra_m0_sysctrl->m0_remap = 0x01;
		}
		else
		{
				cobra_m0_sysctrl->m0_remap = 0x00;
		}
}


//Enable m0 reset if lockup

void cobra_m0_sysctrl_lockup_reset(unsigned int en)
{
		if(en)
		{
				cobra_m0_sysctrl->lockup_reset = 0x01;
		}
		else
		{
				cobra_m0_sysctrl->lockup_reset = 0x00;
		}
}


/*
// reset reason of the last time
// reg_resetinfo[0] : SYSRESETREQ;
// reg_resetinfo[1] : WDOGRESETREQ;
// reg_resetinfo[2] : (reg_lockupreset & LOCKUP);
*/
unsigned int cobra_m0_sysctrl_is_sysrset_req(void)
{
		if(cobra_m0_sysctrl->reset_info & 0x01)
				return 1;
		else
				return 0;
	
}

unsigned int cobra_m0_sysctrl_is_watchdog_req(void)
{
		if(cobra_m0_sysctrl->reset_info & 0x02)
				return 1;
		else
				return 0;
	
}

unsigned int cobra_m0_sysctrl_is_lockupreset_req(void)
{
		if(cobra_m0_sysctrl->reset_info & 0x04)
				return 1;
		else
				return 0;
}


/*
if en  =1 , 把CEAV 蓝牙　base 基带部分　连接到外部 PAD

*/
void cobra_m0_sysctrl_btbase_connect_to_pad(unsigned int en)
{
		if(en)
		{
				cobra_m0_sysctrl->bt_control |= (1 << 9);
		}
		else
		{
			  cobra_m0_sysctrl->bt_control &= ~(1 << 9);
		}
}

/*
一些 DMA  上电时，　需要复位一下

*/
void cobra_m0_sysctrl_reset_dma(void)
{
		cobra_m0_sysctrl->bt_control &= ~(1 << 8);
		cobra_m0_sysctrl->bt_control |= (1 << 8);
}


/*
if en = 1
把 ICYTRX 的脚　都连接到外部 PAD.

*/
void cobra_m0_sysctrl_icytrx_connect_to_pad(unsigned int en)
{
		if(en)
		{
				cobra_m0_sysctrl->bt_control |= (1 << 7);
		}
		else
		{
			  cobra_m0_sysctrl->bt_control &= ~(1 << 7);
		}
}

/*
if en  = 1 , wakeup the bluethooth

*/
void cobra_m0_sysctrl_wakeup_the_bt(unsigned int en)
{
		if(en)
		{
				cobra_m0_sysctrl->bt_control |= (1 << 6);
		}
		else
		{
			  cobra_m0_sysctrl->bt_control &= ~(1 << 6);
		}
}

/*
Clock Frequency selection. The value is limited by MAX_FREQUENCY parameter.

In case multiple radio interfaces with different clocking requirements are supported, 
it is recommended to tie clk_sel to an external register, 
so that on-the-fly selection of radio interfaces is made possible.

cobra  设置为
MAX_FREQUENCY = 32
*/
void cobra_m0_sysctrl_bt_clock_sel(unsigned int clock_sel)
{
	  cobra_m0_sysctrl->bt_control &= ~0x3f;
		cobra_m0_sysctrl->bt_control |= clock_sel&0x3f;
}

/*
驱动　MCU_PMU (m3) 的 rfm0_bus_pwr_req  端口

 当 M3 没有电时，　M0 需要用到 M3 的外设时　，需要

先设置这个为１　

*/
void cobra_m0_sysctrl_rfm0_bus_pwr_req(unsigned int en)
{
		if(en)
		{
				cobra_m0_sysctrl->rfm0_bus_pwr_req |= 0x01;
		}
		else
		{
				cobra_m0_sysctrl->rfm0_bus_pwr_req &= ~0x01;
		}
}

#endif














