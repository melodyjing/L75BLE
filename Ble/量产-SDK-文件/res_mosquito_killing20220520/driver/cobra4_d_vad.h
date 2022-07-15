/******************************************************************************

	Cobra4 Drivers

	File Name	:	cobra4_d_vad.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/6

	Description :   cobra4 
					0x400e000
					

	Changed Log	:

		Liujian  2019/8/6			- Creation


*****************************************************************************/
#ifndef __COBRA4_D_VAD_H__
#define __COBRA4_D_VAD_H__
#include <stdio.h>
#include <cobra_config.h>
#if COBRA4_D_VAD_EN
/*
Addr: 0x400e0000 + 0x79 * 4
prdata <= {btbb_connect_pad,icytrx_connect_pad,1'b0,bt_clk_sel[5:0]} ;

defparam u_CORTEXM4INTEGRATION_0.MPU_PRESENT = 1 ;
defparam u_CORTEXM4INTEGRATION_0.NUM_IRQ = 41 ;
defparam u_CORTEXM4INTEGRATION_0.CLKGATE_PRESENT = 1 ;
defparam u_CORTEXM4INTEGRATION_0.RESET_ALL_REGS = 1 ;
//defparam u_CORTEXM4INTEGRATION_0.OBSERVATION = 1 ;
defparam u_CORTEXM4INTEGRATION_0.WIC_PRESENT = 1 ;
defparam u_CORTEXM4INTEGRATION_0.WIC_LINES = 6 ;
defparam u_CORTEXM4INTEGRATION_0.FPU_PRESENT = 0 ;
defparam u_CORTEXM4INTEGRATION_0.DEBUG_LVL = 1 ;
defparam u_CORTEXM4INTEGRATION_0.TRACE_LVL = 0 ;
*/

#define COBRA4_MCU_VD_CTRL				(0X400E0000 + 0x79*4)
#define COBRA4_MCU_EVENT_TO_PPU		(0X400E0000 + 0x80*4)

void cobra4_d_vad_init(void);

/*
if en  =1 , 把CEAV 蓝牙　base 基带部分　连接到外部 PAD

*/
void cobra4_d_vad_btbase_connect_to_pad(unsigned int en);

/*
if en = 1
把 ICYTRX 的脚　都连接到外部 PAD.

*/
void cobra4_d_vad_icytrx_connect_to_pad(unsigned int en);

/*
Clock Frequency selection. The value is limited by MAX_FREQUENCY parameter.

In case multiple radio interfaces with different clocking requirements are supported, 
it is recommended to tie clk_sel to an external register, 
so that on-the-fly selection of radio interfaces is made possible.

cobra  设置为
MAX_FREQUENCY = 32
*/
void cobra4_d_vad_bt_clock_sel(unsigned int clock_sel);

void cobra4_d_vad_send_sigal_to_ppu(unsigned int en);

#endif
#endif

