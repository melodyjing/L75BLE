/******************************************************************************

	Cobra4 Drivers

	File Name	:	cobra4_d_vad.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/6

	Description :   cobra4 
					0x400e000
					

	Changed Log	:

		Liujian  2019/8/6			- Creation


*****************************************************************************/

#include "cobra4_d_vad.h"

#if (COBRA_POWER_DOWN_ENABLE)
static volatile unsigned int *cobra4_d_vad_ctrl __attribute__((section("retention_area"),zero_init));
static volatile unsigned int *cobra4_d_vad_event __attribute__((section("retention_area"),zero_init));
#else
static volatile unsigned int * cobra4_d_vad_ctrl;
static volatile unsigned int * cobra4_d_vad_event;
#endif

void cobra4_d_vad_init(void)
{
	 cobra4_d_vad_ctrl = (volatile unsigned int *)COBRA4_MCU_VD_CTRL;
	 cobra4_d_vad_event = (volatile unsigned int *)COBRA4_MCU_EVENT_TO_PPU;
}

/*
Clock Frequency selection. The value is limited by MAX_FREQUENCY parameter.

In case multiple radio interfaces with different clocking requirements are supported, 
it is recommended to tie clk_sel to an external register, 
so that on-the-fly selection of radio interfaces is made possible.

cobra  设置为
MAX_FREQUENCY = 32
*/
void cobra4_d_vad_bt_clock_sel(unsigned int clock_sel)
{
	  *cobra4_d_vad_ctrl &= ~0x3f;
		*cobra4_d_vad_ctrl |= clock_sel&0x3f;
}

/*
if en = 1
把 ICYTRX 的脚　都连接到外部 PAD.

*/
void cobra4_d_vad_icytrx_connect_to_pad(unsigned int en)
{
		if(en)
		{
				*cobra4_d_vad_ctrl |= (1 << 7);
		}
		else
		{
			  *cobra4_d_vad_ctrl &= ~(1 << 7);
		}
}

/*
if en  =1 , 把CEAV 蓝牙　base 基带部分　连接到外部 PAD

*/
void cobra4_d_vad_btbase_connect_to_pad(unsigned int en)
{
		if(en)
		{
				*cobra4_d_vad_ctrl |= (1 << 8);
		}
		else
		{
			  *cobra4_d_vad_ctrl &= ~(1 << 8);
		}
}

/*
cortex m4  发送事件给 PPU

*/
void cobra4_d_vad_send_sigal_to_ppu(unsigned int en)
{
		if(en)
		{
				*cobra4_d_vad_event |= 0x01;
		}
		else
		{
				*cobra4_d_vad_event &= ~0x01;
		}
}





