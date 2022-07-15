/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	cobra_aon_dcdc.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/19

	Description :   
					cobra power manger control
					according from document cobra2 register
					
					Power, Reset and Clock Management


	Changed Log	:

		Liujian  2019/3/19			- Creation


*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_aon_dcdc.h"


#if COBRA_AON_DCDC_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_AON_DCDC_T * cobra_aon_dcdc		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_AON_DCDC_T * cobra_aon_dcdc;
#endif

void cobra_aon_dcdc_init(void)
{
		cobra_aon_dcdc = (COBRA_AON_DCDC_T *)COBRA_AON_DCDC_BASE;  
}

/*
DCDC mode control

000: hibernate mode
001: sleep mode
010: ptat mode
011: ldo mode
100: vreg mode
101: hold mode
110: psm mode
111: buck mode

*/
void cobra_aon_dcdc_mode_ctrl(AON_DCDC_MODE mode)
{
		if(mode == AON_DCDC_HIBERNATE_MODE)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~0x07;
		}
		else if(mode == AON_DCDC_SLEEP_MODE)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~0x07;
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= 0x01;
		}
		else if(mode == AON_DCDC_PTAT_MODE)  //Proportional to absolute temperature
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~0x07;
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= 0x02;
		}
		else if(mode == AON_DCDC_LDO_MODE)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~0x07;
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= 0x03;
		}
		else if(mode == AON_DCDC_VREG_MODE)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~0x07;
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= 0x04;
		}
		else if(mode == AON_DCDC_HOLD_MODE)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~0x07;
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= 0x05;
		}
		else if(mode == AON_DCDC_PSM_MODE)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~0x07;
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= 0x06;
		}
		else if(mode == AON_DCDC_BUCK_MODE)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~0x07;
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= 0x07;
		}
		
	
}


/*

Select the PMU FSM clock: 0 => 32kHz, 1 => 8MHz (if present)

*/
void cobra_aon_dcdc_sel_clk(FSM_CLOCK fsm_clock)
{
		if(fsm_clock == FSM_CLOCK_32KHZ)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<8);
		}
		else if(fsm_clock == FSM_CLOCK_8MHZ)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<8);
		}
}

/*

If set to 1, the radio_on signal triggers the DCDC active state
*/
void cobra_aon_dcdc_dcdc_on_w_radio(unsigned int  on)
{
		if(on)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<9);
		}
		else
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<9);
		}	
}


/*
If set to 1 allows the direct transition from Buck to Hold state without passing through the PSM
state. It requires that the enb_buck_hold bit is set to 0.

*/
void cobra_aon_dcdc_direct_buck_hold(unsigned int  en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<10);
		}
		else
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<10);
		}	
}

/*
	If set to 1, disables the LDO bootstrap when DCDC is active

*/
void cobra_aon_dcdc_disable_bstr_w_dcdc(unsigned int  en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<11);
		}
		else
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<11);
		}	
}

/*
If set to 1, The buck clock division is set to minimum when the radio is active
*/
void cobra_aon_dcdc_min_buck_clkdiv_w_radio(unsigned int  en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<12);
		}
		else
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<12);
		}	
}

/*
If set to 1, forces the usage of the low-power Vref

*/
void cobra_aon_dcdc_force_vreflp(unsigned int  en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<13);
		}
		else
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<13);
		}	
}


//----------------0x02 PMU_CONF -----------------------------------------------------
/*
Control of the Ultra-Low-Power PTAT
*/
void cobra_aon_dcdc_ptat_ulp_ctrl(unsigned int  control_value)
{
		
		cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(0x03 << 16);
		cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (control_value&0x03) << 16;
		
}

/*

Force the startup mode of the Bandgap LP

*/
void cobra_aon_dcdc_en_bglp_startup(unsigned int  en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<18);
		}
		else
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<18);
		}	
}

/*

Vref tuning of the bandgap

default vaule is 4
*/
void cobra_aon_dcdc_bgp_lp_vref_tune(unsigned int  vref)
{
		cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(0x07 << 19);
		cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (vref&0x07) << 19;
}

/*
Tune the low-power Vref (00 - max, 11 - min)

*/
void cobra_aon_dcdc_vreflp_vtrl(unsigned int  vref)
{
		cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(0x03 << 22);
		cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (vref&0x03) << 22;
}

//---------------------0x03 - ICYTRX_PMU_ENABLES------------------------------------------
//0x03 - ICYTRX_PMU_ENABLES

/*

Enable icytrx PTAT (active low)
*/
void cobra_aon_dcdc_ptat_enb(unsigned int  en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<24);
		}
		else
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<24);
		}	
}

/*

Enable icytrx bandgap

*/
void cobra_aon_dcdc_bgp_en(unsigned int  en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<25);
		}
		else
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<25);
		}	
}

/*
Enable icytrx VDDD

*/
void cobra_aon_dcdc_ldo_vddd_en(unsigned int  en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl |= (1<<26);
		}
		else
		{
				cobra_aon_dcdc->aon_dcdc_mode_ctrl &= ~(1<<26);
		}	
}

//------------0x04 BUCK_CONF_1-----------------------------------

/*
Vreg trim for the DCDC LDO
default is 9

*/
void cobra_aon_dcdc_ldo_dcdc_vreg_trim(unsigned int  trim_value)
{
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(0x0f << 0);
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (trim_value&0x0f) << 0;
}

/*
Enable transient mode for the DCDC LDO

*/
void cobra_aon_dcdc_ldo_dcdc_transient(unsigned int  en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (1<<4);
		}
		else
		{
				cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(1<<4);
		}	
}

/*

Ctrl buck LDO
*/
void cobra_aon_dcdc_ctrl_ldo(unsigned int  value)
{
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(0x07 << 5);
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (value&0x07) << 5;
}


/*
Ctrl buck

default 0x42
*/
void cobra_aon_dcdc_ctrl_buck(unsigned int  vaule)
{
	  volatile unsigned int temp;
	  temp = cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk;
	  temp &= ~(0xff << 8);
	  temp |= (vaule&0xff) << 8;
	  cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk = temp;
//		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(0xff << 8);
//		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (vaule&0xff) << 8;
}


//------------0x06 BUCK_CONF_2-----------------------------------

/*

Division clock for the DCDC
default is 3
*/
void cobra_aon_dcdc_buck_clkdiv(unsigned int  div)
{
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(0x03 << 16);
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (div&0x03) << 16;
}

/*

If set to 1, the DCDC uses the VCO divided clock during RF operations

*/
void cobra_aon_dcdc_dcdc_use_lo_clk(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (1<<18);
		}
		else
		{
				cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(1<<18);
		}	
}

/*
If set to 1, the RC oscillator is disabled when the LO signal as DCDc clock

*/
void cobra_aon_dcdc_disable_rc_w_lo(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (1<<19);
		}
		else
		{
				cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(1<<19);
		}	
}

//---------------0x07 CSEM_RC_32K -----------------------

/*

Trim the amplitude of the 32k RC osc

default is 1
*/
void cobra_aon_dcdc_rc32k_aml(unsigned int  ampl)
{
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(0x03 << 24);
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (ampl&0x03) << 24;
}

/*

Trim the spread of the 32k RC osc

default is 0
*/
void cobra_aon_dcdc_rc32k_spread(unsigned int  spread)
{
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(0x03 << 26);
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (spread&0x03) << 26;
}

/*
Trim the temperature drift of the 32k RC osc

default is 0
*/
void cobra_aon_dcdc_rc32k_tdrift(unsigned int  drift)
{
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk &= ~(0x03 << 28);
		cobra_aon_dcdc->aon_ldo_dcdc_ctrl_bulk |= (drift&0x03) << 28;
}


//---------------------0x08 BUCK_ENABLES-------------------------------
/*
enables DCDC feedback opamp circuit (must be on during DCDC, off for test purpose only)

default is 1 on
*/

void cobra_aon_dcdc_en_op(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<0);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<0);
		}	
}

/*
enables DCDC error comparator circuit (must be on during DCDC, off for test purpose only)

default is 1 on
*/

void cobra_aon_dcdc_en_comp(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<1);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<1);
		}	
}

/*
enables DCDC current sense circuit (must be on during DCDC, off for test purpose only)
default is 1 on
*/

void cobra_aon_dcdc_en_sense(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<2);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<2);
		}	
}


/*

enables DCDC current ramp circuit (must be on during DCDC, off for test purpose only)
default is 1 on
*/

void cobra_aon_dcdc_en_ramp(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<3);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<3);
		}	
}

/*

enables DCDC pulse skip detection circuit (must be on during DCDC, off for test purpose
only)

default is 1 on
*/
void cobra_aon_dcdc_en_pskip(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<4);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<4);
		}	
}

/*
enables DCDC zero crossing detection circuit (must be on during DCDC, off for test purpose
only)

default is on 1
*/  
void cobra_aon_dcdc_en_zc(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<5);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<5);
		}	
}

/*
enables DCDC high current detection circuit (must be on during DCDC, off for test purpose
only)

default is on 1

*/
void cobra_aon_dcdc_en_iover(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<6);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<6);
		}	
}


/*

enables DCDC hold state when DCDC is idle, active low

default is on 

*/
void cobra_aon_dcdc_en_buck_hold(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<7);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<7);
		}	
}

//all follow is used to debug 
/*
Debug: overrides the FSM definintion. Enable pulse skip mode,PSM

default is off
*/
void cobra_aon_dcdc_debug_en_psm(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<8);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<8);
		}	
}

/*
Debug: overrides the FSM definintion. enables DCDC bootstrap PTAT

default is off

*/
void cobra_aon_dcdc_debug_en_plat(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<9);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<9);
		}	
}

/*
Debug: overrides the FSM definintion. enables DCDC bootstrap LDO

default is off 0

*/
void cobra_aon_dcdc_debug_en_ldo(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<10);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<10);
		}	
}

/*
Debug: overrides the FSM definintion. enables DCDC service blocks

default is off 0

*/
void cobra_aon_dcdc_debug_en_buck(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<11);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<11);
		}	
}


/*
Debug: overrides the FSM definintion. enables DCDC internal regulator

default is off 0

*/
void cobra_aon_dcdc_debug_en_vreg(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<12);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<12);
		}	
}

/*
Debug: overrides the FSM definintion. enables clock for DCDC operation

default is off 0

*/
void cobra_aon_dcdc_debug_en_clk(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= (1<<13);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(1<<13);
		}	
}

//-----------------0x0A ICYTRX_PMU_CONF ----------------------------

/*

PMU ptat current fine tuning: 

00 typ, 01 min (-10%), 10 max (+33%), 11 mid (+14%)


*/
void cobra_aon_dcdc_ptat_ctrl(unsigned int  ctrl)
{
		cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(0x03 << 16);
		cobra_aon_dcdc->aon_test_enable_bgp_trim |= (ctrl&0x03) << 16;
}

/*

bandgap voltage trimming, two's-complement code

*/
void cobra_aon_dcdc_bgp_vref_trim(unsigned int  trim)
{
		cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(0x1f << 18);
		cobra_aon_dcdc->aon_test_enable_bgp_trim |= (trim&0x1f) << 18;
}

/*

bandgap voltage trimming versus temperature, two's-complement code
*/
void cobra_aon_dcdc_bgp_t_trim(unsigned int  trim)
{
		cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(0x0f << 23);
		cobra_aon_dcdc->aon_test_enable_bgp_trim |= (trim&0x0f) << 23;
}

/*

digital LDO 4 bit output voltage control: 0x0 = 0.6V, 0xF = 1.3V
*/
void cobra_aon_dcdc_ldo_vddd_vreg_trim(unsigned int  trim)
{
		cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~(0x0f << 27);
		cobra_aon_dcdc->aon_test_enable_bgp_trim |= (trim&0x0f) << 27;
}

/*

digital LDO high current mode  
*/
void cobra_aon_dcdc_ldo_vddd_transient(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim |= ((unsigned int)1<<31);
		}
		else
		{
				cobra_aon_dcdc->aon_test_enable_bgp_trim &= ~((unsigned int)1<<31);
		}	
}

//---------------------0x0C IQS_BUCK_0 ---------------------------------------------

/*

feedback opamp bais current

default is 6
*/
void cobra_aon_dcdc_iq_buck_0(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_01234567 &= ~(0x0f << 0);
				cobra_aon_dcdc->aon_iq_buck_01234567 |= (value&0x0f) << 0;
}

/*
error comparator bias current
default is 9
*/
void cobra_aon_dcdc_iq_buck_1(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_01234567 &= ~(0x0f << 4);
				cobra_aon_dcdc->aon_iq_buck_01234567 |= (value&0x0f) << 4;
}

/*

current sense circuit bias current

default is 9

*/
void cobra_aon_dcdc_iq_buck_2(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_01234567 &= ~(0x0f << 8);
				cobra_aon_dcdc->aon_iq_buck_01234567 |= (value&0x0f) << 8;
}


/*

current ramp circuit bias current

default is 0x0b
*/
void cobra_aon_dcdc_iq_buck_3(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_01234567 &= ~(0x0f << 12);
				cobra_aon_dcdc->aon_iq_buck_01234567 |= (value&0x0f) << 12;
}

/*

pulse skip detection circuit bias current

default is 9
*/
void cobra_aon_dcdc_iq_buck_4(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_01234567 &= ~(0x0f << 16);
				cobra_aon_dcdc->aon_iq_buck_01234567 |= (value&0x0f) << 16;
}

/*
zero crossing detection circuit bias current

default is 0x05

*/
void cobra_aon_dcdc_iq_buck_5(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_01234567 &= ~(0x0f << 20);
				cobra_aon_dcdc->aon_iq_buck_01234567 |= (value&0x0f) << 20;
}


/*
zero crossing comparator bias current

default is 0x09

*/
void cobra_aon_dcdc_iq_buck_6(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_01234567 &= ~(0x0f << 24);
				cobra_aon_dcdc->aon_iq_buck_01234567 |= (value&0x0f) << 24;
}


/*
zero crossing offset control bias current

default is 0x04

*/
void cobra_aon_dcdc_iq_buck_7(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_01234567 &= ~((unsigned int)0x0f << 28);
				cobra_aon_dcdc->aon_iq_buck_01234567 |= ((unsigned int)(value&0x0f)) << 28;
}


//----- 0x10  IQS_BUCK_1 --------------------------------

/*
current ramp control bias current
default is 4

*/
void cobra_aon_dcdc_iq_buck_8(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_89101112 &= ~(0x0f << 0);
				cobra_aon_dcdc->aon_iq_buck_89101112 |= (value&0x0f) << 0;
}


/*

high current threshold control bias current

default is 0x04

*/
void cobra_aon_dcdc_iq_buck_9(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_89101112 &= ~(0x0f << 4);
				cobra_aon_dcdc->aon_iq_buck_89101112 |= (value&0x0f) << 4;
}

/*
clock pulse delay control bias current

default is 0x0B

*/
void cobra_aon_dcdc_iq_buck_10(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_89101112 &= ~(0x0f << 8);
				cobra_aon_dcdc->aon_iq_buck_89101112 |= (value&0x0f) << 8;
}

/*
zero crossing clock pulse delay control bias current

default is 0x08

*/
void cobra_aon_dcdc_iq_buck_11(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_89101112 &= ~(0x0f << 12);
				cobra_aon_dcdc->aon_iq_buck_89101112 |= (value&0x0f) << 12;
}


//----0x12  IQS_BUCK_2 -----------------
/*
pulse skip mode threshold control bias current

default is 0x09

*/
void cobra_aon_dcdc_iq_buck_12(unsigned int value)
{
				cobra_aon_dcdc->aon_iq_buck_89101112 &= ~(0x0f << 16);
				cobra_aon_dcdc->aon_iq_buck_89101112 |= (value&0x0f) << 16;
}


//---- 0x18 RC_OSC_FLL -----------------------------------
/*

Ratio between the RF signal and the RC frequency

default 0x132
*/
void cobra_aon_dcdc_div_rc(unsigned int value)
{
				cobra_aon_dcdc->aon_div_rc_r32k_freq &= ~(0x1ff << 0);
				cobra_aon_dcdc->aon_div_rc_r32k_freq |= (value&0x1ff) << 0;
}

/*
RC oscillator amplitude trimming
default is 0x02

*/
void cobra_aon_dcdc_rcos_8m_ampl(unsigned int value)
{
				cobra_aon_dcdc->aon_div_rc_r32k_freq &= ~(0x03 << 10);
				cobra_aon_dcdc->aon_div_rc_r32k_freq |= (value&0x03) << 10;
}

/*
If set to 1, the 8MHz RC oscillator is enabled

default is 0
*/
void cobra_aon_dcdc_en_rcosc_8m(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_div_rc_r32k_freq |= (1<<12);
		}
		else
		{
				cobra_aon_dcdc->aon_div_rc_r32k_freq &= ~(1<<12);
		}	
}

/*
If set to 1, the FLL is activated

default is 0

*/
void cobra_aon_dcdc_en_fll(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_div_rc_r32k_freq |= (1<<13);
		}
		else
		{
				cobra_aon_dcdc->aon_div_rc_r32k_freq &= ~(1<<13);
		}	
}

/*

If set to 1, the FLL is bypassed. The div_rc value is used as trim for the RC oscillator

default is 0
*/
void cobra_aon_dcdc_bypass_fll(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_div_rc_r32k_freq |= (1<<14);
		}
		else
		{
				cobra_aon_dcdc->aon_div_rc_r32k_freq &= ~(1<<14);
		}	
}

//--------------
void cobra_aon_dcdc_rc32k_freq(unsigned int value)
{
				cobra_aon_dcdc->aon_div_rc_r32k_freq &= ~(0x7fff << 16);
				cobra_aon_dcdc->aon_div_rc_r32k_freq |= (value&0x7fff) << 16;
}


void cobra_aon_dcdc_en_rc32k_trim(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_div_rc_r32k_freq |= ((unsigned int)1<<31);
		}
		else
		{
				cobra_aon_dcdc->aon_div_rc_r32k_freq &= ~((unsigned int)1<<31);
		}	
}


void cobra_aon_dcdc_rc32k_cnt_len(unsigned int value)
{
				cobra_aon_dcdc->aon_r32k_ctrl &= ~(0x3 << 0);
				cobra_aon_dcdc->aon_r32k_ctrl |= (value&0x03) << 0;
}


void cobra_aon_dcdc_rc32k_tau(unsigned int value)
{
				cobra_aon_dcdc->aon_r32k_ctrl &= ~(0x3 << 2);
				cobra_aon_dcdc->aon_r32k_ctrl |= (value&0x03) << 2;
}


void cobra_aon_dcdc_rc32k_sd_tau(unsigned int value)
{
				cobra_aon_dcdc->aon_r32k_ctrl &= ~(0x3 << 4);
				cobra_aon_dcdc->aon_r32k_ctrl |= (value&0x03) << 4;
}


void cobra_aon_dcdc_rc32k_bypass(unsigned int en)
{
		if(en)
		{
				cobra_aon_dcdc->aon_r32k_ctrl |= (1<<6);
		}
		else
		{
				cobra_aon_dcdc->aon_r32k_ctrl &= ~(1<<6);
		}	
}


void cobra_aon_dcdc_test_mode(unsigned int value)
{
				cobra_aon_dcdc->aon_r32k_ctrl &= ~(0x3 << 8);
				cobra_aon_dcdc->aon_r32k_ctrl |= (value&0x03) << 8;
}

//DCDC_STATE
//DCDC FSM STATE
/*
"000: hibernate mode
001: sleep mode
010: ptat mode
011: ldo mode
100: vreg mode
101: hold mode
110: psm mode
111: buck mode"
*/


AON_DCDC_MODE  cobra_aon_dcdc_dcdc_fsm_state(void)
{
			unsigned int value;
			value = cobra_aon_dcdc->aon_dcdc_state;
			value &= 0x07;
			if(value == 0x00)
			{
					return AON_DCDC_HIBERNATE_MODE;
			}
			else if(value == 0x01)
			{
					return AON_DCDC_SLEEP_MODE;
			}
			else if(value == 0x02)
			{
					return AON_DCDC_PTAT_MODE;
			}
			else if(value == 0x03)
			{
					return AON_DCDC_LDO_MODE;
			}
			else if(value == 0x04)
			{
					return AON_DCDC_VREG_MODE;
			}
			else if(value == 0x05)
			{
					return AON_DCDC_HOLD_MODE;
			}
			else if(value == 0x06)
			{
					return AON_DCDC_PSM_MODE;
			}
			else  //0x07
			{
					return AON_DCDC_BUCK_MODE;
			}
	
}

//-------------------0xE1 DCDC_READOUT --------------

/*

ZC (Zero-Crossing) 0x0
*/
unsigned int cobra_aon_dcdc_zc(void)
{
		if(cobra_aon_dcdc->aon_dcdc_state & (1 << 8))
		{
				return 1;
		}
		return 0;
}

/*
PSM (Pulse-Skip-Mode)

*/
unsigned int cobra_aon_dcdc_psm(void)
{
		if(cobra_aon_dcdc->aon_dcdc_state & (1 << 9))
		{
				return 1;
		}
		return 0;
}

/*

IOV (I overflow)

*/
unsigned int cobra_aon_dcdc_iov(void)
{
		if(cobra_aon_dcdc->aon_dcdc_state & (1 << 10))
		{
				return 1;
		}
		return 0;
}

/*
ICTAT LP ok

*/
unsigned int cobra_aon_dcdc_ictat_lp_ok(void)
{
		if(cobra_aon_dcdc->aon_dcdc_state & (1 << 11))
		{
				return 1;
		}
		return 0;
}

/*
nreset signal of the retention LDO

*/
unsigned int cobra_aon_dcdc_nreset_ret(void)
{
		if(cobra_aon_dcdc->aon_dcdc_state & (1 << 12))
		{
				return 1;
		}
		return 0;
}

/*
Wake-up signal

*/
unsigned int cobra_aon_dcdc_wake_up(void)
{
		if(cobra_aon_dcdc->aon_dcdc_state & (1 << 13))
		{
				return 1;
		}
		return 0;
}

#endif














