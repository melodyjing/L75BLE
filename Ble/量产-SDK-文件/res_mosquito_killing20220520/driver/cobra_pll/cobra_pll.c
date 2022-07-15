#include <cobra_config.h>
#include "cobra_pll.h"
#include "cobra_pcrm.h"
#include "reg_access.h"
#include "cobra_uart.h"
#include "cobra_timer.h"
#include "cobra4_aon_wk.h"

#if (COBRA_PLL_VER2_EN)
#define TRIM_FREQ_STEP  50.0


//void pll_test_pin_cfg(void)
//{
//	uint32_t reg_value = REG_PL_RD(PCRM_BASE+0x100);
//	reg_value |= 0x100;															//d_pll_test_en
//	REG_PL_WR(PCRM_BASE+0x100,reg_value);
//	
//	reg_value = REG_PL_RD(PCRM_BASE+0x108);
//	reg_value = (reg_value & ~0x30000) | 0x20000;			//d_tmux[1:0]=1  0x20000 pll voltage  0x10000  clk  0x30000 adc voltage
//	reg_value |= 0x80;
//	REG_PL_WR(PCRM_BASE+0x108,reg_value);
//	
//	reg_value = REG_PL_RD(PCRM_BASE+0x10C);
//	reg_value |= 0x20;
//	REG_PL_WR(PCRM_BASE+0x10C,reg_value);	
//	
//	//?DPIN26???floating??,?????,?????:
//	reg_value = REG_PL_RD(PINMUX_BASE+0x68);
//	reg_value &= ~0x80;		//IE=0
//	reg_value &= ~0x10;		 //pull up/down invalid
//	reg_value = (reg_value&~0xf)|2;		 //cfg3, as input pad
//	REG_PL_WR(PINMUX_BASE+0x68,reg_value);		
//}

//void close_8m(void)
//{
//	REG_PL_WR(PMU_BASE+0x100,0x1);
//	uint32_t reg_value = REG_PL_RD(PMU_BASE+0x11C);
////	printf("0x104 %x\n",reg_value);
//	reg_value &= ~0x3;
//	reg_value |= 0x2;
//	REG_PL_WR(PMU_BASE+0x11C,reg_value);
//	REG_PL_WR(PMU_BASE+0x100,0x0);
//	reg_value = REG_PL_RD(PMU_BASE+0x104);
////	printf("0x104 %x\n",reg_value);
//}


int set_pll_frenquence(double pll_target)
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
    int   flag = 0,delay = 0;

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

//    uint32_t reg_value;

//    printf("APLL_STATUS %x\n",REG_PL_RD(PCRM_BASE+APLL_STATUS_REG));
    
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL0_REG);
//    reg_value |= (D2A_PLL_PFD_EN);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL0_REG,reg_value);
    cobra4_aon_wk_protection_en(1);
    cobra4_aon_wk_d2a_ldoanaforpll_en(1);
    cobra_pcrm_d2a_pll_pfd_en(1);
    
    cobra4_aon_wk_apll_isolate0_en(1);
    cobra4_aon_wk_protection_en(0);
    //1. ??analog?????,??:??disable?PFD/CP/LPF,??disable?LPF,??chan 
    
    cobra_pcrm_d2a_pll_cp_en(0);
    cobra_pcrm_d2a_pll_en(1);
    cobra_pcrm_d2a_pll_lct_en(1);
    cobra_pcrm_d2a_pll_pfd_en(0);
    cobra_pcrm_d2a_pll_retimer_en(0);
    cobra_pcrm_d2a_pll_vco_en(1);
    cobra_pcrm_d2a_pll_lpf_c1(0xA);
    
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL0_REG);
//    reg_value &= ~(D2A_PLL_CP_EN);
//    reg_value |= (D2A_PLL_EN);
//    reg_value |= (D2A_PLL_LCT_EN);
//    reg_value &= ~(D2A_PLL_PFD_EN);
//    reg_value |= (D2A_PLL_RETIMER_EN);
//    reg_value |= (D2A_PLL_VCO_EN);
//    reg_value &= ~(D2A_PLL_LPF_C1_MASK);
//    reg_value |= 0xA<<D2A_PLL_LPF_C1_POW;
//    REG_PL_WR(PCRM_BASE+APLL_CTRL0_REG,reg_value);
    
    //cobra_pcrm_ana_cfg0(0x188);
    cobra_pcrm_d2a_ldodig_pd0_vreg_trim(12);
    cobra_pcrm_d2a_vdd3_adc_en(1);
//    REG_PL_WR(PCRM_BASE+ANA_CFG0_REG,0x188);
    //2.??apll_cfg2??status_clr?0,????status_clr???1?
    
    cobra_pcrm_apll_control_status_clr(0);
    cobra_pcrm_apll_control_status_clr(1);
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL1_REG);
//    reg_value &=~(STATUS_CLR_MASK);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL1_REG,reg_value);

//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL1_REG);
//    reg_value |=(STATUS_CLR_MASK);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL1_REG,reg_value);

    //3.??channel_sel??
    cobra_pcrm_set_try_num(0xf);
    cobra_pcrm_set_pll_cnt_judge(pll_cnt_judge);
    cobra_pcrm_trim_gain(trim_gain);
    cobra_pcrm_ctrl_cnt_cfg(0x1f);
    cobra_pcrm_wait_trim_cycle(0xd);
    
    cobra_pcrm_trim_value(0x7d);
    cobra_pcrm_trim_en_pll_cnt(1);
    cobra_pcrm_step_value(0x1);
    cobra_pcrm_lock_cfg(0xff);
    
//    reg_value = 0x475f7843;
//    reg_value &= ~0x3FF;
//    reg_value |= pll_cnt_judge;
//    reg_value &= ~(TRIM_GAIN_MASK);
//    reg_value |= trim_gain<<TRIM_GAIN_POS;
//    REG_PL_WR(PCRM_BASE+APLL_CTRL1_REG,reg_value);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL2_REG,0x01fe0c7d);

    cobra_pcrm_pll_freq_off(0x184);
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL3_REG);
//    reg_value = (reg_value & ~0x1ff) | 0x184;  //0x184
//    REG_PL_WR(PCRM_BASE+APLL_CTRL3_REG,reg_value);

    //4.?trim_cfg_update???1(?????0),??channel sel????.
    
    cobra_pcrm_trim_cfg_update(0);
    cobra_pcrm_trim_cfg_update(1);
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL2_REG);
//    reg_value = reg_value & ~(TRIM_CFG_UPDATE_MASK);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL2_REG,reg_value);

//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL2_REG);
//    reg_value |=(TRIM_CFG_UPDATE_MASK);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL2_REG,reg_value);	

    //5.?trim_start???1,??channel selection???
    cobra_pcrm_trim_start(0);
    cobra_pcrm_trim_start(1);
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL1_REG);
//    reg_value = reg_value & ~(TRIM_START_MASK);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL1_REG,reg_value);

//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL1_REG);
//    reg_value |=(TRIM_START_MASK);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL1_REG,reg_value);		

    //6.status_register_1?????trim_end??,trim???????
    while(!cobra_pcrm_pll_trim_end());
//    while(!(REG_PL_RD(PCRM_BASE+APLL_STATUS_REG)&TRIM_END_MASK));

//    pll_test_pin_cfg();

    //7.enable PFD/CP/LPF,??PLL close loop
    
    cobra_pcrm_d2a_pll_cp_en(1);
    cobra_pcrm_d2a_pll_pfd_en(1);
    
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL0_REG);
//    reg_value |=D2A_PLL_CP_EN;
//    reg_value |=D2A_PLL_PFD_EN;
//    //		reg_value &= ~0x40;
//    //		reg_value |=(PLL_EN_COM|PLL_MS|PLL_SMS|PLL_SW_MODE);
//    //		reg_value &= ~(PLL_EN_COM|PLL_MS|PLL_SMS|PLL_SW_MODE);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL0_REG,reg_value);	

    //		REG_PL_WR(APLL_CFG1,0xe021c70);

    //8.sdm_ctrl_register_1sdm_ctrl_register_2??update_cfg(update_cfg0)
    
    cobra_pcrm_p_num(p_num);
    cobra_pcrm_work_mode(0x1);
    cobra_pcrm_fract_num(fract_num);
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL3_REG);

//    reg_value &=~(0x7f<<SDM_CTRL_REG_2_POS);
//    reg_value |=(p_num)<<SDM_CTRL_REG_2_POS;
//    reg_value |= 0x01<<WORK_MODE_POS;
//    //		reg_value &= ~(0x03<<WORK_MODE_POS);
//    REG_PL_WR(PCRM_BASE+APLL_CTRL3_REG,reg_value);		

//    REG_PL_WR(PCRM_BASE+APLL_CTRL4_REG,fract_num);	

    //9.??sdm_ctrl_register_2??update_cfg?1
    cobra_pcrm_update_cfg(0);
    cobra_pcrm_update_cfg(1);
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL3_REG);
//    reg_value &= ~SDM_UPDATE_CFG;
//    REG_PL_WR(PCRM_BASE+APLL_CTRL3_REG,reg_value);		

//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL3_REG);
//    reg_value |=SDM_UPDATE_CFG;
//    REG_PL_WR(PCRM_BASE+APLL_CTRL3_REG,reg_value);		

    //10.??50us,??pll????lock?????
    for(delay = 0; delay < 2400; delay++);

    //11.??lock_detect_en=1??:?COBRA??,??????unlock,???????lock??
    cobra_pcrm_lock_detect_en(1);
//    reg_value = REG_PL_RD(PCRM_BASE+APLL_CTRL2_REG);
//    reg_value |=LOCK_DETECT_EN_MASK;
//    REG_PL_WR(PCRM_BASE+APLL_CTRL2_REG,reg_value);	

#ifdef PRINT_REG
    printf("0x9C:%x\n",REG_PL_RD(0x400e209C));
    printf("0xA0:%x\n",REG_PL_RD(0x400e20a0));
    printf("0xA4:%x\n",REG_PL_RD(0x400e20a4));
    printf("0xA8:%x\n",REG_PL_RD(0x400e20a8));
    printf("0x100:%x\n",REG_PL_RD(0x400e2100));
    printf("0x138:%x\n",REG_PL_RD(0x400e2138));
#endif

    return 0;
}
#endif