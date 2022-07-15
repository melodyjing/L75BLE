/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  rdw_adc.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/6/8

	Description :   
///////////////////
//≤Œ’’Œƒµµ COMPAB_cobra4_aux_sensor_and_bor_ate_flow.xlsx Œƒµµ

	Changed Log	:

		Liujian  2021/6/8	- Creation

*****************************************************************************/

#if (COBRA_APP_ADC_DATA_EN)
#include "rdw_adc.h"
#include "cobra4C_eflash_ctrl.h"


/*
pd0_sel:
  0: 1.0v by ldo_perm with bgp_lp
  1: 1.2v by ldo_dig0 with bgp_ln
ref_sel:
  0:ldo_adc ref from bgp_lp
  1:ldo_adc ref from bgp_ln
*/
void  rdw_adc_set_adc_power(ADC_PD0_SEL_TYPE pd0_sel, ADC_REF_SEL_TYPE ref_sel)
{
	  cobra4_aon_pd_top_protection_en(1);
	  //1. switch on sw_adc3v3
	  cobra4_aon_pd_top_d2a_vdd3_adc_en(1);
	  //2. configure for ldoadc and gpadc work mode,set d2a_vr3v3to1v8_1v2ref_sel=ref_sel
	  cobra4_aon_pd_top_d2a_vr3v3to1v8_ldoadc_en(1);
	  
	  if(ref_sel == REF_SEL_BLP_LN)
		{
	     cobra4_aon_pd_top_d2a_vr3v3to1v8_1v2ref_sel(1);
	  }
    else if(ref_sel == REF_SEL_BGP_LP)
    {
       cobra4_aon_pd_top_d2a_vr3v3to1v8_1v2ref_sel(0); 
		}	
		
	  cobra4_aon_pd_top_protection_en(0);
	
	  cobra4_aon_wk_protection_en(1);

	  cobra4_aon_wk_pd0_sw_perm_manu_enable(1);
		
		if(pd0_sel == PD0_SEL_BGP_LN)   //1.2v
		{
			  cobra4_aon_wk_pd0_sw_ldo0_manu_enable(1);
		}
		else if(pd0_sel == PD0_SEL_BGP_LP)    //1.0v
		{
			  cobra4_aon_wk_pd0_sw_ldo0_manu_enable(0);
		}
	  cobra4_aon_wk_protection_en(0);
	
}

/*
clk_src:
    0:RCO  
    1:XO

adc_rate:
    0:8M     clk_src 1: XO48M    div=6 to get 8M
             clk_src 0: RCO24M   div=3 to get 8M

    1:32K    clk_src 1: XO32K
             clk_src 0: RCO32K

*/
void  rdw_adc_set_adc_clk(ADC_RATE_TYPE adc_rate, ADC_CLK_SRC_TYPE clk_src)
{
	
	  cobra4_aon_wk_protection_en(1);
	  cobra4_aon_pd_top_protection_en(1);
	  if(adc_rate == ADC_RATE_8M)
		{
			  cobra4_aon_pd_top_d2a_btgpadc_32k_mode_en(0);
			
			  if(clk_src == CLK_SRC_RCO)   //24M
				{
					 cobra4_aon_pd_top_gpadc_clock_selection(GPADC_RC_OSC24M);
					 cobra4_aon_pd_top_aon_hclk_divider(1);
				}
				else if(clk_src == CLK_SRC_XO) //48M
				{
					cobra4_aon_pd_top_gpadc_clock_selection(GPADC_XTAL48M);
					cobra4_aon_pd_top_aon_hclk_divider(1);
				}
		}
		else if(adc_rate == ADC_RATE_32K)
		{
			 cobra4_aon_pd_top_d2a_btgpadc_32k_mode_en(1);
			 cobra4_aon_pd_top_an_clk_gpadc_32k_gate_en(1);
			
			 if(clk_src == CLK_SRC_RCO)   //rco32k
			 {
					 cobra4_aon_pd_top_gpadc_clock_selection(GPADC_RC_OSC24M);
				   cobra4_aon_wk_aon_csem_clk_selection(CSEM_CLK_RCOSC32K);
			 }
			 else if(clk_src == CLK_SRC_XO) //xtal32k
			 {
					cobra4_aon_pd_top_gpadc_clock_selection(GPADC_XTAL48M);
				  cobra4_aon_wk_aon_csem_clk_selection(CSEM_CLK_XTAL32K);
			 }
		}
		
		cobra4_aon_pd_top_protection_en(0);
		cobra4_aon_wk_protection_en(0);
	 	
}


void cobra4_gpadc_get_adc_data_init_gpio_pos(DIFFERENTIAL_POS_INPUT_DPIN pin)
{
	     switch(pin)
			 {
				 case DIFF_POS_DPIN24:
					 app_m3_gpio_set_direction(GPIO_PIN_24, GPIO_HIGH_Z);
					 break;
				 case DIFF_POS_DPIN25:
					 app_m3_gpio_set_direction(GPIO_PIN_25, GPIO_HIGH_Z);
					 break;
				 case DIFF_POS_DPIN26:
					 app_m3_gpio_set_direction(GPIO_PIN_26, GPIO_HIGH_Z);
					 break;
				 case DIFF_POS_DPIN27:
					 app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_HIGH_Z);
					 break;
				 case DIFF_POS_DPIN28:
					 app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_HIGH_Z);
					 break;
				 case DIFF_POS_DPIN29:
					 app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_HIGH_Z);
					 break;
				 case DIFF_POS_DPIN30:
					 app_m3_gpio_set_direction(GPIO_PIN_30, GPIO_HIGH_Z);
					 break;
				 case DIFF_POS_DPIN31:
					 app_m3_gpio_set_direction(GPIO_PIN_31, GPIO_HIGH_Z);
					 break;
				 default:
					 break;
			 }
}

static void cobra4_gpadc_get_adc_data_init_gpio_neg(DIFFERENTIAL_NEG_INPUT_DPIN pin)
{
	     switch(pin)
			 {
				 case DIFF_NEG_DPIN24:
					 app_m3_gpio_set_direction(GPIO_PIN_24, GPIO_HIGH_Z);
					 break;
				 case DIFF_NEG_DPIN25:
					 app_m3_gpio_set_direction(GPIO_PIN_25, GPIO_HIGH_Z);
					 break;
				 case DIFF_NEG_DPIN26:
					 app_m3_gpio_set_direction(GPIO_PIN_26, GPIO_HIGH_Z);
					 break;
				 case DIFF_NEG_DPIN27:
					 app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_HIGH_Z);
					 break;
				 case DIFF_NEG_DPIN28:
					 app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_HIGH_Z);
					 break;
				 case DIFF_NEG_DPIN29:
					 app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_HIGH_Z);
					 break;
				 case DIFF_NEG_DPIN30:
					 app_m3_gpio_set_direction(GPIO_PIN_30, GPIO_HIGH_Z);
					 break;
				 case DIFF_NEG_DPIN31:
					 app_m3_gpio_set_direction(GPIO_PIN_31, GPIO_HIGH_Z);
					 break;
				 default:
					 break;
			 }
}

void rdw_adc_single_adc_mode_select_channel(DIFFERENTIAL_POS_INPUT_DPIN pin)
{
	  cobra4_aon_pd_top_protection_en(1);
	  cobra4_aon_pd_top_d2a_btgpadc_vpin_sel(pin);
//	  cobra4_gpadc_get_adc_data_init_gpio_pos(pin);	
	  cobra4_aon_pd_top_protection_en(0);
	
}

void rdw_adc_diff_adc_mode_select_channel(DIFFERENTIAL_POS_INPUT_DPIN pin1, 
                                     DIFFERENTIAL_NEG_INPUT_DPIN pin2)
{
	  cobra4_aon_pd_top_protection_en(1);
	
	  cobra4_aon_pd_top_d2a_btgpadc_vpin_sel(pin1);
		cobra4_aon_pd_top_d2a_btgpadc_diff(pin2, 1);
//		cobra4_gpadc_get_adc_data_init_gpio_pos(pin1);
//	  cobra4_gpadc_get_adc_data_init_gpio_neg(pin2);
	
	  cobra4_aon_pd_top_protection_en(0);
	
}

void rdw_adc_get_adc_data_init( GPADC_MODE gpadc_mode, 
	                                   DIFF_MODE diff, \
                                     ADC_GAIN  gain,
	                                   DIFFERENTIAL_POS_INPUT_DPIN pin1, \
                                     DIFFERENTIAL_NEG_INPUT_DPIN pin2)
{
	  uint8_t  value;
	
	  cobra4_aon_pd_top_protection_en(1);
	
	  cobra4_aon_pd_top_d2a_btgpadc_mode(gpadc_mode);
	
	  cobra4_aon_pd_top_d2a_btgapadc_en_gpadc_en(1);
	
	  if(gpadc_mode == ADC_MODE_NORMAL)
		{
	      if(diff == ADC_SINGLE_END_MODE)
		    {
			       cobra4_aon_pd_top_d2a_btgpadc_vpin_sel(pin1);
			       cobra4_gpadc_get_adc_data_init_gpio_pos(pin1);	
             cobra4_aon_pd_top_d2a_btgpadc_diff(pin1, 0);					
		    }
		    else if(diff == ADC_DIFFERENTIAL_MODE)
		    {
			       cobra4_aon_pd_top_d2a_btgpadc_vpin_sel(pin1);
			       cobra4_aon_pd_top_d2a_btgpadc_diff(pin2, 1);
			       cobra4_gpadc_get_adc_data_init_gpio_pos(pin1);
			       cobra4_gpadc_get_adc_data_init_gpio_neg(pin2);
		    } 
	  }

		
		  value = 0;
			switch(gain)
			{
				case ADC_GAIN_1X:
					value = 0;
					break;
				case ADC_GAIN_4X:
					if(diff == ADC_DIFFERENTIAL_MODE)
					{
					   value = 6;
					}
					break;
				case ADC_GAIN_2X:
					if(diff == ADC_DIFFERENTIAL_MODE)
					{
					    value = 5;
					}
					break;
				case ADC_GAIN_DIV_4:
					value = 2;
					break;
				case ADC_GAIN_DIV_2:
					value = 1;
					break;
					default:
						break;
		  }
	  
			cobra4_aon_pd_top_d2a_btgpadc_adcgain(value);
			cobra4_aon_pd_top_protection_en(0);
		
}

void rdw_adc_fifo_init(void)
{
	  cobra4_gpadc_fifo_overflow_clr( 1);
	  cobra4_gpadc_fifo_underflow_clr( 1);
	
		cobra4_gpadc_fifo_underflow_clr( 0);
		cobra4_gpadc_fifo_overflow_clr( 0);
		cobra4_gpadc_cfg_debounce_sample_points(4);
		cobra4_gpadc_cfg_debounce_filter_en(1);
		cobra4_gpadc_cfg_debounce_filter_clk_divide_vld(1);
		cobra4_gpadc_cfg_debounce_filter_clock_count_max(8);
	  cobra4_gpadc_cfg_comparator_b_in_mode(0x00);
		cobra4_gpadc_fifo_wen_enable(1);
		cobra4_gpadc_fifo_enable(1);
	
}

void rdw_adc_test(void)
{
	   unsigned int temp;
	   float        xxx;
	//   unsigned int chipid[4];
	//   cobra_information_flash_read(0x0040, 16, (uint8_t *)chipid);
	   cobra4_aon_pd_top_protection_en(1);
	//   cobra4_aon_pd_top_d2a_vr3v3to1v8_votrim(0);
	   cobra4_aon_pd_top_d2a_anatmux_en(1);
	   cobra4_aon_pd_top_d2a_anatmux_sel(10);
	//   cobra4_aon_pd_top_d2a_bgln_vref_trim(1);
	   cobra4_aon_pd_top_protection_en(0);
	
	   rdw_adc_set_adc_power(PD0_SEL_BGP_LN, REF_SEL_BLP_LN);
	   rdw_adc_set_adc_clk(ADC_RATE_8M, CLK_SRC_XO);
	
	   rdw_adc_get_adc_data_init(ADC_MODE_NORMAL, ADC_SINGLE_END_MODE, ADC_GAIN_DIV_2, DIFF_POS_DPIN29, DIFF_NEG_DPIN_MAX);
	
	   rdw_adc_fifo_init();
	   
		 while(1)
		 {
			  temp = cobra4_gpadc_read_adc_data();
			  xxx = temp;
			  xxx /= 2048.0;
			  xxx *= 3.55;
			  printf("adc = %d \n", temp);			
		 }
		
}



#endif