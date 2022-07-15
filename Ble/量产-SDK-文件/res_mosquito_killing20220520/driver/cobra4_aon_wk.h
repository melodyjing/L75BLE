/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4_aon_wk.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/8/6

	Description :   
        for cobra4 
		
	Changed Log	:

		Liujian  2019/8/6			- Creation



*****************************************************************************/
#ifndef __COBRA4_AON_WK_H__
#define __COBRA4_AON_WK_H__
#include <stdio.h>
#include <cobra_config.h>

#if COBRA4_AON_WK_EN

typedef struct cobra4_aon_wk_t{
	
	__attribute__((aligned(4))) volatile unsigned int wk_aon_protect; //  0x100

	__attribute__((aligned(4))) volatile unsigned int wk_clk_control; //   0x104

	__attribute__((aligned(4))) volatile unsigned int wk_ppu_power_control; //   0x108

	__attribute__((aligned(4))) volatile unsigned int wk_ram_control; //   0x10c

	__attribute__((aligned(4))) volatile unsigned int wk_aon_control_low; //  0x110

	__attribute__((aligned(4))) volatile unsigned int wk_aon_power_down_control; //  114

	__attribute__((aligned(4))) volatile unsigned int wk_aon_ram_pwr_ctrl; //0x118

	__attribute__((aligned(4))) volatile unsigned int wk_aon_control_high; //0x11c
	
}COBRA4_AON_WK_Type;


typedef enum {
		CSEM_CLK_RCOSC32K,
		CSEM_CLK_XTAL32K
}AON_CSEM_CLK_SOURCE;

typedef enum {
	AON_PMU_STANDBY,
	AON_PMU_SHUTDOWN	
}AON_PMU_MODE;

typedef enum {
	PD0_LDO_DIG,
	PD0_LDO_PERM	
}PD0_LDO_TYPE;

typedef enum {
	PD0_POWER_MANUAL,
	PD0_POWER_AUTO	
}PD0_POWER_MODE;


#define COBRA4_AON_WK_BASE (0x40089000 + 0x100)

void cobra4_aon_wk_init(void);
void cobra4_aon_wk_protection_en(unsigned int en);
void cobra4_aon_wk_aon_rcosc32k_en(unsigned int en);
void cobra4_aon_wk_aon_rcosc32k_wr_en(unsigned int en);
void cobra4_aon_wk_aon_xtal32k_en(unsigned int en);
void cobra4_aon_wk_aon_xtal32k_en_wr(unsigned int en);

void cobra4_aon_wk_aon_rcosc24m_en(unsigned int en);
void cobra4_aon_wk_aon_rcosc24m_en_wr(unsigned int en);

void cobra4_aon_wk_aon_csem_clk_selection(AON_CSEM_CLK_SOURCE selection);
void cobra4_aon_wk_aon_warm_sysrst_en(unsigned int en);
void cobra4_aon_wk_aon_soft_sysrst_en(unsigned int en);
void cobra4_aon_wk_pd1_por_sysrst_en(unsigned int en);
void cobra4_aon_wk_ppu_standby_req(unsigned int en);
void cobra4_aon_wk_ppu_shutdwn_req(unsigned int en);
void cobra4_aon_wk_aon_pmu_mode(AON_PMU_MODE mode);
void cobra4_aon_wk_mcu_iso_en(unsigned int en);
void cobra4_aon_wk_icytrx_iso_en(unsigned int en);
unsigned int cobra4_aon_wk_m4_system_soft_reset(void);
unsigned int cobra4_aon_wk_m4_system_lockup_reset(void);
unsigned int cobra4_aon_wk_m4_system_watchdog_reset(void);
void cobra4_aon_wk_retention_ldo_control(unsigned int retention_ldo_value);
void cobra4_aon_wk_ram_ldo_control(unsigned int retention_ldo_value);
void cobra4_aon_wk_flash_pdm25_enable(unsigned int en);
void cobra4_aon_wk_flash_pdm25_mask_enable(unsigned int en);
void cobra4_aon_wk_pd0_sw_selection(PD0_LDO_TYPE type);
void cobra4_aon_wk_pd0_sw_mode(PD0_POWER_MODE mode);
void cobra4_aon_wk_pd0_sw_perm_manu_enable(unsigned int en);
void cobra4_aon_wk_pd0_sw_ldo0_manu_enable(unsigned int en);
void cobra4_aon_wk_pd0_sw_power_down_delay_value(unsigned int delay);
void cobra4_aon_wk_pd0_sw_power_up_delay_value(unsigned int delay);
unsigned int cobra4_aon_wk_m4_reset_information(void);

void cobra4_aon_wk_d2a_ldoanaforpll_en(unsigned int en);
void cobra4_aon_wk_apll_isolate0_en(unsigned int en);

/*
1: 表示当前全局复位源为pin or power up 复位

*/
unsigned int cobra4_aon_wk_m4_pin_or_power_up_reset(void);

/*
1: 表示当前全局复位源为m4 lockup 复位


*/
unsigned int cobra4_aon_wk_m4_system_lockup_reset(void);

/*
1: 表示当前全局复位源为soft 复位


*/
unsigned int cobra4_aon_wk_m4_system_soft_reset(void);

/*
1: 表示当前全局复位源为mcu_pd por 复位

*/
unsigned int cobra4_aon_wk_m4_system_mcu_pd_por_reset(void);
	
#endif
#endif
