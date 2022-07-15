/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_aon_pmu.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/8

	Description :   accord from cobra2 register document
					APB BUS3
					PCRM module
					Base address is 0x400E0000
					offset address is 0x2000

	Changed Log	:

		Liujian  2019/3/13			- Creation


*****************************************************************************/

#ifndef __COBRA_AON_PMU__
#define __COBRA_AON_PMU__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_AON_PMU_EN


typedef struct cobra_aon_pmu_t {
		__attribute__ ((aligned (4))) unsigned int aon_protcetion;  
	
		__attribute__ ((aligned (4))) unsigned int aon_ldo_pd;  
	
		__attribute__ ((aligned (4))) unsigned int aon_ldo_vreg_trim; 

//0x10C	
		__attribute__ ((aligned (4))) unsigned int aon_lf_hf_apb_hclk_cfg;
	
		__attribute__ ((aligned (4))) unsigned int aon_hard_pd_ctrl_state;  //0x110  有值 !!!!!!!
		__attribute__ ((aligned (4))) unsigned int reserver2;   //0x114

	//0x118		
			__attribute__ ((aligned (4))) unsigned int aon_pclk_reset_ctrl;   //这个有错误,应该放到 0x114 中去,, bug!!!!!!
	
	//	__attribute__ ((aligned (4))) unsigned int aon_clk_test; //aon_rcos8m_32k_cfg;   //0x118
	
		__attribute__ ((aligned (4))) unsigned int aon_rcos8m_32k_cfg;        //0x11C
	
		__attribute__ ((aligned (4))) unsigned int reserver3;         //0x120
	//0x124
		__attribute__ ((aligned (4))) unsigned int aon_clk_gate;
	
		__attribute__ ((aligned (4))) unsigned int aon_pmu_mode_enable;
		
		__attribute__ ((aligned (4))) unsigned int reserver4;
	//0x130	
		__attribute__ ((aligned (4))) unsigned int reserver5;
		
		__attribute__ ((aligned (4))) unsigned int reserver6;
		
		__attribute__ ((aligned (4))) unsigned int reserver7;

//0x13C
		__attribute__ ((aligned (4))) unsigned int no_reserver8;  //aon_battery_temperature_adc_cfg;
		
		__attribute__ ((aligned (4))) unsigned int d_xo_32k_cl_adc_en;
		
		__attribute__ ((aligned (4))) unsigned int aon_cmp_32k_rsoc8m_ftune;

//0x148		
		__attribute__ ((aligned (4))) unsigned int aon_aonpad_ana_spare;

//0x14C		
		__attribute__ ((aligned (4))) unsigned int aon_cu_bgp_t_vref_trim;
	
		__attribute__ ((aligned (4))) unsigned int reserver8;
		__attribute__ ((aligned (4))) unsigned int reserver9;
		__attribute__ ((aligned (4))) unsigned int reserver10;
		__attribute__ ((aligned (4))) unsigned int reserver11;
		
//0x160		
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg0;
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg1;
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg2;
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg3;
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg4;
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg5;
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg6;
		__attribute__ ((aligned (4))) unsigned int aon_sec_cfg7;
	
//0x180
		__attribute__ ((aligned (4))) unsigned int aon_io0_edge_det_cfg;
		
		__attribute__ ((aligned (4))) unsigned int aon_wakup_src_aon2mcu_src1;
		
		__attribute__ ((aligned (4))) unsigned int aon_wakup_src_aon2mcu_src2;
		
		__attribute__ ((aligned (4))) unsigned int reserver12[15];
		
		__attribute__ ((aligned (4))) unsigned int aon_reset_state_and_wakeup_state;
	
}COBRA_AON_PMU_T;

//蓝牙低功耗时钟
typedef enum {
		LF_CLK_RCOSC32K,
		LF_CLK_XTAL48M,
		LF_CLK_RCOSC8M,
		LF_CLK_XTAL32K
}AON_LF_CLK_SOURCE;

typedef enum {
		GPADC_CLK_RCOSC8M,
		GPADC_CLK_XTAL48M,
}AON_GPADC_CLK_SOURCE;


typedef enum {
		HF_CLK_RCOSC8M,
		HF_CLK_XTAL48M,
}AON_HF_CLK_SOURCE;


typedef enum {
		AON_HCLKI,
		CSEM_CLK,
}AON_HCLK_SOURCE;


typedef enum {
		CSEM_RCOSC32K,
		CSEM_XTAL32K,
}AON_CSEM_32K_CLK_SOURCE;

typedef enum {
		BATTERY_MODE,
		TEMPERATURE_MODE,
}ADC_MONITOR_MODE;

typedef enum {
		AON_SEQ_CFG0,
		AON_SEQ_CFG1,
		AON_SEQ_CFG2,
		AON_SEQ_CFG3,
		AON_SEQ_CFG4,
		AON_SEQ_CFG5,
		AON_SEQ_CFG6,
		AON_SEQ_CFG7,
}AON_SEQ_CFG_INDEX;

/*
assign o_events={
         16'b0,
         i_gpadc_int       ,
         i_timer2_int      ,
         i_va_wakeup       ,
         {i_rtc_alm1,i_rtc_alm0,i_rtc_int} ,
         i_int_rmt0        ,
         i_int_rmt0_wakeup ,
         i_uc_int
         };
*/
typedef enum {
		IO0_EDGE_INT_EVENT = 1,
		IO1_EDGE_INT_EVENT,
		IO2_EDGE_INT_EVENT,
		IO3_EDGE_INT_EVENT,
		IO4_EDGE_INT_EVENT,
	
		IO5_NULL,    //??????????         
	
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

#define COBRA_AON_PMU_BASE				(0x40082100UL)



AON_HF_CLK_SOURCE cobra_aon_pmu_clk_source(void);

void cobra_aon_pmu_init(void);
void cobra_aon_pmu_protection_en(unsigned int en);
void cobra_aon_pmu_aon_hf_clk_cfg(AON_HF_CLK_SOURCE source_index);

void cobra_aon_pmu_aon_rcosc32k_enable(unsigned int en);
void cobra_aon_pmu_aon_rcosc8m_enable(unsigned int en);
void cobra_aon_pmu_aon_xtal32k_enable(unsigned int en);


void cobra_aon_pmu_wakeup_source0_selection(AON_EVENT_SOURCE selection);
void cobra_aon_pmu_wakeup_source1_selection(AON_EVENT_SOURCE selection);
void cobra_aon_pmu_wakeup_source2_selection(AON_EVENT_SOURCE selection);

void cobra_aon_pmu_aon2mcu_source0_selection(AON_EVENT_SOURCE selection);
void cobra_aon_pmu_aon2mcu_source1_selection(AON_EVENT_SOURCE selection);
void cobra_aon_pmu_aon2mcu_source2_selection(AON_EVENT_SOURCE selection);
void cobra_aon_pmu_aon2mcu_source3_selection(AON_EVENT_SOURCE selection);
void cobra_aon_pmu_aon2mcu_source4_selection(AON_EVENT_SOURCE selection);

void cobra_aon_pmu_rtc_pclk_enable(unsigned int en);
void cobra_aon_pmu_irda_pclk_enable(unsigned int en);
void cobra_aon_pmu_enclica_rtc_clk_mask_enable(unsigned int en);
void cobra_aon_pmu_ucpu_hclk_enable(unsigned int en);
void cobra_aon_pmu_timer2_pclk_enable(unsigned int en);
void cobra_aon_pmu_gpadc_pclk_enable(unsigned int en);

void cobra_aon_pmu_irda_presetn(unsigned int en);
void cobra_aon_pmu_rtc_presetn(unsigned int en);
void cobra_aon_pmu_ucpu_presetn(unsigned int en);
void cobra_aon_pmu_timer2_presetn(unsigned int en);
void cobra_aon_pmu_gpadc_presetn(unsigned int en);

void cobra_aon_pmu_aon_csem_clk_cfg(AON_CSEM_32K_CLK_SOURCE source_index);
void cobra_aon_pmu_aon_gp_adc_clk_cgf(AON_GPADC_CLK_SOURCE source);
void cobra_aon_pmu_aon_pclk_cfg_divider(unsigned int  divider);
/*
aon_hclk = hf_clk/(aon_hclki_cfg+1)
当hf_clk选择为xtal48m,aon_hclki_cfg不能配置为0
*/
void cobra_aon_pmu_aon_hclki_cfg_divider(unsigned int  divider);
/*
aon_hclk selection
*/
void cobra_aon_pmu_aon_hclk_sel(AON_HCLK_SOURCE source);

void cobra_aon_d_vdd3_adc_en(unsigned int en);

void cobra_aon_d_xo_32k_cl(unsigned int value);

void cobra_aon_analog_pin0_enable(unsigned int en);
void cobra_aon_analog_pin1_enable(unsigned int en);
void cobra_aon_analog_pin2_enable(unsigned int en);
void cobra_aon_analog_pin3_enable(unsigned int en);
void cobra_aon_analog_pin4_enable(unsigned int en);


void cobra_aon_pmu_shutdown_mode_enable(unsigned int en);
void cobra_aon_pmu_standby_mode_enable(unsigned int en);
void cobra_aon_pmu_wakeup_reset_enable(unsigned int en);


void cobra_aon_pmu_io0_posedge_detect_enable(unsigned int en);
void cobra_aon_pmu_io0_negedge_detect_enable(unsigned int en);
void cobra_aon_pmu_io1_posedge_detect_enable(unsigned int en);
void cobra_aon_pmu_io1_negedge_detect_enable(unsigned int en);
void cobra_aon_pmu_io2_posedge_detect_enable(unsigned int en);
void cobra_aon_pmu_io2_negedge_detect_enable(unsigned int en);
void cobra_aon_pmu_io3_posedge_detect_enable(unsigned int en);
void cobra_aon_pmu_io3_negedge_detect_enable(unsigned int en);
void cobra_aon_pmu_io4_posedge_detect_enable(unsigned int en);
void cobra_aon_pmu_io4_negedge_detect_enable(unsigned int en);
void cobra_aon_pmu_mcu_io_posedge_detect_enable(unsigned int en);
void cobra_aon_pmu_mcu_io_negedge_detect_enable(unsigned int en);
void cobra_aon_pmu_edge_irq_en(unsigned int en);
void cobra_aon_pmu_io_lat_en(unsigned int en);

void cobra_an_d_rco8m_ftune_set(unsigned char value);
unsigned char cobra_an_d_rco8m_ftune_read(void);

void cobra_aon_pmu_aon_seq_cfg(AON_SEQ_CFG_INDEX seq_index, unsigned short address, unsigned short data, bool last_cmd);

void cobra_aon_pmu_d_vddio_postdrv_pd1_en(unsigned int en);

void cobra_aon_pmu_aon_lf_clk_cfg(AON_LF_CLK_SOURCE source_index);

void cobra_aon_pmu_gp_clock_gate_enable(unsigned int en);
void cobra_aon_pmu_va_clock_gate_enable(unsigned int en);
void cobra_aon_pmu_lf_clk_gate_enable(unsigned int en);
void cobra_aon_pmu_hf_clk_gate_enable(unsigned int en);
#endif   //COBRA_PCRM_EN



#endif

