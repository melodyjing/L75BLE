/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4_pll.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/2/25

	Description :   


	Changed Log	:

		Liujian  2020/2/25			- Creation



*****************************************************************************/
#include "cobra4_pll.h"

#if COBRA4_PLL_EN

#define TRIM_FREQ_STEP  50.0

int cobra4_pll_set_frenquence(double pll_target)
{
    int   pll_cnt_judge;
    int   trim_gain;
    //   int   step_value;

    //sdm
    int  p_num;
    int  fract_num;

    double f_target_div2;
    double f_target_div4;
    double f_dig;
    double f_ref;
    int  int_trim_gain;
    int   ctrl_cnt_cfg = 0x1f;
    int   flag = 0;

    double temp;
    double temp1;
    int   temp_int;


    pll_target *= 2.0;
    f_target_div2 = pll_target/2.0;
    f_target_div4 = pll_target/4.0;

    f_dig = 48.0; 

    f_ref = 48.0/2; 


    temp = (ctrl_cnt_cfg + 1)*f_target_div4/f_dig;



    pll_cnt_judge = (int)((temp + 0.5)*10)*0.1;


    if(pll_cnt_judge>=1024)
    {
        return 0;
    }

    temp = TRIM_FREQ_STEP*(ctrl_cnt_cfg+1)/32.0/f_dig;

    int_trim_gain = (int)((temp + 0.5)*10)*0.1;

    if(int_trim_gain>=2)
    {
        trim_gain = 0x3f; //set to max value
    }
    else
    {
        temp1 = (double)int_trim_gain;
        /* 
        temp >= 1.0 && temp < 1.5    int_trim_gain = 1
        temp >= 0.5 && temp < 1.0    int_trim_gain = 1
        temp >= 0   && temp < 0.5    int_trim_gain = 0
        */ 

        if(temp1 > temp)
        {

            int_trim_gain -= 1;
        }
        if(int_trim_gain)
        {
            trim_gain = 0x20;
            flag = 1;
        }
        else
        {
            trim_gain = 0;
            flag = 0;
        }

        temp -= (double)flag;
        temp *= 32;
        temp_int = (int)((temp+0.5)*10)*0.1;

        trim_gain += temp_int;

    }

    //assure trim_gain!=0
    if(trim_gain == 0)
        trim_gain = 1;


    //   step_value = trim_gain/2; //=1;

    //    step_value = 1;

    temp = f_target_div2/f_ref;

    p_num = (int)((temp + 0.5)*10)*0.1;


    if((double)(p_num) > (temp)) //round up
        p_num = p_num - 1;
    if(p_num >= 32) //only p[4:0] valid
    {
    //printf("Error: p_num = 0x%0x, >=32 !!!!! ", p_num);
        return 0;
    }
    temp -= (double)p_num;
    temp *= (double)0x4000000;
    fract_num = (int)((temp + 0.5)*10)*0.1; 

    p_num = p_num - 1; 



//    printf("pll_cnt_judge = %d \n", pll_cnt_judge);
//    printf("trim_gain = %d \n", trim_gain);
//    printf("p_num = %d \n", p_num);
//    printf("fract_num = %d \n", fract_num);
//    printf("ctrl_cnt_cfg = %d \n", ctrl_cnt_cfg);
		
	cobra_pcrm_write_apll_cfg2(0);  //ankui
		
	cobra_pcrm_d2a_pll_pfd_en(1);
		
	cobra4_aon_wk_protection_en(1);
	cobra4_aon_wk_apll_isolate0_en(1);
	cobra4_aon_wk_protection_en(0);
//1.
	cobra_pcrm_d2a_pll_cp_en(0);
	cobra_pcrm_d2a_pll_cp_iupcom(0);
	cobra_pcrm_d2a_pll_en(1);
	cobra_pcrm_d2a_pll_lct_en(1);
	cobra_pcrm_d2a_pll_pfd_en(0);
	cobra_pcrm_d2a_pll_retimer_en(1);
	cobra_pcrm_d2a_pll_vco_en(1);
	cobra_pcrm_d2a_pll_lpf_c1(0x0a);
	cobra_pcrm_d2a_pll_vco_vbias(4);	

    
  //  REG_PL_WR(PCRM_BASE+ANA_CFG0_REG,0x188);

	cobra_pcrm_d2a_vdac_en(0);
	cobra_pcrm_d2a_cpa_en(0);
	cobra_pcrm_d2a_cpb_en(0);
	cobra_pcrm_d2a_vdd3_adc_en(1);
	cobra_pcrm_d2a_ldodig_pd0_high_curr_en(0);
	cobra_pcrm_d2a_ldodig_pd0_vreg_trim(0x0c);
	cobra_pcrm_d2a_ldodig_pd1_high_curr_en(0);
	cobra_pcrm_d2a_ldodig_pd1_vreg_trim(0);
	cobra_pcrm_d2a_efbrwot_en(0);
	cobra_pcrm_d2a_bgln_t_trim(0);
	cobra_pcrm_d2a_ldoanaforpll_high_curr_en(0);
	cobra_pcrm_d2a_ldoanaforpll_trim(0);
	
	cobra_pcrm_apll_control_status_clr(0);
	cobra_pcrm_apll_control_status_clr(1);
	
	cobra_pcrm_trim_start(0);
//channel sel
//	cobra_pcrm_write_apll_cfg2(0x475f7843);
	cobra_pcrm_apll_control_status_clr(0);
	cobra_pcrm_set_try_num(0x0f);
	cobra_pcrm_trim_start(1);
	cobra_pcrm_ctrl_cnt_cfg(0x1f);
	cobra_pcrm_wait_trim_cycle(0x0d);

	cobra_pcrm_trim_gain(trim_gain);
	cobra_pcrm_set_pll_cnt_judge(pll_cnt_judge);
	
//	cobra_pcrm_write_apll_cfg3(0x01fe0c7d);
	
////	
	cobra_pcrm_trim_value(0x7d);
	cobra_pcrm_trim_cfg_update(0);
	cobra_pcrm_trim_en_ctrl(0);
	cobra_pcrm_trim_en_pll_cnt(1);
	cobra_pcrm_step_value(1);
	cobra_pcrm_lock_detect_en(0);
	cobra_pcrm_lock_cfg(0xff);
	
	cobra_pcrm_pll_freq_off(0x184);
	
	cobra_pcrm_trim_cfg_update(1);
	cobra_pcrm_trim_start(1);
	//printf("cobra_pcrm_pll_trim_end 1 \n");
	while(!cobra_pcrm_pll_trim_end())
	{
		
	}
//	printf("cobra_pcrm_pll_trim_end ok \n");
	
	cobra_pcrm_d2a_pll_cp_en(1);
	cobra_pcrm_d2a_pll_pfd_en(1);
	
	cobra_pcrm_work_mode(0x01);
	cobra_pcrm_update_cfg(0);
	cobra_pcrm_p_num(p_num);
	cobra_pcrm_fract_num(fract_num);
//step 9
	cobra_pcrm_update_cfg(0);
	cobra_pcrm_update_cfg(1);
//step 10	
	cobra_soft_delay_us(50);
//step 11	
	cobra_pcrm_lock_detect_en(1);
	
	cobra4_pll_debug_information();
	
   return 0;
}


#endif















