/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4_aon_wk.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/6

	Description :   
        for cobra4 
		
	Changed Log	:

		Liujian  2019/8/6			- Creation



*****************************************************************************/
#include "cobra4_aon_wk.h"
#if COBRA4_AON_WK_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA4_AON_WK_Type *cobra4_aon_wk  __attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA4_AON_WK_Type * cobra4_aon_wk;
#endif

void cobra4_aon_wk_init(void)
{
		cobra4_aon_wk = (volatile COBRA4_AON_WK_Type *)(COBRA4_AON_WK_BASE);
}

/*
aon_protect

"0: write diable
1: write enable"

*/
void cobra4_aon_wk_protection_en(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_aon_protect = 1;
		}
		else
		{
				cobra4_aon_wk->wk_aon_protect = 0;
		}
}

/*
rcosc32k enable
0: disable, 1: enable

*/
//void cobra4_aon_wk_aon_rcosc32k_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra4_aon_wk->wk_clk_control |= 0x01;
//		}
//		else
//		{
//				cobra4_aon_wk->wk_clk_control &= ~0x01;
//		}
//}




/*
rcosc32k enable config enable
0: diable setting, 1: set aon_rcosc32k_en bit

*/
void cobra4_aon_wk_aon_rcosc32k_wr_en(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_clk_control |= (0x01 << 1);
		}
		else
		{
				cobra4_aon_wk->wk_clk_control &= ~(0x01 << 1);
		}
}

void cobra4_aon_wk_aon_rcosc32k_en(unsigned int en)
{
		unsigned int temp;
		temp = cobra4_aon_wk->wk_clk_control;
		temp |= (0x01 << 1);
		if(en)
		{
				temp |= 0x01;
				cobra4_aon_wk->wk_clk_control = temp;
		}
		else
		{
				temp &= ~0x01;
				cobra4_aon_wk->wk_clk_control = temp;
		}
	//	cobra4_aon_wk_aon_rcosc32k_wr_en(0);		
}


/*
aon_xtal32k_en
xtal32k enable

0: disable, 1: enable

*/
void cobra4_aon_wk_aon_xtal32k_en(unsigned int en)
{
		unsigned int temp;
	  temp = cobra4_aon_wk->wk_clk_control;
		temp |= (0x01 << 3);
		if(en)
		{
				temp |= (0x01 << 2);
				cobra4_aon_wk->wk_clk_control = temp;
		}
		else
		{
				temp &= ~(0x01 << 2);
				cobra4_aon_wk->wk_clk_control = temp;
		}
//		cobra4_aon_wk_aon_xtal32k_en_wr(0);
}	
	
/*
aon_xtal32k_en_wr

xtal32k enable config enable
0: diable setting, 1: set aon_xtal32k_en bit

*/
void cobra4_aon_wk_aon_xtal32k_en_wr(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_clk_control |= (0x01 << 3);
		}
		else
		{
				cobra4_aon_wk->wk_clk_control &= ~(0x01 << 3);
		}
}	

/*
aon_rcosc24m_en
rcosc24m enable
0: disable, 1: enable

*/
void cobra4_aon_wk_aon_rcosc24m_en(unsigned int en)
{
		unsigned int temp;
		temp = cobra4_aon_wk->wk_clk_control;
		temp |= (0x01 << 5);
		if(en)
		{
				temp |= (0x01 << 4);
				cobra4_aon_wk->wk_clk_control = temp;
				//cobra4_aon_wk->wk_clk_control |= (0x01 << 4);
		}
		else
		{
				temp &= ~(0x01 << 4);
				//cobra4_aon_wk->wk_clk_control &= ~(0x01 << 4);
				cobra4_aon_wk->wk_clk_control = temp;
		}
//		cobra4_aon_wk_aon_rcosc24m_en_wr(0);
}

/*
aon_rcosc24m_en_wr

rcosc24m enable config enable

0: diable setting, 1: set aon_rcosc24m_en bit

*/
void cobra4_aon_wk_aon_rcosc24m_en_wr(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_clk_control |= (0x01 << 5);
		}
		else
		{
				cobra4_aon_wk->wk_clk_control &= ~(0x01 << 5);
		}
}

/*
csem 32k clock source selection
0: rcosc32k, 1: xtal32k
aon_csem_clk_cfg

*/
void cobra4_aon_wk_aon_csem_clk_selection(AON_CSEM_CLK_SOURCE selection)
{
		if(selection == CSEM_CLK_RCOSC32K)
		{
				cobra4_aon_wk->wk_clk_control &= ~(1 << 8);
		}
		else if(selection == CSEM_CLK_XTAL32K)
		{
				cobra4_aon_wk->wk_clk_control |= (1 << 8);
		}
	
}


/*
warm system reset 使能控制
0:warm system reset被屏蔽，1：warm system reset生效
aon_warm_sysrst_en

*/
void cobra4_aon_wk_aon_warm_sysrst_en(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_clk_control |= (1 << 16);
		}
		else
		{
			 cobra4_aon_wk->wk_clk_control &= ~(1 << 16);
		}
	
}


/*
soft system reset 使能控制

0:soft system reset被屏蔽，1：soft system reset生效
aon_soft_sysrst_en


*/
void cobra4_aon_wk_aon_soft_sysrst_en(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_clk_control |= (1 << 17);
		}
		else
		{
			 cobra4_aon_wk->wk_clk_control &= ~(1 << 17);
		}
	
}


/*
pd1 por reset 使能控制

0:pd1 por reset被屏蔽，1：pd1 por reset生效

pd1_por_sysrst_en

*/
void cobra4_aon_wk_pd1_por_sysrst_en(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_clk_control |= (1 << 18);
		}
		else
		{
			 cobra4_aon_wk->wk_clk_control &= ~(1 << 18);
		}
	
}

/*
aon_pd关闭时钟请求，配合aon_pmu_mode使用
1： 请求有效

ppu_standby_req

*/
void cobra4_aon_wk_ppu_standby_req(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_ppu_power_control |= 0x01;
		}
		else
		{
				cobra4_aon_wk->wk_ppu_power_control &= ~0x01;
		}
}


/*
aon_pd下电/关闭时钟请求,请求完成自清除。配合aon_pmu_mode使用

1： 请求有效
ppu_shutdwn_req


*/
void cobra4_aon_wk_ppu_shutdwn_req(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_ppu_power_control |= (0x01 << 4);
		}
		else
		{
				cobra4_aon_wk->wk_ppu_power_control &= ~(0x01 << 4);
		}
}

/*
aon pmu模式

01： standby 模式，10： shutdown模式，其他正常工作模式
aon_pmu_mode[0]
aon_pmu_mode[1]

*/
void cobra4_aon_wk_aon_pmu_mode(AON_PMU_MODE mode)
{
		volatile unsigned int temp;
	
		temp = cobra4_aon_wk->wk_ppu_power_control;
		temp &= ~(0x03 << 8);
		if(mode == AON_PMU_STANDBY)
		{
			temp |= 1;
			cobra4_aon_wk->wk_ppu_power_control = temp;
		}
		else if(mode == AON_PMU_SHUTDOWN)
		{
			temp |= 2;
			cobra4_aon_wk->wk_ppu_power_control = temp;		
		}
}

/*

iso enable for mcu_vd interface
mcu_iso

*/
void cobra4_aon_wk_mcu_iso_en(unsigned int en)
{
	if(en)
	{
			cobra4_aon_wk->wk_ppu_power_control |= 1<<16;		
	}
	else
	{
			cobra4_aon_wk->wk_ppu_power_control &= ~(1<<16);		
	}
	
}

/*
iso enable for icytrx interface
icytrx_iso

*/
void cobra4_aon_wk_icytrx_iso_en(unsigned int en)
{
	if(en)
	{
			cobra4_aon_wk->wk_ppu_power_control |= 1<<17;		
	}
	else
	{
			cobra4_aon_wk->wk_ppu_power_control &= ~(1<<17);		
	}
	
}
/*

resetinfo[0]   1: 表示当前全局复位源为pin or power up 复位
resetinfo[1]   1: 表示当前全局复位源为m4 lockup 复位
resetinfo[2]   1: 表示当前全局复位源为wdt0 复位
resetinfo[3]   1: 表示当前全局复位源为mcu_pd por 复位
*/

unsigned int cobra4_aon_wk_m4_reset_information(void)
{
		volatile unsigned int temp;
    temp = 	cobra4_aon_wk->wk_ppu_power_control;
	  temp >>= 24;
	  temp &= 0x0f;
	  return temp;	
}



/*
1: 表示当前全局复位源为pin or power up 复位

*/
unsigned int cobra4_aon_wk_m4_pin_or_power_up_reset(void)
{
		if(cobra4_aon_wk->wk_ppu_power_control & (1<<24))
				return 1;
		else
				return 0;
}

/*
1: 表示当前全局复位源为m4 lockup 复位


*/
unsigned int cobra4_aon_wk_m4_system_lockup_reset(void)
{
		if(cobra4_aon_wk->wk_ppu_power_control & (1<<25))
				return 1;
		else
				return 0;
}

/*
1: 表示当前全局复位源为soft 复位


*/
unsigned int cobra4_aon_wk_m4_system_soft_reset(void)
{
		if(cobra4_aon_wk->wk_ppu_power_control & (1<<26))
				return 1;
		else
				return 0;
}

/*
1: 表示当前全局复位源为mcu_pd por 复位

*/
unsigned int cobra4_aon_wk_m4_system_mcu_pd_por_reset(void)
{
		if(cobra4_aon_wk->wk_ppu_power_control & (1<<27))
				return 1;
		else
				return 0;
}


/*

ram的retention_ldo 电源开关使能

*/
void cobra4_aon_wk_retention_ldo_control(unsigned int retention_ldo_value)
{
		volatile unsigned int temp;
	
		temp = cobra4_aon_wk->wk_ram_control;
		temp &= ~0x1ff;
		temp |= retention_ldo_value&0x1ff;
		cobra4_aon_wk->wk_ram_control = temp;
}



/*

ram的ldo 电源开关使能


*/
void cobra4_aon_wk_ram_ldo_control(unsigned int retention_ldo_value)
{
		volatile unsigned int temp;
	
		temp = cobra4_aon_wk->wk_ram_control;
		temp &= ~(0x1ff << 16);
		temp |= ((retention_ldo_value&0x1ff) << 16);
		cobra4_aon_wk->wk_ram_control = temp;
}

/*
flash_pdm25强制有效，1：pdm25保持为有效，0：由flash控制器信号及mask控制

*/
//void cobra4_aon_wk_flash_pdm25_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra4_aon_wk->wk_aon_control |= (1<<30);
//		}
//		else
//		{
//			 cobra4_aon_wk->wk_aon_control &= ~(1<<30);
//		}
//	
//}

/*
flash控制器信号mask，1：由flash控制器信号控制,0:屏蔽flash控制器信号

这两bit可以合作完成flash_pdm2使能

*/
//void cobra4_aon_wk_flash_pdm25_mask_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra4_aon_wk->wk_aon_control |= (1<<31);
//		}
//		else
//		{
//			 cobra4_aon_wk->wk_aon_control &= ~(1<<31);
//		}
//	
//}

/*
上电序列： 
      ldo_pd0使能并稳定后（使能后等待5-10us）
      ldo_perm_sw =1，ldo_pd0_sw = 0；
      ldo_perm_sw =1，ldo_pd0_sw =1； 
      ldo_perm_sw =0，ldo_pd0_sw =1；
下电序列： 
      ldo_perm_sw =0，ldo_pd0_sw =1；
      ldo_perm_sw =1，ldo_pd0_sw =1； 
      ldo_perm_sw =1，ldo_pd0_sw =0； 
*/

/*
自动模式下的pd0电源选择, 1: 选ldo_dig,0:选ldo_perm

pd0_sw

*/
void cobra4_aon_wk_pd0_sw_selection(PD0_LDO_TYPE type)
{
	if(type == PD0_LDO_DIG)
	{
		cobra4_aon_wk->wk_aon_power_down_control |= 0x01;
	}
	else if(type == PD0_LDO_PERM)
	{
		cobra4_aon_wk->wk_aon_power_down_control &= ~(0x01);
	}
	
}

/*
pd0电源选择控制模式，0：手动模式，1：自动模式

*/
void cobra4_aon_wk_pd0_sw_mode(PD0_POWER_MODE mode)
{
		if(mode == PD0_POWER_MANUAL)
		{
				cobra4_aon_wk->wk_aon_power_down_control &= ~(1<<8);
		}
		else if(mode == PD0_POWER_AUTO)
		{
				cobra4_aon_wk->wk_aon_power_down_control |= (1<<8);
		}
}

/*

ldo_perm手动使能，1：使能

*/
void cobra4_aon_wk_pd0_sw_perm_manu_enable(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_aon_power_down_control |= (1<<16);
		}
		else
		{
				cobra4_aon_wk->wk_aon_power_down_control &= ~(1<<16);
		}
}

/*
ldo_dig手动使能，1：使能

*/
void cobra4_aon_wk_pd0_sw_ldo0_manu_enable(unsigned int en)
{
		if(en)
		{
				cobra4_aon_wk->wk_aon_power_down_control |= (1<<17);
		}
		else
		{
				cobra4_aon_wk->wk_aon_power_down_control &= ~(1<<17);
		}
}

/*
自动模式下电delay周期数

*/
void cobra4_aon_wk_pd0_sw_power_down_delay_value(unsigned int delay)
{
		volatile unsigned int temp;
		temp = cobra4_aon_wk->wk_aon_power_down_control;
		temp &= ~(0x0f << 24);
		temp |= ((delay&0x0f) << 24);
		cobra4_aon_wk->wk_aon_power_down_control = temp;
}

/*
自动模式上电delay周期数

*/
void cobra4_aon_wk_pd0_sw_power_up_delay_value(unsigned int delay)
{
		volatile unsigned int temp;
		temp = cobra4_aon_wk->wk_aon_power_down_control;
		temp &= ~(0x0f << 28);
		temp |= ((delay&0x0f) << 28);
		cobra4_aon_wk->wk_aon_power_down_control = temp;
}

/*
ldodig_pd1 power switch for pd1	
ldoret power switch for pd1	low
input pad mux swtich for comp A	
input pad mux swtich for comp A	
input pad mux swtich for comp A	
input pad mux swtich for comp A	
input pad mux swtich for comp A	
input pad mux swtich for comp A	
input pad mux swtich for comp A	
input pad mux swtich for comp B	
input pad mux swtich for comp B	
input pad mux swtich for comp B	
input pad mux swtich for comp B	
input pad mux swtich for comp B	
input pad mux swtich for comp B	
input pad mux swtich for gpadc	
input pad mux swtich for gpadc	
input pad mux swtich for gpadc	
enable aon oscs test mux	
aon oscs test mux sel	
aon oscs test mux sel	


analog vdd3 enable	
ldo_ret enable, active low	

used by digital 	
used by digital 	
used by digital 	

*/
void cobra4_aon_wk_d2a_ldoanaforpll_en(unsigned int en)
{
	if (en)
	{
		cobra4_aon_wk->wk_aon_control_high |= (1 << 10);
	}
	else
	{
		cobra4_aon_wk->wk_aon_control_high &= ~(1 << 10);
	}
}

//apll_isolate0使能，0：端口锁定， 1：端口使能
void cobra4_aon_wk_apll_isolate0_en(unsigned int en)
{
	if (en)
	{
		cobra4_aon_wk->wk_aon_control_high |= (1 << 11);
	}
	else
	{
		cobra4_aon_wk->wk_aon_control_high &= ~(1 << 11);
	}
}

//cpa_isolate0使能，0：端口锁定， 1：端口使能
void cobra4_aon_wk_cpa_isolate0_en(unsigned int en)
{
	if (en)
	{
		cobra4_aon_wk->wk_aon_control_high |= (1 << 12);
	}
	else
	{
		cobra4_aon_wk->wk_aon_control_high &= ~(1 << 12);
	}
}

//cpb_isolate0使能，0：端口锁定， 1：端口使能
void cobra4_aon_wk_cpb_isolate0_en(unsigned int en)
{
	if (en)
	{
		cobra4_aon_wk->wk_aon_control_high |= (1 << 13);
	}
	else
	{
		cobra4_aon_wk->wk_aon_control_high &= ~(1 << 13);
	}
}

//flash_pdm25强制有效，1：pdm25保持为有效，0：由flash控制器信号及mask控制
void cobra4_aon_wk_flash_pdm25_en(unsigned int en)
{
	if (en)
	{
		cobra4_aon_wk->wk_aon_control_high |= (1 << 14);
	}
	else
	{
		cobra4_aon_wk->wk_aon_control_high &= ~(1 << 14);
	}
}

//flash控制器信号mask，1：由flash控制器信号控制,0:屏蔽flash控制器信号
void cobra4_aon_wk_flash_ctrl_mask_en(unsigned int en)
{
	if (en)
	{
		cobra4_aon_wk->wk_aon_control_high |= (1 << 15);
	}
	else
	{
		cobra4_aon_wk->wk_aon_control_high &= ~(1 << 15);
	}
}

#endif