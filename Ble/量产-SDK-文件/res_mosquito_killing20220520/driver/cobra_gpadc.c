/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_gpadc.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/9

	Description :   


	Changed Log	:

		Liujian  2019/5/9			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_gpadc.h"
#include "cobra_pcrm.h"
#include "cobra_aon_pmu.h"
#include "cobra4_aon_top.h"

#if COBRA_GPADC_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_GPADC_TypeDef * cobra_gpadc		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_GPADC_TypeDef * cobra_gpadc; 
#endif

void cobra_gpadc_init(void)
{
		cobra_gpadc = (COBRA_GPADC_TypeDef * )COBRA_GPADC_BASE;
}

/*
"0: the state is IDLE  
1: permit begin to work . When cfg_enable 1->0, it must wait >3 cycles(adc_clkout) to make cfg_enable 0->1"
*/
void cobra_gpadc_cfg_enable(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->GPADC_CTRL |= 0x01;
		}
		else
		{
				cobra_gpadc->GPADC_CTRL &= ~0x01;
		}	
}

/*
"0: not anon mode 
 1: aon mode, the temperature and battery can be monitored at different period, clk=32k"

*/
void cobra_gpadc_cfg_aon_mode(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->GPADC_CTRL |= 0x02;
		}
		else
		{
				cobra_gpadc->GPADC_CTRL &= ~0x02;
		}	
	
}

/*
"0: not one-time reading data , in normal mode
 1: one-time reading data , in normal mode. 
	  temperature and battery reading need clk=32k, 4-ann-pin need clk=8M"
*/
/*
1. cfg_aon_mode, cfg_onetime_in_normal_mode, cfg_sampling_continuous_in_normal_mode; 
	 only one of these three signal can be permit to be enabled at the same time.
2. the controller use the adc_clk,adc_clk=2k or adc_lk=200k, 
	 so every time when we execute one-time measurement, 
	 we need to wait >3 adc_clock periods;
*/
void cobra_gpadc_cfg_onetime_in_normal_mode(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->GPADC_CTRL |= 0x04;
		}
		else
		{
				cobra_gpadc->GPADC_CTRL &= ~0x04;
		}	
	
}

/*
"0: not continuously reading data , in normal mode
 1: continuously reading data , in normal mode. 
	  temperature and battery reading need clk=32k(with some time sleep,LDO shutdown), 
		4-ann-pin need clk=8M(without sleep, LDO no shutdown)"
*/
void cobra_gpadc_cfg_sampling_continuous_in_normal_mode(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->GPADC_CTRL |= 0x08;
		}
		else
		{
				cobra_gpadc->GPADC_CTRL &= ~0x08;
		}
}

/*
"for hand_debug, 00: temp mon, 01: bat mon; 10:GP adc ; 11: idle
for auto-state, 
2'b00: temperature continuously reading 
2'b01: battery continuously reading 
2'b10: 4-ana-pin continuously reading  
2'b11: temperature -battery turn (only cfg_sampling_continuous_in_normal_mode=1)
"
*/
void cobra_gpadc_cfg_btadc_mode(BTADC_MODE mode)
{
		if(mode == TEMP_MON_MODE)
		{
				cobra_gpadc->GPADC_CTRL &= ~(0x03<<4);
		}
		else if(mode == BAT_MON_MODE)
		{
				cobra_gpadc->GPADC_CTRL &= ~(0x03<<4);
				cobra_gpadc->GPADC_CTRL |= (0x01 << 3);
		}
		else if(mode == GPADC_MODE)
		{
				cobra_gpadc->GPADC_CTRL &= ~(0x03<<4);
				cobra_gpadc->GPADC_CTRL |= (0x02 << 3);
		}
		else if(mode == IDLE_MODE)
		{
				cobra_gpadc->GPADC_CTRL |= (0x03 << 3);
		}
	
}

/*
"0: differential pin meas ( vanapin<0>-vanapin<1>   vapapin<2>-vapapin<3>)

 1: single-end measurement(vanapin,vanapin<1> , vapapin,vapapin)"

*/
void cobra_gpadc_cfg_single_end_meas_en(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->GPADC_CTRL |= (0x01 << 6);
		}
		else
		{
				cobra_gpadc->GPADC_CTRL &= ~(0x01 << 6);
		}
	
}

/*
0: use 8M clock 
1: use 32k clock temp&bat mon
*/
void cobra_gpadc_cfg_d32k_mode(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->GPADC_CTRL |= (0x01 << 7);
		}
		else
		{
				cobra_gpadc->GPADC_CTRL &= ~(0x01 << 7);
		}	
}

/*
"for the sleep of temperature and battery measurement, 
cfg_sleep_logic_for_gpadc_en =0, close the gpadc-enable. 
cfg_sleep_logic_for_gpadc_en =1, enable.
"
*/
void cobra_gpadc_cfg_sleep_logic_for_gpadc_en(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->GPADC_CTRL |= (0x01 << 8);
		}
		else
		{
				cobra_gpadc->GPADC_CTRL &= ~(0x01 << 8);
		}	
}

/*
ex. Throwaway 8 points,average next 8points, 
cfg_throwaway_num_position=7;
cfg_sampling_num_position=15; and so on.
*/
void cobra_gpadc_cfg_throwaway_num_position(unsigned int position)
{
			cobra_gpadc->CFG_SAMPLING_CTRL &= ~0x0f;
			cobra_gpadc->CFG_SAMPLING_CTRL |= position&0x0f;	
}

/*
ex. average  8points,cfg_right_shift_for_average=3;
*/
void cobra_gpadc_cfg_right_shift_for_average(unsigned int shift)
{
			cobra_gpadc->CFG_SAMPLING_CTRL &= ~(0x03 << 4);
			cobra_gpadc->CFG_SAMPLING_CTRL |= (shift&0x03 << 4);	
}

void cobra_gpadc_cfg_hand_d_en_LDO_1p8_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= 0x01;
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~0x01;
		}
}

void cobra_gpadc_cfg_hand_d_1p0ref_s_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= 0x02;
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~0x02;
		}
}

void cobra_gpadc_cfg_hand_d_en_t2vc_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= 0x04;
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~0x04;
		}
}



void cobra_gpadc_cfg_hand_d_btadc_mode_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= 0x08;
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~0x08;
		}
}


void cobra_gpadc_cfg_hand_d_adc_diff_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 4);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 4);
		}
}

void cobra_gpadc_cfg_hand_d_adcin_sel_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 5);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 5);
		}
}


void cobra_gpadc_cfg_hand_d_adccal_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 6);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 6);
		}
}

void cobra_gpadc_cfg_hand_d_gpadc_en_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 7);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 7);
		}
}

void cobra_gpadc_cfg_hand_d32k_mode_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 8);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 8);
		}
}

void cobra_gpadc_cfg_hand_read_data_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 9);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 9);
		}
}


void cobra_gpadc_cfg_hand_d_en_LDO_1p8(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 12);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 12);
		}
}

void cobra_gpadc_cfg_hand_d_en_byp_LDO_1p8(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 13);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 13);
		}
}


void cobra_gpadc_cfg_hand_d_1p0ref_s(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 14);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 14);
		}
}


void cobra_gpadc_cfg_hand_d_en_t2vc(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 15);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 15);
		}
}


void cobra_gpadc_cfg_hand_d_gpadc_en(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_OPT |= (1 << 16);
		}
		else
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(1 << 16);
		}
}

/*

"//01: vanapin<0>-vanapin<1>   10: vapapin<2>-vapapin<3>
"

*/
void cobra_gpadc_cfg_hand_adc_diff_mode(ADC_DIFF_MODE mode)
{
		if(mode == VANA_DIFF_MODE)
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(0x03 << 20);
		}
		else if(mode == VANA_PIN0_PIN1)
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(0x03 << 20);
				cobra_gpadc->CFG_HAND_OPT |=  (0x01 << 20);
		}
		else if(mode == VANA_PIN2_PIN3)
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(0x03 << 20);
				cobra_gpadc->CFG_HAND_OPT |=  (0x02 << 20);
		}
}

/*
,//d_adc_diff==00, 
00: vanapin<0>, 
01: vanapin<1>, 
10: vanapin<2>,
11: vanapin<3> 
*/
void cobra_gpadc_cfg_hand_adcin_sel(ADC_SEL_MODE mode)
{
		if(mode == VANA_PAD0)
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(0x03 << 22);
		}
		else if(mode == VANA_PAD1)
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(0x03 << 22);
				cobra_gpadc->CFG_HAND_OPT |=  (0x01 << 22);
		}
		else if(mode == VANA_PAD2)
		{
				cobra_gpadc->CFG_HAND_OPT &= ~(0x03 << 22);
				cobra_gpadc->CFG_HAND_OPT |=  (0x02 << 22);
		}
		else if(mode == VANA_PAD3)
		{
				cobra_gpadc->CFG_HAND_OPT |=  (0x03 << 22);
		}
}

/*
if cal_done=0, when state=IDLE, then goto calibaration. No matter whether the cfg_cal_enable is 1. 

*/
void cobra_gpadc_cfg_cal_by_not_cal_done(unsigned int en)
{
			if(en)
			{
					cobra_gpadc->CFG_CALIBRATION_CTRL |= 1;
			}
			else
			{
					cobra_gpadc->CFG_CALIBRATION_CTRL &= ~1;
			}
}

/*
"when cfg_cal_by_not_cal_done = 0, then 
0: no calibaration  
1: when state=IDLE, state can come into calibration"

*/
void cobra_gpadc_cfg_cal_enable(unsigned int en)
{
			if(en)
			{
					cobra_gpadc->CFG_CALIBRATION_CTRL |= 0x02;
			}
			else
			{
					cobra_gpadc->CFG_CALIBRATION_CTRL &= ~0x02;
			}
}

/*
when ccfg_cal_enable=1;
cfg_adccal=2'b00, first differential calib then sing-end calib, 
cfg_adccal=2'b10: only differential calibration 
cfg_adccal=2'b11: single-end calibration 

for hand-debug: 
00: no cal, 
11: single ended cal  
10: differential cal
*/
void cobra_gpadc_cfg_adccal_mode(ADC_CAL_MODE mode)
{
		if(cobra_gpadc->CFG_CALIBRATION_CTRL & 0x02)
		{
				if(mode == FIRST_DIFF_THEN_SINGLE)
				{
						cobra_gpadc->CFG_CALIBRATION_CTRL &= ~(0x03 << 2);
				}
				else if(mode == ONLY_DIFF)
				{
						cobra_gpadc->CFG_CALIBRATION_CTRL &= ~(0x03 << 2);
						cobra_gpadc->CFG_CALIBRATION_CTRL |=  (0x02 << 2);
				}
				else if(mode == SINGLE_CAL)
				{
						cobra_gpadc->CFG_CALIBRATION_CTRL |= (0x03 << 2);
				}
		}
		else
		{
				if(mode == NO_CAL)
				{
						cobra_gpadc->CFG_CALIBRATION_CTRL &= ~(0x03 << 2);
				}
				else if(mode == SINGLE_ENDED_CAL)
				{
						cobra_gpadc->CFG_CALIBRATION_CTRL |= (0x03 << 2);
				}
				else if(mode == DIFFERENTAL_CAL)
				{
						cobra_gpadc->CFG_CALIBRATION_CTRL &= ~(0x03 << 2);
						cobra_gpadc->CFG_CALIBRATION_CTRL |=  (0x02 << 2);
				}
		}
}


/*
turn round multi-analog pins.shedule the ana-pins one-by-one. 
Only Support 0-1-2-3, 0-1-2,0-1,1-2-3,1-2,2-3, 0,1,2,3 for single-end; 
for differential mode, support 0-1, 0, 1.

*/
void cobra_gpadc_cfg_gpadc_turn_en(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_ANA_VAR_CTRL |= 0x01;
		}
		else
		{
				cobra_gpadc->CFG_ANA_VAR_CTRL &= ~0x01;
		}	
}

/*
it can be inited by 0,1,2,3 for single-end; only 0 for differential mode

*/
void cobra_gpadc_cfg_ana_sig_init(unsigned int val)
{
		cobra_gpadc->CFG_ANA_VAR_CTRL &= ~(0x03 << 1);
		cobra_gpadc->CFG_ANA_VAR_CTRL |= (val&0x03 << 1);
}


/*

the number of  continuous sampling points for one-pin every turn

*/
void cobra_gpadc_cfg_gpadc_sampling_points_count_max(unsigned int val)
{
		cobra_gpadc->CFG_ANA_VAR_CTRL &= ~(0xff << 8);
		cobra_gpadc->CFG_ANA_VAR_CTRL |= (val&0xff << 8);
}


//----------------------------------------------------------------------------------------
//compare A 
void cobra_gpadc_cfg_d_compa_en(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->COMPARE_A_CTRL |= 0x01;
		}
		else
		{
				cobra_gpadc->COMPARE_A_CTRL &= ~0x01;
		}	
}

void cobra_gpadc_cfg_d_compa_reverse(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->COMPARE_A_CTRL |= 0x02;
		}
		else
		{
				cobra_gpadc->COMPARE_A_CTRL &= ~0x02;
		}	
}


void cobra_gpadc_cfg_d_compa_sel(unsigned int select)
{
		cobra_gpadc->COMPARE_A_CTRL &= ~(0x07 << 2);
		cobra_gpadc->COMPARE_A_CTRL |= ((select & 0x07) << 2);
}


void cobra_gpadc_cfg_d_aon_comp_en(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->COMPARE_B_CTRL |= 0x01;
		}
		else
		{
				cobra_gpadc->COMPARE_B_CTRL &= ~0x01;
		}	
}

void cobra_gpadc_cfg_comp_b_one_time_read_en(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->COMPARE_B_CTRL |= 0x02;
		}
		else
		{
				cobra_gpadc->COMPARE_B_CTRL &= ~0x02;
		}	
}

void cobra_gpadc_cfg_comp_b_turn_ctn_read_en(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->COMPARE_B_CTRL |= 0x04;
		}
		else
		{
				cobra_gpadc->COMPARE_B_CTRL &= ~0x04;
		}	
}


void cobra_gpadc_cfg_d_aon_comp_sel_bdiv(unsigned int div)
{
		cobra_gpadc->COMPARE_B_CTRL &= ~(0x03 << 4);
		cobra_gpadc->COMPARE_B_CTRL |= ((div & 0x03) << 4);
}

void cobra_gpadc_cfg_comp_b_turn_init(unsigned int init_val)
{
		cobra_gpadc->COMPARE_B_CTRL &= ~(0x07 << 8);
		cobra_gpadc->COMPARE_B_CTRL |= ((init_val & 0x07) << 8);
}

void cobra_gpadc_cfg_comp_b_turn_max(unsigned int value)
{
		cobra_gpadc->COMPARE_B_CTRL &= ~(0x07 << 12);
		cobra_gpadc->COMPARE_B_CTRL |= ((value & 0x07) << 12);
}

void cobra_gpadc_cfg_comp_b_sampling_count_per_turn(unsigned int value)
{
		cobra_gpadc->COMPARE_B_CTRL &= ~(0xff << 16);
		cobra_gpadc->COMPARE_B_CTRL |= ((value & 0xff) << 16);
}

void cobra_gpadc_cfg_comp_b_wait_time(unsigned int value)
{
		cobra_gpadc->COMPARE_B_CTRL &= ~(((unsigned int)0xff << 24));
		cobra_gpadc->COMPARE_B_CTRL |= ((value & 0xff) << 24);
}


void cobra_gpadc_cfg_hand_d_aon_comp_sel_bp_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_COMPARE_B |= 0x01;
		}
		else
		{
				cobra_gpadc->CFG_HAND_COMPARE_B &= ~0x01;
		}
}

void cobra_gpadc_cfg_hand_read_compa_b_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_COMPARE_B |= 0x02;
		}
		else
		{
				cobra_gpadc->CFG_HAND_COMPARE_B &= ~0x02;
		}
}

void cobra_gpadc_cfg_hand_d_aon_comp_sel_bp(unsigned int value)
{
		cobra_gpadc->CFG_HAND_COMPARE_B &= ~(0x0f << 4);
		cobra_gpadc->CFG_HAND_COMPARE_B |= ((value & 0x0f) << 4);
}

void cobra_gpadc_cfg_hand_d_aon_comp_sel_bm_vld(unsigned int en)
{
		if(en)
		{
				cobra_gpadc->CFG_HAND_COMPARE_B |= (1 << 8);
		}
		else
		{
				cobra_gpadc->CFG_HAND_COMPARE_B &= ~(1 << 8);
		}
}


void cobra_gpadc_cfg_hand_compare_b(unsigned int value)
{
		cobra_gpadc->CFG_HAND_COMPARE_B &= ~(0x03 << 9);
		cobra_gpadc->CFG_HAND_COMPARE_B |= ((value & 0x03) << 9);
}

int  cobra_gpadc_adc_data(void)
{
		volatile unsigned int temp;
	
		while(1)
		{
				temp = cobra_gpadc->UPDATA;
				if(temp & (1 << 12))
				{
						break;
				}
		}
		return temp & 0xfff;
}

/*
0: data is not ready
1: data is ready to be read (the data is signed)"
*/
unsigned int cobra_gpadc_adc_data_ready(void)
{
		if(cobra_gpadc->UPDATA & (1 << 12))
				return 1;
		else
				return 0;
}


unsigned int cobra_gpadc_cal_done(void)
{
		if(cobra_gpadc->CALIBRATION_STATUS & 0x01)
				return 1;
		else
				return 0;
}

/*
because of no calibration and calib_done is 0,
cfg_cal_by_not_cal_done=1, if we config measurement, 
there will be 
*/
unsigned int cobra_gpadc_meas_failed_by_nocal(void)
{
		if(cobra_gpadc->CALIBRATION_STATUS & 0x02)
				return 1;
		else
				return 0;
}


void cobra_gpadc_dcoffset_differential(unsigned int value)
{
		cobra_gpadc->DC_OFFSET &= ~(0x0fff);
		cobra_gpadc->DC_OFFSET |= (value & 0x0fff);
}

void cobra_gpadc_dcoffset_single_end(unsigned int value)
{
		cobra_gpadc->DC_OFFSET &= ~(0x0fff << 16);
		cobra_gpadc->DC_OFFSET |= ((value & 0x0fff) << 16);
}


void app_cobra_gpadc_init(void)
{
//		cobra_aon_pmu_protection_en(1);
//		cobra_aon_pmu_aon_gp_adc_clk_cgf(GPADC_CLK_XTAL48M);  	
//		cobra_aon_d_vdd3_adc_en(1);  	
//		cobra_aon_analog_pin4_enable(1);
//  	cobra_aon_pmu_protection_en(0);

//liujian 这里还需要继续修改   2019/8/6
	
		cobra4_aon_pd_top_protection_en(1);
		cobra4_aon_pd_top_gpadc_clock_selection(GPADC_XTAL48M);	
		cobra4_aon_pd_top_protection_en(0);
	
	
		cobra_gpadc_cfg_hand_d_en_LDO_1p8_vld(1);
		cobra_gpadc_cfg_hand_d_1p0ref_s_vld(1);
		cobra_gpadc_cfg_hand_d_en_t2vc_vld(1);
		cobra_gpadc_cfg_hand_d_btadc_mode_vld(1);
		cobra_gpadc_cfg_hand_d_adc_diff_vld(1);
		cobra_gpadc_cfg_hand_d_adcin_sel_vld(1);
		cobra_gpadc_cfg_hand_d_adccal_vld(1);
		cobra_gpadc_cfg_hand_d_gpadc_en_vld(1);
		cobra_gpadc_cfg_hand_d32k_mode_vld(1);
		cobra_gpadc_cfg_hand_read_data_vld(1);
	
		cobra_gpadc_cfg_hand_d_en_LDO_1p8(1);
		cobra_gpadc_cfg_hand_d_en_byp_LDO_1p8(1);
		cobra_gpadc_cfg_hand_d_1p0ref_s(1);
		
		cobra_gpadc_cfg_hand_d_gpadc_en(1);
		
		cobra_gpadc_cfg_enable(0);
		cobra_gpadc_cfg_sampling_continuous_in_normal_mode(1);
		cobra_gpadc_cfg_sleep_logic_for_gpadc_en(0);
		cobra_gpadc_cfg_btadc_mode(TEMP_MON_MODE);
		
		cobra_gpadc_cfg_hand_d_1p0ref_s(0);
		cobra_gpadc_cfg_hand_adc_diff_mode(VANA_DIFF_MODE);
		cobra_gpadc_cfg_hand_adcin_sel(VANA_PAD3);
		
//		unsigned int i;
//		for(i = 0; i < 100; i++)
//		{
//				printf("adc = %x \n", cobra_gpadc_adc_data());
//		
//		}
}

#endif














