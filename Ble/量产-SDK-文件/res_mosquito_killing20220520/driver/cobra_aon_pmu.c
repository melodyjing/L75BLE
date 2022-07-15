/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_aon_pmu.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/8

	Description :   
					cobra power manger control
					according from document cobra2 register
					
					Power, Reset and Clock Management


	Changed Log	:

		Liujian  2019/3/13			- Creation


*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_aon_pmu.h"


#if COBRA_AON_PMU_EN

static volatile COBRA_AON_PMU_T * cobra_aon_pmu;

void cobra_aon_pmu_init(void)
{
		cobra_aon_pmu = (COBRA_AON_PMU_T *)COBRA_AON_PMU_BASE;   
}

/*
aon config register write protect enable

"0: write diable
 1: write enable"

*/
void cobra_aon_pmu_protection_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_protcetion = 1;
		}
		else
		{
				cobra_aon_pmu->aon_protcetion = 0;
		}
}
/*
PD0-SW  to always on power domain

*/

void cobra_aon_pmu_d_ldo_pd0_perm_enb(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 0);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 0);
		}
}
/*

	LDO PD0-DIG
*/
void cobra_aon_pmu_d_ldo_pd0dig1_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 1);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 1);
		}
}

/*

	LDO Retention
*/
void cobra_aon_pmu_d_ldo_ret_enb(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 2);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 2);
		}
}

/*

	PD1-DIG1 domain
*/
void cobra_aon_pmu_d_ldo_pd1dig1_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 3);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 3);
		}
}
/*

	PD1-DIG2 domain
*/
void cobra_aon_pmu_d_ldo_pd1dig2_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 4);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 4);
		}
}

void cobra_aon_pmu_d_cu_bgp_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 5);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 5);
		}
}

///*

//PD0-SW control
//*/
//void cobra_aon_pmu_d_pd0_perm_enb(unsigned int en)
//{
//		if(en)
//		{
//				cobra_aon_pmu->aon_ldo_pd |= (1 << 8);
//		}
//		else
//		{
//				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 8);
//		}
//}

/*
PD0-SW control

*/
void cobra_aon_pmu_d_pd0_ldo1_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 9);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 9);
		}
}

/*
PD1-SW control

*/
void cobra_aon_pmu_d_pd1_ret_enb(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 10);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 10);
		}
}

/*

PD1-SW control
*/
void cobra_aon_pmu_d_pd1_ldo1_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 11);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 11);
		}
}

/*
VDDIO post-driver switch enable

*/
void cobra_aon_pmu_d_vddio_postdrv_pd1_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 12);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 12);
		}
}

/*
VDDIO ADC supply switch enable
*/
void cobra_aon_pmu_d_vdd3_adc_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_pd |= (1 << 13);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_pd &= ~(1 << 13);
		}
}
//----------------------------
// PD0 LDO1 DIG high current mode

void cobra_aon_pmu_d_ldo_pd0dig1_transient(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_vreg_trim |= 0x01;
		}
		else
		{
				cobra_aon_pmu->aon_ldo_vreg_trim &= ~0x01;
		}
}

/*

PD1 LDO1 DIG high current mode
*/
void cobra_aon_pmu_d_ldo_pd1dig1_transient(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_vreg_trim |= (1 << 1);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_vreg_trim &= ~(1 << 1);
		}	
}

/*
PD1 LDO2  high current mode
*/
void cobra_aon_pmu_d_ldo_pd1dig2_transient(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_ldo_vreg_trim |= (1 << 2);
		}
		else
		{
				cobra_aon_pmu->aon_ldo_vreg_trim &= ~(1 << 2);
		}	
}

/*
PD0 LDO DIG1 4 bit output voltage control: 0x0 = 0.6V, 0xF = 1.3
*/

void cobra_aon_pmu_d_ldo_pd0dig1_vreg_trim(unsigned int trim_value)
{
		cobra_aon_pmu->aon_ldo_vreg_trim &= ~(0x0f << 4);
		cobra_aon_pmu->aon_ldo_vreg_trim |= ((trim_value&0x0f) << 4);
}

/*
PD1 LDO DIG1 4 bit output voltage control: 0x0 = 0.6V, 0xF = 1.3
*/

void cobra_aon_pmu_d_ldo_pd1dig1_vreg_trim(unsigned int trim_value)
{
		cobra_aon_pmu->aon_ldo_vreg_trim &= ~(0x0f << 8);
		cobra_aon_pmu->aon_ldo_vreg_trim |= ((trim_value&0x0f) << 8);
}

/*
PD1 LDO DIG2 4 bit output voltage control: 0x0 = 0.6V, 0xF = 1.3
*/
void cobra_aon_pmu_d_ldo_pd1dig2_vreg_trim(unsigned int trim_value)
{
		cobra_aon_pmu->aon_ldo_vreg_trim &= ~(0x0f << 12);
		cobra_aon_pmu->aon_ldo_vreg_trim |= ((trim_value&0x0f) << 12);
}

/*
蓝牙低功耗选择的时钟
lf clock source selection
00: rcosc32k, 01:xtal48m, 10: rcosc8m, 11:xtal32k

*/
void cobra_aon_pmu_aon_lf_clk_cfg(AON_LF_CLK_SOURCE source_index)
{
		volatile unsigned int temp = cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg;
	
		if(source_index == LF_CLK_RCOSC32K)
		{
				temp &= ~0x03;
				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg = temp;
				//cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg &= ~0x03;
		}
		else if(source_index == LF_CLK_XTAL48M)
		{
					temp &= ~0x03;
					temp |= 0x01;
					cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg = temp;
//				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg &= ~0x03;
//				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg |= 0x01;
		}
		else if(source_index == LF_CLK_RCOSC8M)
		{
					temp &= ~0x03;
					temp |= 0x02;
					cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg = temp;
//				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg &= ~0x03;
//				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg |= 0x02;
		}
		else if(source_index == LF_CLK_XTAL32K)
		{
					//temp &= ~0x03;
					temp |= 0x03;
					cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg = temp;
//				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg |= 0x03;
		}
}

/*
gpadc 8M clock source selection 
"2'b11配置是禁止的。时钟源的切换保护由软件完成,
时钟源切换时必须有全关后再开另一时钟源。"

*/
void cobra_aon_pmu_aon_gp_adc_clk_cgf(AON_GPADC_CLK_SOURCE source)
{
		volatile unsigned int temp = cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg;
		if(source == GPADC_CLK_RCOSC8M)
		{
				temp &= ~(0x03 << 2);
				temp |= (0x01 << 2);
//				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg &= ~(0x03 << 2);
//				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg |= (0x01 << 2);
				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg = temp;
		}
		else if(source == GPADC_CLK_XTAL48M)
		{
//				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg &= ~(0x03 << 2);
//				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg |= (0x02 << 2);
				temp &= ~(0x03 << 2);
				temp |= (0x02 << 2);
				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg = temp;
		}
	
}

/*
aon ahb clock divider

aon_hclk = hf_clk/(aon_hclki_cfg+1)
当hf_clk选择为xtal48m,aon_hclki_cfg不能配置为0
*/
void cobra_aon_pmu_aon_hclki_cfg_divider(unsigned int  divider)
{
		volatile unsigned int temp = cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg;
		temp &= ~(0x1f << 8);
		temp |= ((divider & 0x1f) << 8);
		cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg = temp;
}

/*
apb clock divider

0: aon_hclk,  1: aon_hclk/2,
2: aon_hclk/3,3: aon_hclk/4
*/

void cobra_aon_pmu_aon_pclk_cfg_divider(unsigned int  divider)
{
		volatile unsigned int temp = cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg;
		temp &= ~(0x3 << 14);
		temp |= ((divider & 0x3) << 14);
		cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg = temp;
}

/*
aon_hclk selection
*/
void cobra_aon_pmu_aon_hclk_sel(AON_HCLK_SOURCE source)
{
		if(source == AON_HCLKI)
		{
				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg &= ~(1 << 16);
		}
		else if(source == CSEM_CLK)
		{
				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg |= (1 << 16);
		}
}
/*
hf clock source selection
0: rcosc8m, 1: xtal48m
*/
void cobra_aon_pmu_aon_hf_clk_cfg(AON_HF_CLK_SOURCE source_index)
{
		if(source_index == HF_CLK_RCOSC8M)
		{
				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg &= ~(0x01 << 4);
		}
		else if(source_index == HF_CLK_XTAL48M)
		{
				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg |=  (0x01 << 4);
		}
}

AON_HF_CLK_SOURCE cobra_aon_pmu_clk_source(void)
{
		if(cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg & (1 << 4))
		{
				return HF_CLK_XTAL48M;
		}
				return HF_CLK_RCOSC8M;
		
}

/*
csem 32k clock source selection
0: rcosc32k, 1: xtal32k

*/
void cobra_aon_pmu_aon_csem_clk_cfg(AON_CSEM_32K_CLK_SOURCE source_index)
{
		if(source_index == CSEM_RCOSC32K)
		{
				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg &= ~(0x01 << 8);
		}
		else if(source_index == CSEM_XTAL32K)
		{
				cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg |=  (0x01 << 8);
		}
}

/*
aon apb hclk divider
00/01: hf_clk; 10: hf_clk/8, 11: hf_clk/16

*/
void cobra_aon_pmu_aon_apb_hclk_divider(unsigned int divider)
{
		cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg &= ~(0x03 << 12);
		cobra_aon_pmu->aon_lf_hf_apb_hclk_cfg |=  ((0x03&divider)  << 12);
}


//open rcos8m
void cobra_aon_pmu_aon_rcosc8m_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_rcos8m_32k_cfg |= 0x03;
		}
		else
		{
				cobra_aon_pmu->aon_rcos8m_32k_cfg |= 0x02;
				cobra_aon_pmu->aon_rcos8m_32k_cfg &= ~0x01;
		}
		while(cobra_aon_pmu->aon_rcos8m_32k_cfg & (1<<9));   //wait feedback		
		
		cobra_aon_pmu->aon_rcos8m_32k_cfg &= ~0x02;
}

//open rcos32k
void cobra_aon_pmu_aon_rcosc32k_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_rcos8m_32k_cfg |= (0x03 << 2);
		}
		else
		{
				cobra_aon_pmu->aon_rcos8m_32k_cfg |= (1<<3);
				cobra_aon_pmu->aon_rcos8m_32k_cfg &= ~(0x01 << 2);
		}
		while(cobra_aon_pmu->aon_rcos8m_32k_cfg & (1<<11));   //wait feedback		
		
		cobra_aon_pmu->aon_rcos8m_32k_cfg &= ~(1<<3);
}


//open xtal32k
void cobra_aon_pmu_aon_xtal32k_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_rcos8m_32k_cfg |= (0x03 << 4);
		}
		else
		{
				cobra_aon_pmu->aon_rcos8m_32k_cfg |= (1<<5);
				cobra_aon_pmu->aon_rcos8m_32k_cfg &= ~(0x01 << 4);
		}
		while(cobra_aon_pmu->aon_rcos8m_32k_cfg & (1<<13));   //wait feedback		
		
		cobra_aon_pmu->aon_rcos8m_32k_cfg &= ~(1<<5);
}

/*
irda pclk enable
rtc pclk enable
enclica rtc clk mask enable
ucpu hclk enable
timer2 pclk enable
gpadc pclk enable


irda presetn, active low
rtc presetn, active low

ucpu presetn, active low
timer2 presetn, active low
gpadc presetn, active low



*/
void cobra_aon_pmu_irda_pclk_enable(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl |= 0x01;
			}
			else
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl &= ~0x01;
			}
}

void cobra_aon_pmu_rtc_pclk_enable(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl |= 0x02;
			}
			else
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl &= ~0x02;
			}
}

void cobra_aon_pmu_enclica_rtc_clk_mask_enable(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl |= 0x04;
			}
			else
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl &= ~0x04;
			}
}

void cobra_aon_pmu_ucpu_hclk_enable(unsigned int en)
{
			unsigned int temp;
			temp = cobra_aon_pmu->reserver2;
	
			if(en)
			{
					temp |= 0x08;
			}
			else
			{
					temp &= ~0x08;
			}
			
			cobra_aon_pmu->aon_pclk_reset_ctrl = temp;
}

void cobra_aon_pmu_timer2_pclk_enable(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl |= 0x10;
			}
			else
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl &= ~0x10;
			}
}

void cobra_aon_pmu_gpadc_pclk_enable(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl |= 0x20;
			}
			else
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl &= ~0x20;
			}
}


/*

irda presetn, active low

*/
void cobra_aon_pmu_irda_presetn(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl &= ~(1 << 8);
			}
			else
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl |= (1 << 8);
			}
}

/*

rtc presetn, active low

*/
void cobra_aon_pmu_rtc_presetn(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl &= ~(1 << 9);
			}
			else
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl |= (1 << 9);
			}
}

/*

ucpu presetn, active low

*/
void cobra_aon_pmu_ucpu_presetn(unsigned int en)
{
			unsigned int temp;
			temp = cobra_aon_pmu->reserver2;
			if(en)
			{
					temp &= ~(1 << 11);
			}
			else
			{
					temp |= (1 << 11);
			}
			
			cobra_aon_pmu->aon_pclk_reset_ctrl = temp;
}

/*

timer2 presetn, active low

*/
void cobra_aon_pmu_timer2_presetn(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl &= ~(1 << 12);
			}
			else
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl |= (1 << 12);
			}
}

/*

gpadc presetn, active low

*/
void cobra_aon_pmu_gpadc_presetn(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl &= ~(1 << 13);
			}
			else
			{
					cobra_aon_pmu->aon_pclk_reset_ctrl |= (1 << 13);
			}
}

/*
?aon_hard_pd_ctrl== 0xA3?,???????????aon????

?aon_hard_pd_ctrl== 0xA3?,???????????aon????

硬件控制序列
*/

void cobra_aon_pmu_aon_hard_pd_ctrl(unsigned int en)
{
			if(en)
			{
					cobra_aon_pmu->aon_hard_pd_ctrl_state &= ~(0xff);
					cobra_aon_pmu->aon_hard_pd_ctrl_state |= 0x00;
			}
			else
			{
				//不再 AON  硬件控制下电了
					cobra_aon_pmu->aon_hard_pd_ctrl_state &= ~(0xff);
					cobra_aon_pmu->aon_hard_pd_ctrl_state |= 0xA3;
			}
}

/*
0: POWER OFF, 1/2/3:POWER????
0: POWER ON, 1/2/3:POWER????


*/
unsigned int cobra_aon_pmu_aon_state(void)
{
		return (cobra_aon_pmu->aon_hard_pd_ctrl_state >> 8)&0x7;
}

#if 0
///*
//hf clock test enable, 
//1: enable, 0: disable, connect to aon_pad[0] 


//*/
//void cobra_aon_pmu_hf_clk_tst_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_aon_pmu->aon_clk_test |= (1 << 2);
//		}
//		else
//		{
//				cobra_aon_pmu->aon_clk_test &= ~(1 << 2);
//		}
//}

///*
//32k rcosc clock test enable,
//1: enable, 0: disable, connect to aon_pad[1]


//*/
//void cobra_aon_pmu_rcosc_32k_tst_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_aon_pmu->aon_clk_test |= (1 << 3);
//		}
//		else
//		{
//				cobra_aon_pmu->aon_clk_test &= ~(1 << 3);
//		}
//}

///*
//32k xtal clock test enable,
//1: enable, 0: disable, connect to aon_pad[2]

//*/
//void cobra_aon_pmu_xtal_32k_tst_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_aon_pmu->aon_clk_test |= (1 << 4);
//		}
//		else
//		{
//				cobra_aon_pmu->aon_clk_test &= ~(1 << 4);
//		}
//}

#endif

/*

hf clock gate enable
0: hk clock is gated, 1: hf clock is passed

*/
void cobra_aon_pmu_hf_clk_gate_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_clk_gate |= (1 << 0);
		}
		else
		{
				cobra_aon_pmu->aon_clk_gate &= ~(1 << 0);
		}
}

/*
lf clock gate enable
0: lk clock is gated, 1: lf clock is passed

*/
void cobra_aon_pmu_lf_clk_gate_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_clk_gate |= (1 << 1);
		}
		else
		{
				cobra_aon_pmu->aon_clk_gate &= ~(1 << 1);
		}
}

/*
va clock gate enable
0: lk clock is gated, 1: lf clock is passed

voice wake up 
*/
void cobra_aon_pmu_va_clock_gate_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_clk_gate |= (1 << 2);
		}
		else
		{
				cobra_aon_pmu->aon_clk_gate &= ~(1 << 2);
		}
}


/*
gp clock gate enable
0: lk clock is gated, 1: lf clock is passed

genernal adc clock
*/
void cobra_aon_pmu_gp_clock_gate_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_clk_gate |= (1 << 3);
		}
		else
		{
				cobra_aon_pmu->aon_clk_gate &= ~(1 << 3);
		}
}



#if 0
///*

//irda pclk enable

//*/
//void cobra_aon_pmu_irda_pclk_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_aon_pmu->aon_clk_gate |= (1 << 4);
//		}
//		else
//		{
//				cobra_aon_pmu->aon_clk_gate &= ~(1 << 4);
//		}
//}

///*

//irda rtc enable

//*/
//void cobra_aon_pmu_rtc_pclk_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_aon_pmu->aon_clk_gate |= (1 << 5);
//		}
//		else
//		{
//				cobra_aon_pmu->aon_clk_gate &= ~(1 << 5);
//		}
//}

///*
//enclica clk mask enable

//*/
//void cobra_aon_pmu_enclica_clk_mask_enable(unsigned int en)
//{
//		if(en)
//		{
//				cobra_aon_pmu->aon_clk_gate |= (1 << 6);
//		}
//		else
//		{
//				cobra_aon_pmu->aon_clk_gate &= ~(1 << 6);
//		}
//}

#endif

/*

standby mode enable

*/
void cobra_aon_pmu_standby_mode_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_pmu_mode_enable |= (1 << 0);
		}
		else
		{
				cobra_aon_pmu->aon_pmu_mode_enable &= ~(1 << 0);
		}
}

/*

shutdown mode enable


*/
void cobra_aon_pmu_shutdown_mode_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_pmu_mode_enable |= (1 << 1);
		}
		else
		{
				cobra_aon_pmu->aon_pmu_mode_enable &= ~(1 << 1);
		}
}

/*

wakeup reset enable

*/
void cobra_aon_pmu_wakeup_reset_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_pmu_mode_enable |= (1 << 2);
		}
		else
		{
				cobra_aon_pmu->aon_pmu_mode_enable &= ~(1 << 2);
		}
}


void cobra_aon_d_xo_32k_cl(unsigned int value)
{
		cobra_aon_pmu->d_xo_32k_cl_adc_en &= ~0x0f;
		cobra_aon_pmu->d_xo_32k_cl_adc_en |= (value&0x0f);
}


/*
d_vdd3_adc_en

*/
void cobra_aon_d_vdd3_adc_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->d_xo_32k_cl_adc_en |= (1 << 7);
		}
		else
		{
				cobra_aon_pmu->d_xo_32k_cl_adc_en &= ~(1 << 7);
		}
}

void cobra_an_d_rco8m_ftune_set(unsigned char value)
{
		unsigned int temp;
		temp = cobra_aon_pmu->aon_cmp_32k_rsoc8m_ftune;
		temp &= ~0xff;
		temp |= value;
		cobra_aon_pmu->aon_cmp_32k_rsoc8m_ftune = temp;	
}

unsigned char cobra_an_d_rco8m_ftune_read(void)
{
		return cobra_aon_pmu->aon_cmp_32k_rsoc8m_ftune&0xff;
}

/*
when analog enable, the digital IE/OE should be disable

analog pin enable for aon_agpio[0],active high
*/
void cobra_aon_analog_pin0_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_aonpad_ana_spare |= 0x01;
		}
		else
		{
				cobra_aon_pmu->aon_aonpad_ana_spare &= ~0x01;
		}	
}
	
void cobra_aon_analog_pin1_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_aonpad_ana_spare |= 0x02;
		}
		else
		{
				cobra_aon_pmu->aon_aonpad_ana_spare &= ~0x02;
		}	
}

void cobra_aon_analog_pin2_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_aonpad_ana_spare |= 0x04;
		}
		else
		{
				cobra_aon_pmu->aon_aonpad_ana_spare &= ~0x04;
		}	
}

void cobra_aon_analog_pin3_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_aonpad_ana_spare |= 0x08;
		}
		else
		{
				cobra_aon_pmu->aon_aonpad_ana_spare &= ~0x08;
		}	
}

void cobra_aon_analog_pin4_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_aonpad_ana_spare |= 0x010;
		}
		else
		{
				cobra_aon_pmu->aon_aonpad_ana_spare &= ~0x010;
		}	
}

//--------------------------------------------hardware-------------------------------------
///*
//battery and temperature monitor enable
//1: enable, 0: disable

//*/
//void cobra_aon_pmu_d_bmtm_monitor_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_aon_pmu->aon_battery_temperature_adc_cfg |= (1 << 0);
//		}
//		else
//		{
//				cobra_aon_pmu->aon_battery_temperature_adc_cfg &= ~(1 << 0);
//		}
//}

///*

//d_bmtm_adc_sd_en
//1: battery mode, 0: temperature mode

//*/
//void cobra_aon_pmu_d_bmtm_adc_sd_en(ADC_MONITOR_MODE mode)
//{
//		if(mode == BATTERY_MODE)
//		{
//				cobra_aon_pmu->aon_battery_temperature_adc_cfg |= (1 << 1);
//		}
//		else if(mode == TEMPERATURE_MODE)
//		{
//				cobra_aon_pmu->aon_battery_temperature_adc_cfg &= ~(1 << 1);
//		}
//}

///*
//bmtm adc data enable

//*/
//void cobra_aon_pmu_d_bmtm_adc_data_en(unsigned int en)
//{
//		if(en)
//		{
//				cobra_aon_pmu->aon_battery_temperature_adc_cfg |= (1 << 3);
//		}
//		else
//		{
//				cobra_aon_pmu->aon_battery_temperature_adc_cfg &= ~(1 << 3);
//		}
//}

///*

//bmtm adc data

//*/
//unsigned int cobra_aon_pmu_d_bmtm_adc_out(void)
//{
//		return (cobra_aon_pmu->aon_battery_temperature_adc_cfg >> 4)&0xff;
//}

void cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG_INDEX seq_index, unsigned short address, unsigned short data, bool last_cmd)
{
		unsigned int temp_data = 0;
		temp_data |= data;
		temp_data |= ((unsigned int)(address&0x1ff) << 16);
		if(last_cmd)
		{
				temp_data |= (1<<25);
		}
		switch(seq_index)
		{
			case AON_SEQ_CFG0:
					cobra_aon_pmu->aon_sec_cfg0 = temp_data;
					break;
			case AON_SEQ_CFG1:
					cobra_aon_pmu->aon_sec_cfg1 = temp_data;
					break;
			case AON_SEQ_CFG2:
					cobra_aon_pmu->aon_sec_cfg2 = temp_data;
					break;
			case AON_SEQ_CFG3:
					cobra_aon_pmu->aon_sec_cfg3 = temp_data;
					break;
			case AON_SEQ_CFG4:
					cobra_aon_pmu->aon_sec_cfg4 = temp_data;
					break;
			case AON_SEQ_CFG5:
					cobra_aon_pmu->aon_sec_cfg5 = temp_data;
					break;
			case AON_SEQ_CFG6:
					cobra_aon_pmu->aon_sec_cfg6 = temp_data;
					break;
			case AON_SEQ_CFG7:
					cobra_aon_pmu->aon_sec_cfg7 = temp_data;
					break;
			default:
					break;
		}
		
}

//AON IO0
/*
io0 posedge detect enable

上升边缘触发

0: disable, 1: enable


*/
void cobra_aon_pmu_io0_posedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= 0x01;
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~0x01;
		}
}	

/*
io0 negedge detect enable

下降缘 触发

0: disable, 1: enable


*/

void cobra_aon_pmu_io0_negedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= 0x02;
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~0x02;
		}
}	

//AON IO1
/*
io1 posedge detect enable

上升边缘触发

0: disable, 1: enable


*/
void cobra_aon_pmu_io1_posedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 2);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1<<2);
		}
}	

/*
io1 negedge detect enable

下降缘 触发

0: disable, 1: enable


*/

void cobra_aon_pmu_io1_negedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 3);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 3);
		}
}	

//AON IO2
/*
io2 posedge detect enable

上升边缘触发

0: disable, 1: enable


*/
void cobra_aon_pmu_io2_posedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 4);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 4);
		}
}	

/*
io2 negedge detect enable

下降缘 触发

0: disable, 1: enable


*/

void cobra_aon_pmu_io2_negedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 5);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 5);
		}
}	

//AON IO3
/*
io3 posedge detect enable

上升边缘触发

0: disable, 1: enable


*/
void cobra_aon_pmu_io3_posedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 6);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 6);
		}
}	

/*
io3 negedge detect enable

下降缘 触发

0: disable, 1: enable


*/

void cobra_aon_pmu_io3_negedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 7);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 7);
		}
}	


//AON IO4
/*
io4 posedge detect enable

上升边缘触发

0: disable, 1: enable


*/
void cobra_aon_pmu_io4_posedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 8);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 8);
		}
}	

/*
io4 negedge detect enable

下降缘 触发

0: disable, 1: enable


*/

void cobra_aon_pmu_io4_negedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 9);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 9);
		}
}	

/*
mcu_io posedge detect enable,active high
for hibernate mode, should be set to 0
整个 MCU 上升缘 触发 使能
*/
void cobra_aon_pmu_mcu_io_posedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 10);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 10);
		}
}

/*

mcu_io negedge detect enable,active high
for hibernate mode, should be set to 0

*/
void cobra_aon_pmu_mcu_io_negedge_detect_enable(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 11);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 11);
		}
}

/*
enable edge detect interrupt

1: enable edge irq, the toggle will be clear all interrupt


*/

void cobra_aon_pmu_edge_irq_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 14);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 14);
		}
}
/*

io signal latch enable
1: latch, 

*/
void cobra_aon_pmu_io_lat_en(unsigned int en)
{
		if(en)
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg |= (1 << 15);
		}
		else
		{
				cobra_aon_pmu->aon_io0_edge_det_cfg &= ~(1 << 15);
		}
}

/*
typedef enum {
		IO0_EDGE_INT_EVENT = 1,
		IO1_EDGE_INT_EVENT,
		IO2_EDGE_INT_EVENT,
		IO3_EDGE_INT_EVENT,
		IO4_EDGE_INT_EVENT,
	
		UMCU_INT_EVENT,
		RMT0_WAKEUP_EVENT,
		RMT0_INT_EVENT,
		RTC_INT_EVENT,
		RTC_ALARM0_EVENT,
		RTC_ALARM1_EVENT,
		VOICE_WAKEUP_EVENT,
		TIMER2_INT_EVENT,
		GPADC_INT_EVENT
	
}AON_EVENT_SOURCE;
5'h0: 
5'h1: IO0_edge_int
5'h2: IO1_edge_int
5'h3: IO2_edge_int
5'h4: IO3_edge_int
5'h5: IO4_edge_int

*/
void cobra_aon_pmu_wakeup_source0_selection(AON_EVENT_SOURCE selection)
{
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src1 &= ~0x1f;
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src1 |= selection;
}

void cobra_aon_pmu_wakeup_source1_selection(AON_EVENT_SOURCE selection)
{
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src1 &= ~(0x1f << 8);
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src1 |= (selection << 8);
}

void cobra_aon_pmu_wakeup_source2_selection(AON_EVENT_SOURCE selection)
{
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src1 &= ~(0x1f << 16);
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src1 |= (selection << 16);
}

/*
aon2mcu event source0 selection
aon2mcu_src0[0]
aon2mcu_src0[1]
aon2mcu_src0[2]
aon2mcu_src0[3]
aon2mcu_src0[4]


*/
void cobra_aon_pmu_aon2mcu_source0_selection(AON_EVENT_SOURCE selection)
{
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src1 &= ~(0x1f << 24);
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src1 |= (selection << 24);
}

/*

aon2mcu event source1 selection

*/
void cobra_aon_pmu_aon2mcu_source1_selection(AON_EVENT_SOURCE selection)
{
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src2 &= ~0x1f;
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src2 |= selection;
}

/*

aon2mcu event source2 selection

*/
void cobra_aon_pmu_aon2mcu_source2_selection(AON_EVENT_SOURCE selection)
{
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src2 &= ~(0x1f << 8);
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src2 |= (selection << 8);
}

/*

aon2mcu event source3 selection

*/
void cobra_aon_pmu_aon2mcu_source3_selection(AON_EVENT_SOURCE selection)
{
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src2 &= ~(0x1f << 16);
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src2 |= (selection << 16);
}


/*

aon2mcu event source4 selection

*/
void cobra_aon_pmu_aon2mcu_source4_selection(AON_EVENT_SOURCE selection)
{
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src2 &= ~(0x1f << 24);
		cobra_aon_pmu->aon_wakup_src_aon2mcu_src2 |= (selection << 24);
}




#endif

















