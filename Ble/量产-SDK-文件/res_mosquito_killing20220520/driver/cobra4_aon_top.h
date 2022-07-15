/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4_aon_top.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/6

	Description :   
        for cobra4 , modified power mananger
		APB0 0x4008B000
	Changed Log	:

		Liujian  2019/8/6			- Creation



*****************************************************************************/
#ifndef __COBRA4_AON_TOP_H__
#define __COBRA4_AON_TOP_H__
#include <stdio.h>
#include <cobra_config.h>
#if COBRA4_AON_PD_TOP_EN

typedef struct cobra4_aon_pd_top_t {
		__attribute__ ((aligned (4))) volatile unsigned int aon_config_protect;   //0x00
	
		__attribute__ ((aligned (4))) volatile unsigned int aon_lf_clk_control;
	
		__attribute__ ((aligned (4))) volatile unsigned int aon_wdt_cfg;
	
		__attribute__ ((aligned (4))) volatile unsigned int aon_hclk_sw;
	
	  __attribute__ ((aligned (4))) volatile unsigned int aon_hclk_cfg;        //0x10
	
		__attribute__ ((aligned (4))) volatile unsigned int aon_clk_gate_cfg; 
	
		__attribute__ ((aligned (4))) volatile unsigned int aon_pclk_en; 
	
		__attribute__ ((aligned (4))) volatile unsigned int aon_pclk_mask_cfg;

		__attribute__ ((aligned (4))) volatile unsigned int aon_psrst_en;       //0x20
	
		__attribute__ ((aligned (4))) volatile unsigned int aon_aux_clk_cfg;

		__attribute__ ((aligned (4))) volatile unsigned int fm_clk_measure;     //0x28
		
		__attribute__ ((aligned (4))) volatile unsigned int fm_clk_measure_count;  //0x2C
		
		__attribute__ ((aligned (4))) volatile unsigned int mcu_power_manager;  //0x30
		
		__attribute__ ((aligned (4))) volatile unsigned int aon_ucpu_event;  //0x34
		
		__attribute__ ((aligned (4))) volatile unsigned int aon_reserver0;
		__attribute__ ((aligned (4))) volatile unsigned int aon_reserver1;
		
		__attribute__ ((aligned (4))) volatile unsigned int aon_ana_cfg0;    //0x40
	  __attribute__ ((aligned (4))) volatile unsigned int aon_ana_cfg1;
	  __attribute__ ((aligned (4))) volatile unsigned int aon_ana_cfg2;
	  __attribute__ ((aligned (4))) volatile unsigned int aon_ana_cfg3;
		
		__attribute__ ((aligned (4))) volatile unsigned int aon_reserver2;  //0x50
		__attribute__ ((aligned (4))) volatile unsigned int aon_reserver3; 	
		
		__attribute__ ((aligned (4))) volatile unsigned int aon_pd_ana_state; 	   //0x058
		
		__attribute__ ((aligned (4))) volatile unsigned int aon2mcu_src0_src1_src2_src3;   //0x5c
		__attribute__ ((aligned (4))) volatile unsigned int aon2mcu_src4_src5;             //0x60
		
		__attribute__ ((aligned (4))) volatile unsigned int aon_spare0;               //0x64
		
		__attribute__ ((aligned (4))) volatile unsigned int aon_reserver4[64]; 
		
		
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg_step2;                  //0x168
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg_step3; 
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg_step4; 
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg_step5; 
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg_step6; 
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg_step7; 

		
		__attribute__ ((aligned (4))) unsigned int aon_io_edge_det_cfg;      //0x180
		
		__attribute__ ((aligned (4))) unsigned int aon_wakup_src_aon2mcu_src1;
		
		
		
}COBRA4_AON_PD_TOP_T;

typedef enum {
		LF_RC_OSC32K,
		LF_XTAL48M,
		LF_RC_OSC24M,
		LF_XTAL_32K	
}AON_LF_CLK_SOURCE;


typedef enum {
		HF_RC_OSC24M,
		HF_XTAL48M	
}AON_HF_CLK_SOURCE;

typedef enum {
	HCLK_RC_OSC24M,
	HCLK_XTAL48M,
	HCLK_CSEM32K,
	HCLK_DISABLED
	
}AON_HCLK_SOURCE;

typedef enum {
	GPADC_CLK_DISABLE,
	GPADC_RC_OSC24M,
	GPADC_XTAL48M
}GPADC_CLK_SOURCE;

typedef enum{
	TDC_CLK_DISABLE,
	TDC_RC_OSC24M,
	TDC_XTAL48M	
}TDC_CLK_SOURCE;


typedef enum{
	MEASURE_RC_OSC_32K,
	MEASURE_XTAL_32K,
	MEASURE_RC_OSC_24M,
	MEASURE_HF_CLK,
	MEASURE_AON_HCLK
}MEASURE_CLK_SOURCE;

typedef enum {
	AON_NO_SOURCE,
	AON_UMCU_INT,
	AON_DTIMER2B,
	AON_DTIMER2A,
	AON_RTC_INT0,   // all RTC interrupt
	AON_RTC_INT1,   // alarm0
	AON_RTC_INT2,   // alarm1
	AON_VA_WAKEUP,
	AON_DTIMER2,
	AON_GPADC_INT,
	AON_TDC_DONE,
	AON_BLEBB_RADIO_EN,
	AON_BLEBB_OSC_EN,
	AON_I_CMPB	
}AON_EVENT_SOURCE;


#ifdef COBRA_AON_TOP_NEW
#define		AON_CONFIG_PROTECT			0x0000
#define		AON_HF_CLK_CFG	        0x10UL		
#define 	AON_AHB_CLK_CFG_MASK		(0x1f<<0)
#define   AON_ANA_CFG0					0x0040
#define   D2A_VDD3_ADC_EN_MASK			(1<<3)
#define   AON_ANA_CFG2					0x0048
#define   AON_ANA_CFG1					0x0044
#define   D2A_BTGPADC_MASK			(0x7FFE<<13)
#define   AON_ANA_CFG3					0x004C

#endif

#if (COBRA_ADC_EN_VERSION2)
typedef enum{
	DIFF_POS_DPIN31,
	DIFF_POS_DPIN30,
	DIFF_POS_DPIN29,
	DIFF_POS_DPIN28,
	DIFF_POS_DPIN27,
	DIFF_POS_DPIN26,
	DIFF_POS_DPIN25,
	DIFF_POS_DPIN24,
	DIFF_POS_DPIN_MAX,
}DIFFERENTIAL_POS_INPUT_DPIN;


typedef enum{
	DIFF_NEG_DPIN31,
	DIFF_NEG_DPIN30,
	DIFF_NEG_DPIN29,
	DIFF_NEG_DPIN28,
	DIFF_NEG_DPIN27,
	DIFF_NEG_DPIN26,
	DIFF_NEG_DPIN25,
	DIFF_NEG_DPIN24,
	DIFF_NEG_DPIN_MAX,
}DIFFERENTIAL_NEG_INPUT_DPIN;

typedef enum {
	 ADC_MODE_TM,
	 ADC_MODE_BM,
	 ADC_MODE_NORMAL,
	 ADC_MODE_MAX,	
}GPADC_MODE;

typedef enum {
	 ADC_SINGLE_END_MODE,
	 ADC_DIFFERENTIAL_MODE,
}DIFF_MODE;

typedef enum {
	 ADC_GAIN_1X,
	 ADC_GAIN_4X,
	 ADC_GAIN_2X,
	 ADC_GAIN_DIV_4,
	 ADC_GAIN_DIV_2,
}ADC_GAIN;
#endif


#define COBRA4_AON_PD_TOP_BASE				(0x4008B000UL)


void cobra4_aon_pd_top_init(void);
void cobra4_aon_pd_top_protection_en(unsigned int en);
/*
ucpu软复位请求，硬件自清除
1：软复位
*/
void cobra4_aon_pd_top_umcu_srst_req_en(unsigned int  en);

/*
gpadc 8M时钟源选择
[1:0]： 
0x，gpadc 8M时钟关闭
10:时钟源为rcosc24M, 11:时钟源为xtal48M


*/
void cobra4_aon_pd_top_gpadc_clock_selection(GPADC_CLK_SOURCE source);


/*
aon config register write protect enable

"0: write diable
 1: write enable"

*/
void cobra4_aon_pd_top_protection_en(unsigned int en);

AON_HF_CLK_SOURCE cobra4_aon_pd_top_hf_clock_source(void);

/*
hf clock source selection
0: rcosc8m, 1: xtal48m


*/
void cobra4_aon_pd_top_hf_clock_selection(AON_HF_CLK_SOURCE source);
void cobra4_aon_pd_top_lf_clock_selection(AON_LF_CLK_SOURCE source);
void cobra4_aon_pd_top_aon_hclk_clock_selection(AON_HCLK_SOURCE source);

void cobra4_aon_pd_top_watchdog_freq_div(unsigned int value);
void cobra4_aon_pd_top_watchdog_freq_enable(unsigned int en);
void cobra4_aon_pd_top_aon_hclk_divider(unsigned int  divider);
void cobra4_aon_pd_top_aon_pclk_divider(unsigned int  divider);

void cobra4_aon_pd_top_aon2mcu_source0_selection(AON_EVENT_SOURCE selection);
void cobra4_aon_pd_top_aon2mcu_source1_selection(AON_EVENT_SOURCE selection);
void cobra4_aon_pd_top_aon2mcu_source2_selection(AON_EVENT_SOURCE selection);
void cobra4_aon_pd_top_aon2mcu_source3_selection(AON_EVENT_SOURCE selection);
void cobra4_aon_pd_top_aon2mcu_source4_selection(AON_EVENT_SOURCE selection);
void cobra4_aon_pd_top_aon2mcu_source5_selection(AON_EVENT_SOURCE selection);



int cobra4_aon_pd_top_aon2mcu_source3_get(void);


void cobra4_aon_pd_top_umcu_srst_req_en(unsigned int  en);
void cobra4_aon_pd_top_csem_rf_presetn_en(unsigned int  en);

/*
rtc0 pclk en
*/
void cobra4_aon_pd_top_rtc0_pclk_en(unsigned int en);
/*
ext3 ( CSEM RF ) pclk en
*/
void cobra4_aon_pd_top_csem_rf_pclk_en(unsigned int en);
/*
ucpu pclk en
小 MCU 的控制
*/
void cobra4_aon_pd_top_ucpu_pclk_en(unsigned int en);
/*
dualtimers pclk en

*/
void cobra4_aon_pd_top_dualtimers_pclk_en(unsigned int en);
/*
watchdog pclk en

*/
void cobra4_aon_pd_top_watchdog_pclk_en(unsigned int en);
/*
ext1 (dc_dc pmu) pclk en

*/
void cobra4_aon_pd_top_dcdc_pmu_pclk_en(unsigned int en);
/*
ext0 (pinmux ) pclk en

*/
void cobra4_aon_pd_top_pinmux_pclk_en(unsigned int en);

void cobra4_aon_pd_top_rtc0_clk_en(unsigned int en);

void cobra4_aon_pd_top_icytrx_reset_en(unsigned int  en);

//gate clock
void cobra4_aon_pd_top_an_hf_clk_gate_en(unsigned int  en);
void cobra4_aon_pd_top_an_lf_clk_gate_en(unsigned int  en);
void cobra4_aon_pd_top_an_low_power_clk_gate_en(unsigned int  en);
void cobra4_aon_pd_top_an_clk_gpadc_32k_gate_en(unsigned int  en);
void cobra4_aon_pd_top_an_low_power_clk_gate_en(unsigned int  en);
void cobra4_aon_pd_top_rtc_gclk_gate_en(unsigned int  en);

void cobra4_aon_pd_top_rtc0_pclk_en(unsigned int en);
void cobra4_aon_pd_top_rtc0_clk_en(unsigned int en);
void cobra4_aon_pd_top_csem_rf_pclk_en(unsigned int en);
void cobra4_aon_pd_top_ucpu_pclk_en(unsigned int en);
void cobra4_aon_pd_top_dualtimers_pclk_en(unsigned int en);
void cobra4_aon_pd_top_watchdog_pclk_en(unsigned int en);

/*

umcu mask control
*/
void cobra4_aon_pd_top_ucpu_mask_en(unsigned int  en);
void cobra4_aon_pd_top_lf_rstn_en(unsigned int en);

void cobra4_aon_pd_top_gpadc_32k_enable(unsigned int en);

void cobra4_aon_pd_top_watchdog_presetn_en(unsigned int  en);

#if (COBRA_ADC_EN)
/*
switch on sw_adc3v3

*/
void cobra4_aon_pd_top_d2a_vdd3_adc_en(unsigned int en);

void cobra4_aon_pd_top_xtal32_trim(unsigned char value);

void cobra4_aon_pd_top_d2a_btgpadc_vpin_sel(unsigned char vpin_sel);

void cobra4_aon_pd_top_d2a_btgpadc_diff(unsigned char btgpadc_diff);

/*
d2a_btgapadc_en_gpadc

*/
void cobra4_aon_pd_top_d2a_btgapadc_en_gpadc(unsigned int en);

/*
d2a_btgapadc_en_gpadc

*/
void cobra4_aon_pd_top_d2a_btgpadc_en_t2vc(unsigned int en);

/*
btgpadc_mode

*/
void cobra4_aon_pd_top_d2a_btgpadc_mode (unsigned char btgpadc_mode);
/*
d2a_vr3v3to1v8_ldoadc_en

*/
void cobra4_aon_pd_top_d2a_vr3v3to1v8_ldoadc_en(unsigned int en);

/*
d2a_vr3v3to1v8_1v2ref_sel

*/
void cobra4_aon_pd_top_d2a_vr3v3to1v8_1v2ref_sel(unsigned int en);

/*
d2a_btgpadc_adcgain

*/
void cobra4_aon_pd_top_d2a_btgpadc_adcgain(unsigned int gain);

#endif

////////////////////////////////////////////////////////////////////////////////////////////////
#if (COBRA_ADC_EN_VERSION2)
//void cobra4_aon_pd_top_xtal32_trim(unsigned char value);

////////////////////////////
void cobra4_aon_pd_top_d2a_vdac_en(unsigned int en);

//compare A enable
void cobra4_aon_pd_top_d2a_cpa_en(unsigned int en);

//compare B enable
void cobra4_aon_pd_top_d2a_cpb_en(unsigned int en);

//vdd connect adc switch enable
void cobra4_aon_pd_top_d2a_vdd3_adc_en(unsigned int en);

////ldo_pd0 transient
//void cobra4_aon_pd_top_d2a_ldodig_pd0_high_curr_en(unsigned int en);

////ldo_pd0 trim
//void cobra4_aon_pd_top_d2a_ldodig_pd0_vreg_trim(unsigned int value);

////ldo_pd1 transient
//void cobra4_aon_pd_top_d2a_ldodig_pd1_high_curr_en(unsigned int en);

////ldo_pd1 trim
//void cobra4_aon_pd_top_d2a_ldodig_pd1_vreg_trim(unsigned int value);

//d2a_efbrwot_en
void cobra4_aon_pd_top_d2a_efbrwot_en(unsigned int en);

////d2a_bgln_t_trim
//void cobra4_aon_pd_top_d2a_bgln_t_trim(unsigned int value);

////d2a_bgln_vref_trim
//void cobra4_aon_pd_top_d2a_bgln_vref_trim(unsigned int value);

//d2a_ldoanaforpll_high_curr
void cobra4_aon_pd_d2a_ldoanaforpll_high_curr_en(unsigned int en);

////d2a_ldoanaforpll_trim
//void cobra4_aon_pd_top_d2a_ldoanaforpll_trim(unsigned int value);

///////////////////////
////d2a_xo32k_gmtrim
//void cobra4_aon_pd_top_d2a_xo32k_gmtrim(unsigned int value);

//d2a_xo32k_cl
void cobra4_aon_pd_top_d2a_xo32k_cl(unsigned int value);

//d2a_rco24m_ftune
void cobra4_aon_pd_top_d2a_rco24m_ftune(unsigned int value);

//d2a_btgpadc_vpin_sel
void cobra4_aon_pd_top_d2a_btgpadc_vpin_sel(DIFFERENTIAL_POS_INPUT_DPIN input_pin);

//d2a_btgpadc_diff
void cobra4_aon_pd_top_d2a_btgpadc_diff(DIFFERENTIAL_NEG_INPUT_DPIN input_pin, unsigned int diff_mode_en);

//d2a_btgapadc_en_gpadc

void cobra4_aon_pd_top_d2a_btgapadc_en_gpadc_en(unsigned int en);

//d2a_btgpadc_en_t2vc
void cobra4_aon_pd_top_d2a_btgpadc_en_t2vc_en(unsigned int en);

//d2a_btgpadc_mode
void cobra4_aon_pd_top_d2a_btgpadc_mode(GPADC_MODE mode);

//d2a_cpb_dvider_sel
void cobra4_aon_pd_top_d2a_cpb_dvider_sel(unsigned int value);

//d2a_cpa_out_reverse
void cobra4_aon_pd_top_d2a_cpa_out_reverse_en(unsigned int en);

//d2a_xo32k_cnt
void cobra4_aon_pd_top_d2a_xo32k_cnt(unsigned int count);

//////////////////////////////////
//analog vdd18 enable

void cobra4_aon_pd_top_d2a_vr3v3to1v8_ldoadc_en(unsigned int en);

//analog vdd18 vref sel
void cobra4_aon_pd_top_d2a_vr3v3to1v8_1v2ref_sel(unsigned int en);

//analog vdd18 bgpass
void cobra4_aon_pd_top_d2a_vr3v3to1v8_force_bypass(unsigned int en);

////analog vdd18 voltage trim
//void cobra4_aon_pd_top_d2a_vr3v3to1v8_votrim(unsigned int value);

//d2a_btgpadc_adccal
void cobra4_aon_pd_top_d2a_btgpadc_adccal(unsigned int value);

//d2a_btgpadc_32k_mode
void cobra4_aon_pd_top_d2a_btgpadc_32k_mode_en(unsigned int en);

//d2a_cpa_neg_sel
void cobra4_aon_pd_top_d2a_cpa_neg_sel(unsigned int sel);

//d2a_cpa_pos_sel
void cobra4_aon_pd_top_d2a_cpa_pos_sel(unsigned int sel);

//d2a_vdac_setx
void cobra4_aon_pd_top_d2a_vdac_setx(unsigned int value);

//d2a_vdac_sety
void cobra4_aon_pd_top_d2a_vdac_sety(unsigned int value);

///////////////////////////
//d2a_vdac_setz
void cobra4_aon_pd_top_d2a_vdac_setz(unsigned int value);

//d2a_cpb_neg_sel
void cobra4_aon_pd_top_d2a_cpb_neg_sel(unsigned int sel);
//d2a_cpb_pos_sel
void cobra4_aon_pd_top_d2a_cpb_pos_sel(unsigned int sel);
//d2a_btgpadc_adcgain
void cobra4_aon_pd_top_d2a_btgpadc_adcgain(unsigned int gain);
//d2a_anatmux_en
void cobra4_aon_pd_top_d2a_anatmux_en(unsigned int en);
//d2a_anatmux_sel
void cobra4_aon_pd_top_d2a_anatmux_sel(unsigned int sel);
//d2a_aon_tmux_en
void cobra4_aon_pd_top_d2a_aon_tmux_en(unsigned int en);
//d2a_aon_tmux_sel
void cobra4_aon_pd_top_d2a_aon_tmux_sel(unsigned int sel);

#endif

void cobra4_aon_pd_top_xtal32_trim(unsigned char value);

//ldo_pd0 transient
void cobra4_aon_pd_top_d2a_ldodig_pd0_high_curr_en(unsigned int en);

//ldo_pd0 trim
void cobra4_aon_pd_top_d2a_ldodig_pd0_vreg_trim(unsigned int value);

//ldo_pd1 transient
void cobra4_aon_pd_top_d2a_ldodig_pd1_high_curr_en(unsigned int en);

//ldo_pd1 trim
void cobra4_aon_pd_top_d2a_ldodig_pd1_vreg_trim(unsigned int value);

//analog vdd18 voltage trim
void cobra4_aon_pd_top_d2a_vr3v3to1v8_votrim(unsigned int value);

//d2a_ldoanaforpll_trim
void cobra4_aon_pd_top_d2a_ldoanaforpll_trim(unsigned int value);

/////////////////////
//d2a_xo32k_gmtrim
void cobra4_aon_pd_top_d2a_xo32k_gmtrim(unsigned int value);

//d2a_bgln_t_trim
void cobra4_aon_pd_top_d2a_bgln_t_trim(unsigned int value);

//d2a_bgln_vref_trim
void cobra4_aon_pd_top_d2a_bgln_vref_trim(unsigned int value);

#endif

unsigned int cobra4_aon_pd_top_get_mcu_remap2flash_cfg(void);


#endif
