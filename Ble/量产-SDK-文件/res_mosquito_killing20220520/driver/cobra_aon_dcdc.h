/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_aon_dcdd.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/19

	Description :   accord from cobra2 register document
					APB BUS3
					PCRM module
					Base address is 0x400E0000
					offset address is 0x2000

	Changed Log	:

		Liujian  2019/3/19			- Creation


*****************************************************************************/

#ifndef __COBRA_AON_DCDC_H__
#define __COBRA_AON_DCDC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_AON_DCDC_EN


typedef struct cobra_aon_dcdc_t {
		__attribute__ ((aligned (4))) unsigned int aon_dcdc_mode_ctrl;  
	
		__attribute__ ((aligned (4))) unsigned int aon_ldo_dcdc_ctrl_bulk;  
	
		__attribute__ ((aligned (4))) unsigned int aon_test_enable_bgp_trim;  
	
		__attribute__ ((aligned (4))) unsigned int aon_iq_buck_01234567;  
	
		__attribute__ ((aligned (4))) unsigned int aon_iq_buck_89101112;  
	
		__attribute__ ((aligned (4))) unsigned int aon_div_rc_r32k_freq;  
	
		__attribute__ ((aligned (4))) unsigned int aon_r32k_ctrl;
	
		__attribute__ ((aligned (4))) unsigned int reserver1;
	
		__attribute__ ((aligned (4))) unsigned int reserver2[48];
	
		__attribute__ ((aligned (4))) unsigned int aon_dcdc_state;
	

}COBRA_AON_DCDC_T;

/*
000: hibernate mode
001: sleep mode
010: ptat mode
011: ldo mode
100: vreg mode
101: hold mode
110: psm mode
111: buck mode
*/

typedef enum{
		AON_DCDC_HIBERNATE_MODE,
		AON_DCDC_SLEEP_MODE,
		AON_DCDC_PTAT_MODE,
		AON_DCDC_LDO_MODE,
		AON_DCDC_VREG_MODE,
		AON_DCDC_HOLD_MODE,
		AON_DCDC_PSM_MODE,
		AON_DCDC_BUCK_MODE,
	
}AON_DCDC_MODE;

typedef enum{
		FSM_CLOCK_32KHZ,
		FSM_CLOCK_8MHZ
}FSM_CLOCK;


#define COBRA_AON_DCDC_BASE				(0x4008A000UL)   //This for COBRA4 2019/8/6 liujian modified




void cobra_aon_dcdc_init(void);
void cobra_aon_dcdc_ldo_vddd_en(unsigned int  en);
void cobra_aon_dcdc_bgp_en(unsigned int  en);
void cobra_aon_dcdc_ptat_enb(unsigned int  en);
void cobra_aon_dcdc_ctrl_buck(unsigned int  vaule);

#endif   //COBRA_AON_DCDC_EN



#endif

