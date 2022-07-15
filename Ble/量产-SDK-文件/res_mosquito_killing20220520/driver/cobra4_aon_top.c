/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4_aon_top.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/6

	Description :   
        for cobra4 , modified power mananger
		APB0 0x4008B000
	Changed Log	:

		Liujian  2019/8/6			- Creation



*****************************************************************************/
#include "cobra4_aon_top.h"

#if COBRA4_AON_PD_TOP_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA4_AON_PD_TOP_T * cobra4_aon_pd_top		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA4_AON_PD_TOP_T * cobra4_aon_pd_top;
#endif

void cobra4_aon_pd_top_init(void)
{
		cobra4_aon_pd_top = ( volatile COBRA4_AON_PD_TOP_T *)COBRA4_AON_PD_TOP_BASE;
}

/*
aon config register write protect enable

"0: write diable
 1: write enable"

*/
void cobra4_aon_pd_top_protection_en(unsigned int en)
{
		if(en)
		{
				cobra4_aon_pd_top->aon_config_protect = 1;
		}
		else
		{
				cobra4_aon_pd_top->aon_config_protect = 0;
		}
}

/*
lf clock source selection
00: rcosc32k, 01:xtal48m, 10: rcosc28m, 11:xtal32k

aon_lf_clk_cfg[0]
aon_lf_clk_cfg[1]

*/
void cobra4_aon_pd_top_lf_clock_selection(AON_LF_CLK_SOURCE source)
{
		volatile unsigned int temp;
		temp = cobra4_aon_pd_top->aon_lf_clk_control;
		if(source == LF_RC_OSC32K)
		{
				cobra4_aon_pd_top->aon_lf_clk_control &= ~0x03;
		}
		else if(source == LF_XTAL48M)
		{
				temp &= ~0x03;
				temp |= 0x01;
				cobra4_aon_pd_top->aon_lf_clk_control = temp;
		}
		else if(source == LF_RC_OSC24M)
		{
				temp &= ~0x03;
				temp |= 0x02;
				cobra4_aon_pd_top->aon_lf_clk_control = temp;
		}
		else if(source == LF_XTAL_32K)
		{
				temp &= ~0x03;
				temp |= 0x03;
				cobra4_aon_pd_top->aon_lf_clk_control = temp;
		}
		
	
}

/*
hf clock source selection
0: rcosc8m, 1: xtal48m

aon_hf_clk_cfg

*/
void cobra4_aon_pd_top_hf_clock_selection(AON_HF_CLK_SOURCE source)
{
		if(source == HF_RC_OSC24M)
		{
				cobra4_aon_pd_top->aon_lf_clk_control &= ~(1 << 4);	
		}
		else if(source == HF_XTAL48M)
		{
			 cobra4_aon_pd_top->aon_lf_clk_control |= (1 << 4);	
		}
	
}

AON_HF_CLK_SOURCE cobra4_aon_pd_top_hf_clock_source(void)
{
		if(cobra4_aon_pd_top->aon_lf_clk_control & (1 << 4))
			return HF_XTAL48M;
		else
			return HF_RC_OSC24M;
}
/*
aon_lf_rstn_cfg

lf_rstn 控制

0: lf_rstn  复位 , 1: lf_rstn 释放

*/
void cobra4_aon_pd_top_lf_rstn_en(unsigned int en)
{
		
		if(en)  // reset
		{
				cobra4_aon_pd_top->aon_lf_clk_control &= ~(0x03 << 8);
		}
		else
		{
				cobra4_aon_pd_top->aon_lf_clk_control |= (0x03 << 8);
		}
	
}

/*
aon WDT时钟预分频
Freq_wdt = Freq_aon_hclk/(aon_wdt_cfg+1)
aon_wdt_cfg[0]

*/
void cobra4_aon_pd_top_watchdog_freq_div(unsigned int value)
{
		volatile unsigned int temp;
		temp = cobra4_aon_pd_top->aon_wdt_cfg;
		
		temp &= ~(0xff);
		temp |= (value&0xff);
	
		cobra4_aon_pd_top->aon_wdt_cfg = temp;
	
}

void cobra4_aon_pd_top_watchdog_freq_enable(unsigned int en)
{
		volatile unsigned int temp;
		temp = cobra4_aon_pd_top->aon_wdt_cfg;
		 
	  if(en)
		{
				temp |= 0x100;
		}
		else
		{
				temp &= ~0x100;
		}
		
		cobra4_aon_pd_top->aon_wdt_cfg = temp;
	
}

/*
aon_hclk 的clock源选择控制

00: rcosc24m, 01:xtal48m, 10:csem32k, 11:disable
aon_hclk_sw[0]

*/
void cobra4_aon_pd_top_aon_hclk_clock_selection(AON_HCLK_SOURCE source)
{
		volatile unsigned int temp;
		temp = cobra4_aon_pd_top->aon_hclk_sw;
		temp &= ~0x03;
		if(source == HCLK_RC_OSC24M)
		{
				cobra4_aon_pd_top->aon_hclk_sw = temp;
		}
		else if(source == HCLK_XTAL48M)
		{
				temp |= 0x01;
				cobra4_aon_pd_top->aon_hclk_sw = temp;
		}
		else if(source == HCLK_CSEM32K)
		{
				temp |= 0x02;
				cobra4_aon_pd_top->aon_hclk_sw = temp;
		}
		else if(source == HCLK_DISABLED)
		{
				temp |= 0x03;
				cobra4_aon_pd_top->aon_hclk_sw = temp;
		}
}

/*
aon_hclk 时钟预分频

Freq_aon_hclk = Freq_aon_scr/(aon_hclk_cfg+1)

aon_hclk_cfg[0]

*/
void cobra4_aon_pd_top_aon_hclk_divider(unsigned int  divider)
{
		volatile unsigned int temp;
		temp = cobra4_aon_pd_top->aon_hclk_cfg;
		
		temp &= ~0x1f;
		temp |= divider&0x1f;
	
		cobra4_aon_pd_top->aon_hclk_cfg = temp;
}

/*
aon_pclk 时钟预分频

Freq_aon_pclk = Freq_aon_hclk/(aon_hclk_cfg+1)

aon_pclk_cfg[0]

*/
void cobra4_aon_pd_top_aon_pclk_divider(unsigned int  divider)
{
		volatile unsigned int temp;
		temp = cobra4_aon_pd_top->aon_hclk_cfg;
		
		temp &= ~(0x3 << 6);
		temp |= ((divider&0x3) << 6);
	
		cobra4_aon_pd_top->aon_hclk_cfg = temp;
	
}

/*
0	aon_clk_gate_cfg[0]		0	W/R	hf_clk 时钟门控信号	1: enable
1	aon_clk_gate_cfg[1]		0	W/R	lf_clk 时钟门控信号	1: enable
2	aon_clk_gate_cfg[2]		0	W/R	low_power_clk 时钟门控信号	1: enable
3	aon_clk_gate_cfg[3]		0	W/R	clk_gpadc_32k 时钟门控信号	1: enable
4	aon_clk_gate_cfg[4]		0	W/R	rtc_gclk 时钟强制使能信号	1: enable
5	aon_clk_gate_cfg[5]		0	W/R		
6	aon_clk_gate_cfg[6]		0	W/R		
7	aon_clk_gate_cfg[7]		0	W/R		
*/

/*
hf_clk 时钟门控信号
1: enable

*/
void cobra4_aon_pd_top_an_hf_clk_gate_en(unsigned int  en)
{
	 if(en)
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg |= (1 << 0);
	 }
	 else
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg &= ~(1 << 0);
	 }
	
}

/*
lf_clk 时钟门控信号
1: enable


*/
void cobra4_aon_pd_top_an_lf_clk_gate_en(unsigned int  en)
{
	 if(en)
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg |= (1 << 1);
	 }
	 else
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg &= ~(1 << 1);
	 }
	
}

/*
low_power_clk 时钟门控信号

1: enable

*/
void cobra4_aon_pd_top_an_low_power_clk_gate_en(unsigned int  en)
{
	 if(en)
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg |= (1 << 2);
	 }
	 else
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg &= ~(1 << 2);
	 }
	
}

/*
clk_gpadc_32k 时钟门控信号

1: enable

*/
void cobra4_aon_pd_top_an_clk_gpadc_32k_gate_en(unsigned int  en)
{
	 if(en)
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg |= (1 << 3);
	 }
	 else
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg &= ~(1 << 3);
	 }
	
}

/*
rtc_gclk 时钟强制使能信号

1: enable

*/
void cobra4_aon_pd_top_rtc_gclk_gate_en(unsigned int  en)
{
	 if(en)
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg |= (1 << 4);
	 }
	 else
	 {
			cobra4_aon_pd_top->aon_clk_gate_cfg &= ~(1 << 4);
	 }
	
}

/*
rtc0 pclk en
*/
void cobra4_aon_pd_top_rtc0_pclk_en(unsigned int en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_en |= 0x01;
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_en &= ~0x01;
		}
}

void cobra4_aon_pd_top_rtc0_clk_en(unsigned int en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_en |= 0x02;
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_en &= ~0x02;
		}
}


/*
ext3 ( CSEM RF ) pclk en
*/
void cobra4_aon_pd_top_csem_rf_pclk_en(unsigned int en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_en |= (1 << 2);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_en &= ~(1 << 2);
		}
}

/*
ucpu pclk en
小 MCU 的控制
*/
void cobra4_aon_pd_top_ucpu_pclk_en(unsigned int en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_en |= (1 << 3);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_en &= ~(1 << 3);
		}
}

/*
dualtimers pclk en

*/
void cobra4_aon_pd_top_dualtimers_pclk_en(unsigned int en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_en |= (1 << 4);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_en &= ~(1 << 4);
		}
}

/*
watchdog pclk en

*/
void cobra4_aon_pd_top_watchdog_pclk_en(unsigned int en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_en |= (1 << 5);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_en &= ~(1 << 5);
		}
}

/*
ext1 (dc_dc pmu) pclk en

*/
void cobra4_aon_pd_top_dcdc_pmu_pclk_en(unsigned int en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_en |= (1 << 6);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_en &= ~(1 << 6);
		}
}

/*
ext0 (pinmux ) pclk en

*/
void cobra4_aon_pd_top_pinmux_pclk_en(unsigned int en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_en |= (1 << 7);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_en &= ~(1 << 7);
		}
}



/*
"aon_pd模块的apb时钟mask，用于bypass模块的门控信号，
1:bypass 模块门控信号，0:模块门控信号有效"
*/
void cobra4_aon_pd_top_rtc0_mask_en(unsigned int  en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg |= (1 << 0);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg &= ~(1 << 0);
		}
}

/*
ext3(csem rf) mask control

*/
void cobra4_aon_pd_top_csem_rf_mask_en(unsigned int  en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg |= (1 << 2);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg &= ~(1 << 2);
		}
}


/*

umcu mask control
*/
void cobra4_aon_pd_top_ucpu_mask_en(unsigned int  en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg |= (1 << 3);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg &= ~(1 << 3);
		}
}

/*
dualtimers mask gate control

*/
void cobra4_aon_pd_top_dualtimers_mask_en(unsigned int  en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg |= (1 << 4);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg &= ~(1 << 4);
		}
}

/*
watchdog mask gate control

*/
void cobra4_aon_pd_top_watchdog_mask_en(unsigned int  en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg |= (1 << 5);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg &= ~(1 << 5);
		}
}

/*
ext1(dcdc pmu dig) mask gate control

*/
void cobra4_aon_pd_top_dcdc_pmu_dig_mask_en(unsigned int  en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg |= (1 << 6);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg &= ~(1 << 6);
		}
}

/*
ext0(pinmux) mask gate control

*/
void cobra4_aon_pd_top_pinmux_mask_en(unsigned int  en)
{
		if(en)
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg |= (1 << 7);
		}
		else
		{
			cobra4_aon_pd_top->aon_pclk_mask_cfg &= ~(1 << 7);
		}
}

/*
aon 模块软复位，1：复位

rtc0  soft reset
*/
void cobra4_aon_pd_top_rtc0_presetn_en(unsigned int  en)
{
		if(en)
		{
			  cobra4_aon_pd_top->aon_psrst_en |= (1<<0);
		}
		else
		{
				cobra4_aon_pd_top->aon_psrst_en &= ~(1<<0);
		}
	
}

/*
aon 模块软复位，1：复位

dualtimers  soft reset
*/
void cobra4_aon_pd_top_dualtimers_presetn_en(unsigned int  en)
{
		if(en)
		{
			  cobra4_aon_pd_top->aon_psrst_en |= (1<<4);
		}
		else
		{
				cobra4_aon_pd_top->aon_psrst_en &= ~(1<<4);
		}
	
}

/*
aon 48M reset

dualtimers  soft reset
*/
void cobra4_aon_pd_top_icytrx_reset_en(unsigned int  en)
{
		if(en)
		{
			  cobra4_aon_pd_top->aon_psrst_en |= (1<<2);
		}
		else
		{
				cobra4_aon_pd_top->aon_psrst_en &= ~(1<<2);
		}
	
}


/*
aon 模块软复位，1：复位

watchdog  soft reset
*/
void cobra4_aon_pd_top_watchdog_presetn_en(unsigned int  en)
{
		if(en)
		{
			  cobra4_aon_pd_top->aon_psrst_en |= (1<<5);
		}
		else
		{
				cobra4_aon_pd_top->aon_psrst_en &= ~(1<<5);
		}
	
}



/*
ext0 模块软复位，1：复位

csem rf  soft reset
*/
void cobra4_aon_pd_top_csem_rf_presetn_en(unsigned int  en)
{
		if(en)
		{
			  cobra4_aon_pd_top->aon_psrst_en |= (1<<7);
		}
		else
		{
				cobra4_aon_pd_top->aon_psrst_en &= ~(1<<7);
		}
	
}

/*
ucpu软复位请求，硬件自清除
1：软复位


*/
void cobra4_aon_pd_top_umcu_srst_req_en(unsigned int  en)
{
		if(en)
		{
			  cobra4_aon_pd_top->aon_psrst_en |= (1<<16);
		}
		else
		{
				cobra4_aon_pd_top->aon_psrst_en &= ~(1<<16);
		}
	
}

/*
gpadc 8M时钟源选择
[1:0]： 
0x，gpadc 8M时钟关闭
10:时钟源为rcosc24M, 11:时钟源为xtal48M


*/
void cobra4_aon_pd_top_gpadc_clock_selection(GPADC_CLK_SOURCE source)
{
		volatile unsigned  int temp;
	
		if(source == GPADC_CLK_DISABLE)
		{
				cobra4_aon_pd_top->aon_aux_clk_cfg &= ~0x03;
		}
		else if(source == GPADC_RC_OSC24M)
		{
				temp = cobra4_aon_pd_top->aon_aux_clk_cfg;
				temp &= ~0x03;
				temp |= 0x02;
				cobra4_aon_pd_top->aon_aux_clk_cfg = temp;
		}
		else if(source == GPADC_XTAL48M)
		{
				temp = cobra4_aon_pd_top->aon_aux_clk_cfg;
				temp &= ~0x03;
				temp |= 0x03;
				cobra4_aon_pd_top->aon_aux_clk_cfg = temp;
		}
	
}

/*
tdc 时钟源选择
[3:2]: 0x，tdc时钟关闭;10:时钟源为rcosc24M,频率24M;
11:时钟源为xtal48M,频率48M;

*/
void cobra4_aon_pd_top_tdc_clock_selection(TDC_CLK_SOURCE source)
{
		volatile unsigned  int temp;
	
		if(source == TDC_CLK_DISABLE)
		{
				cobra4_aon_pd_top->aon_aux_clk_cfg &= ~(0x03 << 2);
		}
		else if(source == TDC_RC_OSC24M)
		{
				temp = cobra4_aon_pd_top->aon_aux_clk_cfg;
				temp &= ~(0x03 << 2);
				temp |= (0x02 << 2);
				cobra4_aon_pd_top->aon_aux_clk_cfg = temp;
		}
		else if(source == TDC_XTAL48M)
		{
				temp = cobra4_aon_pd_top->aon_aux_clk_cfg;
				temp &= ~(0x03 << 2);
				temp |= (0x03 << 2);
				cobra4_aon_pd_top->aon_aux_clk_cfg = temp;
		}
	
}

/*
内部频率表使能
1：使能,本功能只有当xtal48M有效时才可用


*/
void cobra4_aon_pd_top_fm_set(unsigned int en)
{
		if(en)
		{
				cobra4_aon_pd_top->fm_clk_measure |= 0x01;
		}
		else
		{
				cobra4_aon_pd_top->fm_clk_measure &= ~0x01;
		}	
}

/*
内部测量时钟源选择
000： 保留；001： rcosc_32k；010： xtal_32k
011： rcosc24m_clk；100：hf_clk；101：aon_hclki
其他：保留


*/
void cobra4_aon_pd_top_fm_clock_selection(MEASURE_CLK_SOURCE source)
{
		volatile unsigned int temp;
		
	  temp = cobra4_aon_pd_top->fm_clk_measure;
		temp &= ~(0x07 << 8);
		if(source == MEASURE_RC_OSC_32K)
		{
				temp |= 1<<8;
				cobra4_aon_pd_top->fm_clk_measure = temp;
		}
		else if(source == MEASURE_XTAL_32K)
		{
				temp |= 2<<8;
				cobra4_aon_pd_top->fm_clk_measure = temp;
		}
		else if(source == MEASURE_RC_OSC_24M)
		{
				temp |= 3<<8;
				cobra4_aon_pd_top->fm_clk_measure = temp;
		}
		else if(source == MEASURE_HF_CLK)
		{
				temp |= 4<<8;
				cobra4_aon_pd_top->fm_clk_measure = temp;
		}
		else if(source == MEASURE_AON_HCLK)
		{
				temp |= 5<<8;
				cobra4_aon_pd_top->fm_clk_measure = temp;
		}
		
}

/*
测量是否结束
*/
unsigned int cobra4_aon_pd_top_fm_clock_measure_ok(void)
{
		if(cobra4_aon_pd_top->fm_clk_measure & (1<<7))
				return 1;
		else
			return 0;
}

/*
测量时钟源，测量窗口配置

*/
void cobra4_aon_pd_top_fm_clock_measure_window(unsigned int window_time)
{
		volatile unsigned int temp;
		
	  temp = cobra4_aon_pd_top->fm_clk_measure;
		temp &= ~(0xffff << 16);
		temp |= ((window_time&0xffff) << 16);
		cobra4_aon_pd_top->fm_clk_measure = temp;
}

/*
测量窗口下的基准时钟读数

F_sel = 48*r_cnt/f_cnt, 单位MHz

*/
unsigned int cobra4_aon_pd_top_fm_clock_value(void)
{
		return cobra4_aon_pd_top->fm_clk_measure_count;	
}

/*
pd1 por mask控制
1：pd1 por状态位不受mcu_vd por控制


*/
void cobra4_aon_pd_top_mcu_vd_soft_rdy(unsigned int en)
{
		if(en)
		{
				cobra4_aon_pd_top->mcu_power_manager |= 0x01;
		}
		else
		{
				cobra4_aon_pd_top->mcu_power_manager &= ~0x01;
		}
	
}

/*
pd1 por状态位
1：pd1 电源电压有效
*/
unsigned int cobra4_aon_pd_top_mcu_nrst_ret_sync(void)
{
		if(cobra4_aon_pd_top->mcu_power_manager & 0x02)
			return 1;
		else
			return 0;
}

/*
mcu standby请求状态
m4发出的系统standby请求，ucpu处理该请求

*/
unsigned int cobra4_aon_pd_top_mcu_standby2aon_req(void)
{
		if(cobra4_aon_pd_top->mcu_power_manager & (1 << 8))
			return 1;
		else
			return 0;
}

/*
mcu shutdown请求状态

m4发出的系统shutdown请求，ucpu处理该请求

*/
unsigned int cobra4_aon_pd_top_mcu_shutdown_req(void)
{
		if(cobra4_aon_pd_top->mcu_power_manager & (1 << 9))
			return 1;
		else
			return 0;
}

/*
m4 系统remap使能

*/
void cobra4_aon_pd_top_mcu_remap2flash_cfg_en(unsigned int en)
{
		if(en)
		{
				cobra4_aon_pd_top->mcu_power_manager |= (1<<31);
		}
		else
		{
			 cobra4_aon_pd_top->mcu_power_manager &= ~(1<<31);
		}
}

/*
   查看remap 功能是否打开
*/
unsigned int cobra4_aon_pd_top_get_mcu_remap2flash_cfg(void)
{
	   return (cobra4_aon_pd_top->mcu_power_manager & (1 <<31));
}


/*

aon2mcu event source0 selection

*/

void cobra4_aon_pd_top_aon2mcu_source0_selection(AON_EVENT_SOURCE selection)
{
		cobra4_aon_pd_top->aon2mcu_src0_src1_src2_src3 &= ~(0x3f << 0);
		cobra4_aon_pd_top->aon2mcu_src0_src1_src2_src3 |= (selection << 0);
}

/*

aon2mcu event source1 selection

*/

void cobra4_aon_pd_top_aon2mcu_source1_selection(AON_EVENT_SOURCE selection)
{
		cobra4_aon_pd_top->aon2mcu_src0_src1_src2_src3 &= ~(0x3f << 8);
		cobra4_aon_pd_top->aon2mcu_src0_src1_src2_src3 |= (selection << 8);
}

/*

aon2mcu event source2 selection

*/

void cobra4_aon_pd_top_aon2mcu_source2_selection(AON_EVENT_SOURCE selection)
{
		cobra4_aon_pd_top->aon2mcu_src0_src1_src2_src3 &= ~(0x3f << 16);
		cobra4_aon_pd_top->aon2mcu_src0_src1_src2_src3 |= (selection << 16);
}


/*

aon2mcu event source3 selection

*/

void cobra4_aon_pd_top_aon2mcu_source3_selection(AON_EVENT_SOURCE selection)
{
		cobra4_aon_pd_top->aon2mcu_src0_src1_src2_src3 &= ~(0x3f << 24);
		cobra4_aon_pd_top->aon2mcu_src0_src1_src2_src3 |= (selection << 24);
}


/*
//lianbo
aon2mcu event source3 get

*/

int cobra4_aon_pd_top_aon2mcu_source3_get(void)
{
	return (cobra4_aon_pd_top->aon2mcu_src0_src1_src2_src3 >> 24)&0x3f;
}


/*

aon2mcu event source4 selection

*/

void cobra4_aon_pd_top_aon2mcu_source4_selection(AON_EVENT_SOURCE selection)
{
		cobra4_aon_pd_top->aon2mcu_src4_src5 &= ~(0x3f << 0);
		cobra4_aon_pd_top->aon2mcu_src4_src5 |= (selection << 0);
}

/*

aon2mcu event source5 selection

*/

void cobra4_aon_pd_top_aon2mcu_source5_selection(AON_EVENT_SOURCE selection)
{
		cobra4_aon_pd_top->aon2mcu_src4_src5 &= ~(0x3f << 8);
		cobra4_aon_pd_top->aon2mcu_src4_src5 |= (selection << 8);
}

#if (COBRA_ADC_EN)
/*
switch on sw_adc3v3

*/
void cobra4_aon_pd_top_d2a_vdd3_adc_en(unsigned int en)
{
	if (en)
	{
		cobra4_aon_pd_top->aon_ana_cfg0 |= (1UL << 3);
	}
	else
	{
		cobra4_aon_pd_top->aon_ana_cfg0 &= ~(1UL << 3);
	}
}

void cobra4_aon_pd_top_xtal32_trim(unsigned char value)
{
	cobra4_aon_pd_top->aon_ana_cfg1 = cobra4_aon_pd_top->aon_ana_cfg1 & (~0x0f);
	cobra4_aon_pd_top->aon_ana_cfg1 |= (value & 0x0f);

	//		REG_PL_WR(AON_PD_BASE+AON_ANA_CFG1,(REG_PL_RD(AON_PD_BASE+AON_ANA_CFG1)&(~XO32K_GMTRIM_MASK))|(0x0f<<XO32K_GMTRIM_POS));
}

void cobra4_aon_pd_top_d2a_btgpadc_vpin_sel(unsigned char vpin_sel)
{
	cobra4_aon_pd_top->aon_ana_cfg1 &= ~(0x7 << 16);
	cobra4_aon_pd_top->aon_ana_cfg1 |= ((vpin_sel & 0x07)<<16);
}

void cobra4_aon_pd_top_d2a_btgpadc_diff(unsigned char btgpadc_diff)
{
	cobra4_aon_pd_top->aon_ana_cfg1 &= ~(0xf << 19);
	cobra4_aon_pd_top->aon_ana_cfg1 |= ((btgpadc_diff & 0x0f)<<19);
}

/*
d2a_btgapadc_en_gpadc

*/
void cobra4_aon_pd_top_d2a_btgapadc_en_gpadc(unsigned int en)
{
	if (en)
	{
		cobra4_aon_pd_top->aon_ana_cfg1 |= (1UL << 23);
	}
	else
	{
		cobra4_aon_pd_top->aon_ana_cfg1 &= ~(1UL << 23);
	}
}

/*
d2a_btgapadc_en_gpadc

*/
void cobra4_aon_pd_top_d2a_btgpadc_en_t2vc(unsigned int en)
{
	if (en)
	{
		cobra4_aon_pd_top->aon_ana_cfg1 |= (1UL << 24);
	}
	else
	{
		cobra4_aon_pd_top->aon_ana_cfg1 &= ~(1UL << 24);
	}
}

/*
btgpadc_mode

*/
void cobra4_aon_pd_top_d2a_btgpadc_mode (unsigned char btgpadc_mode)
{
	cobra4_aon_pd_top->aon_ana_cfg1 &= ~(0x3 << 25);
	cobra4_aon_pd_top->aon_ana_cfg1 |= ((btgpadc_mode & 0x03)<<25);
}
/*
d2a_vr3v3to1v8_ldoadc_en

*/
void cobra4_aon_pd_top_d2a_vr3v3to1v8_ldoadc_en(unsigned int en)
{
	if (en)
	{
		cobra4_aon_pd_top->aon_ana_cfg2 |= (1UL << 0);
	}
	else
	{
		cobra4_aon_pd_top->aon_ana_cfg2 &= ~(1UL << 0);
	}
}

/*
d2a_vr3v3to1v8_1v2ref_sel

*/
void cobra4_aon_pd_top_d2a_vr3v3to1v8_1v2ref_sel(unsigned int en)
{
	if (en)
	{
		cobra4_aon_pd_top->aon_ana_cfg2 |= (1UL << 1);
	}
	else
	{
		cobra4_aon_pd_top->aon_ana_cfg2 &= ~(1UL << 1);
	}
}

/*
d2a_btgpadc_adcgain

*/
void cobra4_aon_pd_top_d2a_btgpadc_adcgain(unsigned int gain)
{
    cobra4_aon_pd_top->aon_ana_cfg3 &= ~(0x7 << 13);
	cobra4_aon_pd_top->aon_ana_cfg3 |= ((gain & 0x7) << 13);
}

void cobra4_aon_pd_top_gpadc_32k_enable(unsigned int en)
{
		if(en)
		{
				cobra4_aon_pd_top->aon_ana_cfg2 |= (1<<8);
		}
		else
		{
				cobra4_aon_pd_top->aon_ana_cfg2 &= ~(1<<8);
		}
	
}

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if (COBRA_ADC_EN_VERSION2)
void cobra4_aon_pd_top_gpadc_32k_enable(unsigned int en)
{
		if(en)
		{
				cobra4_aon_pd_top->aon_ana_cfg2 |= (1<<8);
		}
		else
		{
				cobra4_aon_pd_top->aon_ana_cfg2 &= ~(1<<8);
		}
	
}


void cobra4_aon_pd_top_xtal32_trim(unsigned char value)
{
	cobra4_aon_pd_top->aon_ana_cfg1 = cobra4_aon_pd_top->aon_ana_cfg1 & (~0x0f);
	cobra4_aon_pd_top->aon_ana_cfg1 |= (value & 0x0f);
}

unsigned int cobra4_aon_app_ota_flag_read(void)

{

	return (cobra4_aon_pd_top->aon_spare0)>>24;

}

void cobra4_aon_app_ota_flag_write(unsigned char flag)

{

	cobra4_aon_pd_top->aon_spare0 &= 0x00ffffff;

	cobra4_aon_pd_top->aon_spare0 |= flag << 24;

}

//2021/6/7 添加模拟控制部分
//liujian
//vdac enable
void cobra4_aon_pd_top_d2a_vdac_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 |= 0x01;
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 &= ~0x01;
		}	
}

//compare A enable
void cobra4_aon_pd_top_d2a_cpa_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 |= (1 << 1);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 &= ~(1 << 1);
		}	
}

//compare B enable
void cobra4_aon_pd_top_d2a_cpb_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 |= (1 << 2);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 &= ~(1 << 2);
		}	
}

//vdd connect adc switch enable
void cobra4_aon_pd_top_d2a_vdd3_adc_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 |= (1 << 3);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 &= ~(1 << 3);
		}	
}

//ldo_pd0 transient
void cobra4_aon_pd_top_d2a_ldodig_pd0_high_curr_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 |= (1 << 4);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 &= ~(1 << 4);
		}	
}

//ldo_pd0 trim
void cobra4_aon_pd_top_d2a_ldodig_pd0_vreg_trim(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg0;
	 temp  &= ~(0x0f << 5);
	 value &= 0x0f;
	 temp  |= (value << 5);
	 cobra4_aon_pd_top->aon_ana_cfg0 = temp;
}


//ldo_pd1 transient
void cobra4_aon_pd_top_d2a_ldodig_pd1_high_curr_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 |= (1 << 9);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 &= ~(1 << 9);
		}	
}

//ldo_pd1 trim
void cobra4_aon_pd_top_d2a_ldodig_pd1_vreg_trim(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg0;
	 temp  &= ~(0x0f << 10);
	 value &= 0x0f;
	 temp  |= (value << 10);
	 cobra4_aon_pd_top->aon_ana_cfg0 = temp;
}


//d2a_efbrwot_en
void cobra4_aon_pd_top_d2a_efbrwot_en(unsigned int en)
{
	 	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 |= (1 << 14);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 &= ~(1 << 14);
		}	
}


////d2a_bgln_t_trim
//void cobra4_aon_pd_top_d2a_bgln_t_trim(unsigned int value)
//{
//	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg0;
//	 temp  &= ~(0x0f << 16);
//	 value &= 0x0f;
//	 temp  |= (value << 16);
//	 cobra4_aon_pd_top->aon_ana_cfg0 = temp;
//}

////d2a_bgln_vref_trim
//void cobra4_aon_pd_top_d2a_bgln_vref_trim(unsigned int value)
//{
//	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg0;
//	 temp  &= ~(0x1f << 20);
//	 value &= 0x1f;
//	 temp  |= (value << 20);
//	 cobra4_aon_pd_top->aon_ana_cfg0 = temp;
//}


//d2a_ldoanaforpll_high_curr
void cobra4_aon_pd_d2a_ldoanaforpll_high_curr_en(unsigned int en)
{
	 	if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 |= (1 << 25);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg0 &= ~(1 << 25);
		}	
}

////d2a_ldoanaforpll_trim
//void cobra4_aon_pd_top_d2a_ldoanaforpll_trim(unsigned int value)
//{
//	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg0;
//	 temp  &= ~(0x1f << 26);
//	 value &= 0x1f;
//	 temp  |= (value << 26);
//	 cobra4_aon_pd_top->aon_ana_cfg0 = temp;
//}

///////////////////////
////d2a_xo32k_gmtrim
//void cobra4_aon_pd_top_d2a_xo32k_gmtrim(unsigned int value)
//{
//	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg1;
//	 temp  &= ~(0x0f << 0);
//	 value &= 0x0f;
//	 temp  |= (value << 0);
//	 cobra4_aon_pd_top->aon_ana_cfg1 = temp;
//}


//d2a_xo32k_cl
void cobra4_aon_pd_top_d2a_xo32k_cl(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg1;
	 temp  &= ~(0x0f << 4);
	 value &= 0x0f;
	 temp  |= (value << 4);
	 cobra4_aon_pd_top->aon_ana_cfg1 = temp;
}


//d2a_rco24m_ftune
void cobra4_aon_pd_top_d2a_rco24m_ftune(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg1;
	 temp  &= ~(0x7f << 8);
	 value &= 0x7f;
	 temp  |= (value << 8);
	 cobra4_aon_pd_top->aon_ana_cfg1 = temp;
}

//d2a_btgpadc_vpin_sel
/*
"differential pos input select DPIN
vpin_sel:  0  1  2  3  4  5  6  7 
    pin1: 31 30 29 28 27 26 25 24"
*/
void cobra4_aon_pd_top_d2a_btgpadc_vpin_sel(DIFFERENTIAL_POS_INPUT_DPIN input_pin)
{
	 unsigned int value = 0;
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg1;
	 temp  &= ~(0x7 << 16);
	
	 switch(input_pin)
	 {
		 case DIFF_POS_DPIN31:
			 value = 0;
			 break;
		 case DIFF_POS_DPIN30:
			 value = 1;
			 break;
		 case DIFF_POS_DPIN29:
			 value = 2;
			 break;
		 case DIFF_POS_DPIN28:
			 value = 3;
			 break;
		 case DIFF_POS_DPIN27:
			 value = 4;
			 break;
		 case DIFF_POS_DPIN26:
			 value = 5;
			 break;
		 case DIFF_POS_DPIN25:
			 value = 6;
			 break;
		 case DIFF_POS_DPIN24:
			 value = 7;
			 break;
		 default:
			 break;
	 }
	 value &= 0x7;
	 temp  |= (value << 16);
	 cobra4_aon_pd_top->aon_ana_cfg1 = temp;
}

//d2a_btgpadc_diff
/*
"differential neg input select DPIN
diff[2:0]:  0  1  2  3  4  5  6  7 
     pin2: 31 30 29 28 27 26 25 24
+(diff<<3) means set bit3=1'b1, to validate differential mode"

*/
void cobra4_aon_pd_top_d2a_btgpadc_diff(DIFFERENTIAL_NEG_INPUT_DPIN input_pin, unsigned int diff_mode_en)
{
	 unsigned int value = 0;
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg1;
	 temp  &= ~(0xf << 19);
	
	 switch(input_pin)
	 {
		 case DIFF_NEG_DPIN31:
			 value = 0;
			 break;
		 case DIFF_NEG_DPIN30:
			 value = 1;
			 break;
		 case DIFF_NEG_DPIN29:
			 value = 2;
			 break;
		 case DIFF_NEG_DPIN28:
			 value = 3;
			 break;
		 case DIFF_NEG_DPIN27:
			 value = 4;
			 break;
		 case DIFF_NEG_DPIN26:
			 value = 5;
			 break;
		 case DIFF_NEG_DPIN25:
			 value = 6;
			 break;
		 case DIFF_NEG_DPIN24:
			 value = 7;
			 break;
		 default:
			 break;
	 }
	 
	 if(diff_mode_en)
	 {
		   value |= 0x08;
	 }
	 
	 value &= 0xf;
	 temp  |= (value << 19);
	 cobra4_aon_pd_top->aon_ana_cfg1 = temp;
}

//d2a_btgapadc_en_gpadc

void cobra4_aon_pd_top_d2a_btgapadc_en_gpadc_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg1 |= (1 << 23);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg1 &= ~(1 << 23);
		}	
}


//d2a_btgpadc_en_t2vc

void cobra4_aon_pd_top_d2a_btgpadc_en_t2vc_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg1 |= (1 << 24);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg1 &= ~(1 << 24);
		}	
}


//d2a_btgpadc_mode
void cobra4_aon_pd_top_d2a_btgpadc_mode(GPADC_MODE mode)
{
	 unsigned int  value = 0;
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg1;
	 temp  &= ~(0x3 << 25);
	 switch(mode)
	 {
		 case ADC_MODE_TM:
			 cobra4_aon_pd_top_d2a_btgpadc_en_t2vc_en(1);
		   value = 0;
			 break;
		 case ADC_MODE_BM:
			 cobra4_aon_pd_top_d2a_btgpadc_en_t2vc_en(0);
		   value = 1;
			 break;
		 case ADC_MODE_NORMAL:
			 cobra4_aon_pd_top_d2a_btgpadc_en_t2vc_en(0);
		   value = 2;
			 break;
		 default:
			 break;
	 }
	 value &= 0x3;
	 temp  |= (value << 25);
	 cobra4_aon_pd_top->aon_ana_cfg1 = temp;
}


//d2a_cpb_dvider_sel
void cobra4_aon_pd_top_d2a_cpb_dvider_sel(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg1;
	 temp  &= ~(0x3 << 27);
	 value &= 0x3;
	 temp  |= (value << 27);
	 cobra4_aon_pd_top->aon_ana_cfg1 = temp;
}

//d2a_cpa_out_reverse
void cobra4_aon_pd_top_d2a_cpa_out_reverse_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg1 |= (1 << 29);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg1 &= ~(1 << 29);
		}	
}

//d2a_xo32k_cnt
void cobra4_aon_pd_top_d2a_xo32k_cnt(unsigned int count)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg1;
	 temp  &= ~(0x3 << 30);
	 count &= 0x3;
	 temp  |= (count << 30);
	 cobra4_aon_pd_top->aon_ana_cfg1 = temp;
}

//////////////////////////////////
//analog vdd18 enable

void cobra4_aon_pd_top_d2a_vr3v3to1v8_ldoadc_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg2 |= (1 << 0);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg2 &= ~(1 << 0);
		}	
}


//analog vdd18 vref sel
//0:ldo_adc ref from bgp_lp
//1:ldo_adc ref from bgp_ln
void cobra4_aon_pd_top_d2a_vr3v3to1v8_1v2ref_sel(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg2 |= (1 << 1);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg2 &= ~(1 << 1);
		}	
}

//analog vdd18 bgpass
void cobra4_aon_pd_top_d2a_vr3v3to1v8_force_bypass(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg2 |= (1 << 2);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg2 &= ~(1 << 2);
		}	
}

////analog vdd18 voltage trim

//void cobra4_aon_pd_top_d2a_vr3v3to1v8_votrim(unsigned int value)
//{
//	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg2;
//	 temp  &= ~(0x7 << 3);
//	 value &= 0x7;
//	 temp  |= (value << 3);
//	 cobra4_aon_pd_top->aon_ana_cfg2 = temp;
//}

////d2a_btgpadc_adccal

//void cobra4_aon_pd_top_d2a_btgpadc_adccal(unsigned int value)
//{
//	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg2;
//	 temp  &= ~(0x3 << 6);
//	 value &= 0x3;
//	 temp  |= (value << 6);
//	 cobra4_aon_pd_top->aon_ana_cfg2 = temp;
//}

//d2a_btgpadc_32k_mode
/*
"0:8M mode, data clk is 250kbps
1:32K mode"

*/
void cobra4_aon_pd_top_d2a_btgpadc_32k_mode_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg2 |= (1 << 8);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg2 &= ~(1 << 8);
		}	
}


//d2a_cpa_neg_sel
void cobra4_aon_pd_top_d2a_cpa_neg_sel(unsigned int sel)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg2;
	 temp  &= ~(0xf << 9);
	 sel   &= 0xf;
	 temp  |= (sel << 9);
	 cobra4_aon_pd_top->aon_ana_cfg2 = temp;
}

//d2a_cpa_pos_sel
void cobra4_aon_pd_top_d2a_cpa_pos_sel(unsigned int sel)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg2;
	 temp  &= ~(0x7 << 13);
	 sel   &= 0x7;
	 temp  |= (sel << 13);
	 cobra4_aon_pd_top->aon_ana_cfg2 = temp;
}

//d2a_vdac_setx
void cobra4_aon_pd_top_d2a_vdac_setx(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg2;
	 temp    &=  ~(0x3f << 16);
	 value   &=  0x3f;
	 temp    |=  (value << 16);
	 cobra4_aon_pd_top->aon_ana_cfg2 = temp;
}

//d2a_vdac_sety
void cobra4_aon_pd_top_d2a_vdac_sety(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg2;
	 temp    &=  ~(0x3f << 22);
	 value   &=  0x3f;
	 temp    |=  (value << 22);
	 cobra4_aon_pd_top->aon_ana_cfg2 = temp;
}

///////////////////////////
//d2a_vdac_setz
void cobra4_aon_pd_top_d2a_vdac_setz(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg3;
	 temp    &=  ~(0x7f << 0);
	 value   &=  0x7f;
	 temp    |=  (value << 0);
	 cobra4_aon_pd_top->aon_ana_cfg3 = temp;
}

//d2a_cpb_neg_sel

void cobra4_aon_pd_top_d2a_cpb_neg_sel(unsigned int sel)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg3;
	 temp  &= ~(0x3 << 7);
	 sel   &= 0x3;
	 temp  |= (sel << 7);
	 cobra4_aon_pd_top->aon_ana_cfg3 = temp;
}

//d2a_cpb_pos_sel

void cobra4_aon_pd_top_d2a_cpb_pos_sel(unsigned int sel)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg3;
	 temp  &= ~(0xf << 9);
	 sel   &= 0xf;
	 temp  |= (sel << 9);
	 cobra4_aon_pd_top->aon_ana_cfg3 = temp;
}

//d2a_btgpadc_adcgain

void cobra4_aon_pd_top_d2a_btgpadc_adcgain(unsigned int gain)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg3;
	 temp  &= ~(0x7 << 13);
	 gain   &= 0x7;
	 temp  |= (gain << 13);
	 cobra4_aon_pd_top->aon_ana_cfg3 = temp;
}


//d2a_anatmux_en

void cobra4_aon_pd_top_d2a_anatmux_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg3 |= (1 << 16);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg3 &= ~(1 << 16);
		}	
}


//d2a_anatmux_sel
void cobra4_aon_pd_top_d2a_anatmux_sel(unsigned int sel)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg3;
	 temp  &= ~(0xf << 17);
	 sel   &= 0xf;
	 temp  |= (sel << 17);
	 cobra4_aon_pd_top->aon_ana_cfg3 = temp;
}


//d2a_aon_tmux_en

void cobra4_aon_pd_top_d2a_aon_tmux_en(unsigned int en)
{
	  if(en)
		{
			  cobra4_aon_pd_top->aon_ana_cfg3 |= (1 << 21);
		}
		else
		{
			  cobra4_aon_pd_top->aon_ana_cfg3 &= ~(1 << 21);
		}	
}

//d2a_aon_tmux_sel
void cobra4_aon_pd_top_d2a_aon_tmux_sel(unsigned int sel)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg3;
	 temp  &= ~(0x3 << 22);
	 sel   &= 0x3;
	 temp  |= (sel << 22);
	 cobra4_aon_pd_top->aon_ana_cfg3 = temp;
}

#endif

//d2a_ldoanaforpll_trim
void cobra4_aon_pd_top_d2a_ldoanaforpll_trim(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg0;
	 temp  &= ~(0x1f << 26);
	 value &= 0x1f;
	 temp  |= (value << 26);
	 cobra4_aon_pd_top->aon_ana_cfg0 = temp;
}

/////////////////////
//d2a_xo32k_gmtrim
void cobra4_aon_pd_top_d2a_xo32k_gmtrim(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg1;
	 temp  &= ~(0x0f << 0);
	 value &= 0x0f;
	 temp  |= (value << 0);
	 cobra4_aon_pd_top->aon_ana_cfg1 = temp;
}

//analog vdd18 voltage trim

void cobra4_aon_pd_top_d2a_vr3v3to1v8_votrim(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg2;
	 temp  &= ~(0x7 << 3);
	 value &= 0x7;
	 temp  |= (value << 3);
	 cobra4_aon_pd_top->aon_ana_cfg2 = temp;
}

//d2a_btgpadc_adccal

void cobra4_aon_pd_top_d2a_btgpadc_adccal(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg2;
	 temp  &= ~(0x3 << 6);
	 value &= 0x3;
	 temp  |= (value << 6);
	 cobra4_aon_pd_top->aon_ana_cfg2 = temp;
}

//d2a_bgln_t_trim
void cobra4_aon_pd_top_d2a_bgln_t_trim(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg0;
	 temp  &= ~(0x0f << 16);
	 value &= 0x0f;
	 temp  |= (value << 16);
	 cobra4_aon_pd_top->aon_ana_cfg0 = temp;
}

//d2a_bgln_vref_trim
void cobra4_aon_pd_top_d2a_bgln_vref_trim(unsigned int value)
{
	 volatile unsigned int temp = cobra4_aon_pd_top->aon_ana_cfg0;
	 temp  &= ~(0x1f << 20);
	 value &= 0x1f;
	 temp  |= (value << 20);
	 cobra4_aon_pd_top->aon_ana_cfg0 = temp;
}

#endif

