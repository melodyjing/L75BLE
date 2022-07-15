/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.	
	Author:Liujian
	cobra Drivers
	File Name	:	cobra4_audio.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/2/22
	Description :   
	Changed Log	:
		Liujian  2020/2/22			- Creation
*****************************************************************************/
#include "cobra4_audio.h"
#include "cobra4_aon_wk.h"
#include "cobra4_pll.h"
#include "cobra_pinmux.h"
#include <math.h>
#if COBRA4_AUDIO_EN

#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA4_AUDIO_TypeDef * cobra4_audio		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA4_AUDIO_TypeDef * cobra4_audio;
#endif

void cobra4_audio_init(void)
{
	cobra4_audio =  (volatile COBRA4_AUDIO_TypeDef *)COBRA4_AUDIO_BASE;
}

/*
"1:  clear the high-speed fir data ram
 0:  not clear"
*/
void cobra4_audio_cfg_high_speed_firdataram_clear_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<22);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<22);
		}	
}

/*
"1:  agc_ram clock is hclk
 0:  agc_ram clock is from mclk"
*/
void cobra4_audio_cfg_agc_ram_clk_is_hclk_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<21);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<21);
		}	
}

/*
"1:  the high-speed fir fast clock is hclk
 0:  the high-speed fir fast clock is from mclk"

*/
void cobra4_audio_cfg_fast_clk_is_hclk_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<20);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<20);
		}	
}

/*
"1:  enable the mclk
 0:  disable the mclk"
*/
void cobra4_audio_cfg_clk_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<19);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<19);
		}	
}

/*
"1:  mclk is from the xtal
 0:  mclk is from the pll"
*/

void cobra4_audio_cfg_xtal1_pll0_sel(COBRA4_SRC_CLK clk)
{
		if(clk == MCLK_FROM_XTAL)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<18);
		}
		else if(clk == MCLK_FROM_PLL)
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<18);
		}	
}


/*
"1: adc right clk is for left channel, and adc left clk is for right channel 
 0: adc right clk is for right channel or adc left clk is for left channel"

*/

COBRA4_ADC_SWAP cobra4_audio_cfg_audioadc_stero_mode(void)
{
		if(cobra4_audio->audio_cfg_ctrl & (1 << 17))
		{
			 return ADC_AUDIO_SWAP_TRUE;
		}
		else
		{
			return ADC_AUDIO_SWAP_FALSE;
		}
}

/*
"1:  the i2s interface is enabled
 0:  the i2s interface is disabled"
*/
void cobra4_audio_cfg_i2s_enable(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<12);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<12);
		}	
}


/*
"1:  the audio is mute
 0:  the audio is normal"
*/
void cobra4_audio_cfg_audio_mute_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<11);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<11);
		}	
}

/*
"1:  the right channel is from left channel
 0:  the right channel is normal"
*/
void cobra4_audio_cfg_right_copy_left_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<10);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<10);
		}	
}

/*
"1:  the left channel is from right channel
 0:  the left channel is normal"
*/
void cobra4_audio_cfg_left_copy_right_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<9);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<9);
		}	
}

/*
"1:  the iir_group filter is as decimation filter
 0:  the iir_group filter is after decimation filter, as notch, low-pass and so on."
*/
void cobra4_audio_cfg_iir_group_as_decimation_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<8);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<8);
		}	
}



/*
"1:  the high-pass iir (del the dc) is enabled
 0:  the high-pass iir (del the dc) is disabled"
*/
void cobra4_audio_cfg_hp_iir_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<6);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<6);
		}	
}


/*
"1:  the iir-group-filters are enabled
 0:  the iir-group-filters are disabled"
*/
void cobra4_audio_cfg_iir_group_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<5);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<5);
		}	
}

/*
"1:  left channel become right channel
     right channel become left channel
0:   normal"
*/
void cobra4_audio_cfg_left_right_swap(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<4);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<4);
		}	
}


/*
"1:  the right channel are enabled
 0:  the right channel are disabled"
*/
void cobra4_audio_cfg_right_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<3);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<3);
		}	
}

/*
"1:  the left channel are enabled
 0:  the left channel are disabled"
*/
void cobra4_audio_cfg_left_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<2);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<2);
		}	
}


/*
"1:  support pdm interface
 0:  support adc interface "
*/
void cobra4_audio_cfg_adc0_pdm1(COBRA4_SRC_TYPE type)
{
		if(type == PDM_INTERFACE)
		{
				cobra4_audio->audio_cfg_ctrl |= (1<<1);
		}
		else if(type == ADC_INTERFACE)
		{
				cobra4_audio->audio_cfg_ctrl &= ~(1<<1);
		}	
}


/*
"1:  the adc intf fifo is enabled
 0:  the adc intf fifo is disabled"
*/
void cobra4_audio_cfg_intf_fifo_enable(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_ctrl |= (0x01);
		}
		else
		{
				cobra4_audio->audio_cfg_ctrl &= ~(0x01);
		}	
}

/////
/*
	cfg_osr_by_mclk = mclk/clk_1fs
*/
void cobra4_audio_cfg_osr_by_mclk(unsigned int value)
{
		volatile unsigned int temp;
		temp = cobra4_audio->audio_cfg_mclk; 
	
		temp &= ~(0x7ff << 16);
		temp |= ((value&0x7ff) << 16);
		cobra4_audio->audio_cfg_mclk = temp; 
}

/*
	cfg_mclk_ratio =clk_xtal1_or_pll/mclk
*/
void cobra4_audio_cfg_mclk_ratio(unsigned int value)
{
		volatile unsigned int temp;
		temp = cobra4_audio->audio_cfg_mclk; 
	
		temp &= ~(0xffff);
		temp |= (value&0xffff);
		cobra4_audio->audio_cfg_mclk = temp; 
}

/*
The delay phase is the cycle number of clk_xtal1_or_pll

*/
void cobra4_audio_cfg_pdm_data_delay_phase(unsigned int value)
{
		volatile unsigned int temp;
		temp = cobra4_audio->audio_cfg_pdm; 
	
		temp &= ~(0x0f << 4);
		temp |= ((value&0x0f) << 4);
		cobra4_audio->audio_cfg_pdm = temp; 
}

/*
"1:  left 2 wire only support left-mono, right 2 wire only support right-mono
 0:  left 2 wire or right 2 wire support stero. (see the cfg_pdm_left_two_wire_is_stereo  advancely)"
*/
void cobra4_audio_cfg_pdm_four_wire_is_2mono(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_pdm |= (0x01 << 3);
		}
		else
		{
				cobra4_audio->audio_cfg_pdm &= ~(0x01 << 3);
		}	
}


/*
"when cfg_pdm_four_wire_is_2mono =0,
1:  left 2 wire is stero  ,pdm input is from pdm_left_data_in_pin
1:  right 2 wire is stero ,pdm input is from pdm_right_data_in_pin"
*/
void cobra4_audio_cfg_pdm_left_two_wire_is_stereo(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_pdm |= (0x01 << 2);
		}
		else
		{
				cobra4_audio->audio_cfg_pdm &= ~(0x01 << 2);
		}	
}

/*
"1: right channel use  posedge of pdm clk
 0: right channel use  negedge of pdm clk  
(when cfg_left_right_swap,the left channel become the right channel,but this channel still use cfg_pdm_left_posedge )"

*/
void cobra4_audio_cfg_pdm_right_posedge(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_pdm |= (0x01 << 1);
		}
		else
		{
				cobra4_audio->audio_cfg_pdm &= ~(0x01 << 1);
		}	
}


/*
"1: left channel use  posedge of pdm clk
 0: left channel use  negedge of pdm clk
(when cfg_left_right_swap,the left channel become the right channel,but this channel still use cfg_pdm_left_posedge )"

*/
void cobra4_audio_cfg_pdm_left_posedge(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_pdm |= (0x01 << 0);
		}
		else
		{
				cobra4_audio->audio_cfg_pdm &= ~(0x01 << 0);
		}	
}

//////

/*
"1: the msb of the right channel data is changed from 1 to 0 ,or from 0 to 1
 0: normal"
*/
void cobra4_audio_cfg_adc_data_msb_swap_r(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_adc_data_intf |= (0x01 << 2);
		}
		else
		{
				cobra4_audio->audio_cfg_adc_data_intf &= ~(0x01 << 2);
		}	
}

/*
"1: the msb of the left channel data is changed from 1 to 0 ,or from 0 to 1
 0: normal"
*/
void cobra4_audio_cfg_adc_data_msb_swap_l(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_adc_data_intf |= (0x01 << 1);
		}
		else
		{
				cobra4_audio->audio_cfg_adc_data_intf &= ~(0x01 << 1);
		}	
}

/*
"1: the output of the adc_intf is raw_data of adc
 0: the output of the adc_intf is signed data"
*/
void cobra4_audio_cfg_adc_raw_data_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_adc_data_intf |= (0x01 << 0);
		}
		else
		{
				cobra4_audio->audio_cfg_adc_data_intf &= ~(0x01 << 0);
		}	
}

/////

/*
cfg_upfifo_almostempty_interrupt_mask 
"1: mask
 0: unmask"
*/
void cobra4_audio_cfg_upfifo_almostempty_interrupt_mask_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask |= (0x01 << 5);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask &= ~(0x01 << 5);
		}	
}

/*
cfg_upfifo_almostfull_interrupt_mask  
"1: mask
 0: unmask"
*/
void cobra4_audio_cfg_upfifo_almostfull_interrupt_mask_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask |= (0x01 << 4);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask &= ~(0x01 << 4);
		}	
}

/*
cfg_upfifo_empty_interrupt_mask  
"1: mask
 0: unmask"
*/
void cobra4_audio_cfg_upfifo_empty_interrupt_mask_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask |= (0x01 << 3);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask &= ~(0x01 << 3);
		}	
}

/*
cfg_upfifo_overflow_interrupt_mask  
"1: mask
 0: unmask"
*/
void cobra4_audio_cfg_upfifo_overflow_interrupt_mask_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask |= (0x01 << 2);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask &= ~(0x01 << 2);
		}	
}

/*
cfg_upfifo_underflow_interrupt_mask  
"1: mask
 0: unmask"
*/
void cobra4_audio_cfg_upfifo_underflow_interrupt_mask_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask |= (0x01 << 1);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask &= ~(0x01 << 1);
		}	
}

/*
cfg_upfifo_full_interrupt_mask 
"1: mask
 0: unmask"
*/
void cobra4_audio_cfg_upfifo_full_interrupt_mask_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask |= (0x01 << 0);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_mask &= ~(0x01 << 0);
		}	
}

/////////////////////////////////////////////////////////////
/*
"1: sticky interrupt is enabled
 0: sticky interrupt is disabled"
*/
void cobra4_audio_cfg_stciky_int_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_interrupt_sticky |= (0x01 << 30);
		}
		else
		{
				cobra4_audio->audio_cfg_interrupt_sticky &= ~(0x01 << 30);
		}	
}
/*
[0]: cfg_upfifo_full_interrupt_clr (1: clear 0: normal)
[1]: cfg_upfifo_under_interrupt_clr
[2]: cfg_upfifo_overflow_interrupt_clr
[3]: cfg_upfifo_empty_interrupt_clr
[4]: cfg_upfifo_almostfull_interrupt_clr
[5]: cfg_upfifo_almostempty_interrupt_clr
"
*/

//cfg_upfifo_full_interrupt_clr
void cobra4_audio_cfg_upfifo_full_interrupt_clr_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_interrupt_sticky |= (0x01 << 24);
		}
		else
		{
				cobra4_audio->audio_cfg_interrupt_sticky &= ~(0x01 << 24);
		}	
}

//cfg_upfifo_under_interrupt_clr
void cobra4_audio_cfg_upfifo_under_interrupt_clr_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_interrupt_sticky |= (0x01 << 25);
		}
		else
		{
				cobra4_audio->audio_cfg_interrupt_sticky &= ~(0x01 << 25);
		}	
}

//cfg_upfifo_overflow_interrupt_clr
void cobra4_audio_cfg_upfifo_overflow_interrupt_clr_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_interrupt_sticky |= (0x01 << 26);
		}
		else
		{
				cobra4_audio->audio_cfg_interrupt_sticky &= ~(0x01 << 26);
		}	
}

//cfg_upfifo_empty_interrupt_clr
void cobra4_audio_cfg_upfifo_empty_interrupt_clr_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_interrupt_sticky |= (0x01 << 27);
		}
		else
		{
				cobra4_audio->audio_cfg_interrupt_sticky &= ~(0x01 << 27);
		}	
}
//cfg_upfifo_almostfull_interrupt_clr
void cobra4_audio_cfg_upfifo_almostfull_interrupt_clr_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_interrupt_sticky |= (0x01 << 28);
		}
		else
		{
				cobra4_audio->audio_cfg_interrupt_sticky &= ~(0x01 << 28);
		}	
}

//cfg_upfifo_almostempty_interrupt_clr
void cobra4_audio_cfg_upfifo_almostempty_interrupt_clr_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_interrupt_sticky |= (0x01 << 29);
		}
		else
		{
				cobra4_audio->audio_cfg_interrupt_sticky &= ~(0x01 << 29);
		}	
}


/*
the duration  of a interrupt. Ex.4 second
*/
void cobra4_audio_cfg_int_timeout2(unsigned int value)
{
		volatile unsigned int temp;
	
		temp = cobra4_audio->audio_cfg_interrupt_sticky;
		temp &= ~(0xff << 16);
		temp |= ((value & 0xff) << 16);
		
		cobra4_audio->audio_cfg_interrupt_sticky = temp;
}
	
/*
generate the int_timer_clk
*/
void cobra4_audio_cfg_int_timeout1(unsigned int value)
{
		volatile unsigned int temp;
	
		temp = cobra4_audio->audio_cfg_interrupt_sticky;
		temp &= ~(0xffff);
		temp |= (value & 0xffff);
		
		cobra4_audio->audio_cfg_interrupt_sticky = temp;
	
}

//////////////////////
/*
"1: iir-group filters can be connected to a cascaded filter for left channel
 0: iir-group filters can be connected to a cascaded filter for left channel"
*/
void cobra4_audio_cfg_iir_group_10iir_for_which_ch_sel(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_iir_group |= (0x01 << 5);
		}
		else
		{
				cobra4_audio->audio_cfg_iir_group &= ~(0x01 << 5);
		}	
}

/*
"1: the input of last three filter in the iir-group is the input of iir-group
 0: normal"
*/
void cobra4_audio_cfg_iir_group_deci_only_use_last_3iir_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_iir_group |= (0x01 << 4);
		}
		else
		{
				cobra4_audio->audio_cfg_iir_group &= ~(0x01 << 4);
		}	
}

/*
" 0: the output of left channel is the left-channel-input of iir-group;the output of right channel is the right-channel-input of iir-group
	1: the output of left channel is iir0-filter output;the output of left channel is iir1-filter output;
	2: the output of left channel is iir2-filter output;the output of left channel is iir3-filter output;
	3: the output of left channel is iir4-filter output;the output of left channel is iir5-filter output;
	4: the output of left channel is iir6-filter output;the output of left channel is iir7-filter output;
	5: the output of left channel is iir8-filter output;the output of left channel is iir9-filter output;
	6: the output of mono channel is iir5-filter output
	7: the output of mono channel is iir7-filter output
	8: the output of mono channel is iir9-filter output"
*/
void cobra4_audio_cfg_iir_group_mode(COBRA4_IIR_GROUP_MODE mode)
{
		volatile unsigned int temp;
	
		temp = cobra4_audio->audio_cfg_iir_group;
	
		temp &= ~0x0f;
		if(mode == IIR_GROUP_MODE0)
		{
				temp |= 0x00;
		}
		else if(mode == IIR_GROUP_MODE1)
		{
				temp |= 0x01;
		}
		else if(mode == IIR_GROUP_MODE2)
		{
				temp |= 0x02;
		}
		else if(mode == IIR_GROUP_MODE3)
		{
				temp |= 0x03;
		}
		else if(mode == IIR_GROUP_MODE4)
		{
				temp |= 0x04;
		}
		else if(mode == IIR_GROUP_MODE5)
		{
				temp |= 0x05;
		}
		else if(mode == IIR_GROUP_MODE6)
		{
				temp |= 0x06;
		}
		else if(mode == IIR_GROUP_MODE7)
		{
				temp |= 0x07;
		}
		else if(mode == IIR_GROUP_MODE8)
		{
				temp |= 0x08;
		}
		
		cobra4_audio->audio_cfg_iir_group = temp;
}

/*
the depth of the dma-upfifo is 8. so the cfg_upfifo_almostempty_threshold can be from 0 to 8.
cfg_upfifo_almostempty_threshold 

*/
void cobra4_audio_cfg_upfifo_almostempty_threshold(unsigned int value)
{
		volatile unsigned int temp;
	
		temp = cobra4_audio->audio_cfg_upfifo_almost_threshold;
	
		temp &= ~(0x0f << 4);
		temp |= ((value&0x0f) << 4);
	
		cobra4_audio->audio_cfg_upfifo_almost_threshold = temp;
		
}

/*
the depth of the dma-upfifo is 8. so the cfg_upfifo_almostfull_threshold can be from 0 to 8.
cfg_upfifo_almostfull_threshold 
 
*/
void cobra4_audio_cfg_upfifo_almostfull_threshold(unsigned int value)
{
		volatile unsigned int temp;
	
		temp = cobra4_audio->audio_cfg_upfifo_almost_threshold;
	
		temp &= ~(0x0f);
		temp |= (value&0x0f);
	
		cobra4_audio->audio_cfg_upfifo_almost_threshold = temp;
		
}

/////////////////////////////////////////////

/*
"1: no-almostempty is used to trig the dma transfer
 0: no-almostempty is not used to trig the dma transfer"
*/
void cobra4_audio_cfg_dma_req_by_almostempty_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl |=  (1<<6);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl &=  ~(1<<6);
		}
}

/*
"1:  no-empty signal is used to trig the dma transfer
 0:  no-empty signal is not used to trig the dma transfer"
*/
void cobra4_audio_cfg_dma_req_by_empty_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl |=  (1<<5);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl &=  ~(1<<5);
		}
}

/*
"1:  if upfifo underflow is 1, stop dma
 0:  upfifo underflow can not be used to  stop dma"
*/
void cobra4_audio_cfg_dma_stop_by_upfifo_underflow_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl |=  (1<<4);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl &=  ~(1<<4);
		}
}

/*
"1:  if upfifo overflow is 1, stop dma
 0:  upfifo overflow can not be used to  stop dma"
*/
void cobra4_audio_cfg_dma_stop_by_upfifo_overflow_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl |=  (1<<3);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl &=  ~(1<<3);
		}
}

/*
"1:  when upfifo overflow or underflow is high, the dma_req become low.
 0:  dma_req is not related with overflow and underflow"
*/
void cobra4_audio_cfg_dma_error_signal_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl |=  (1<<2);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl &=  ~(1<<2);
		}
}

/*
"1. up_to_cpu dma is enabled
 0: up_to_cpu dma is disabled"
*/
void cobra4_audio_cfg_upfifo_dma_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl |=  (1<<1);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl &=  ~(1<<1);
		}
}

/*
"1. upfifo is enabled
 0: upfifo is disabled"
*/
void cobra4_audio_cfg_upfifo_en(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl |=  (1<<0);
		}
		else
		{
				cobra4_audio->audio_cfg_dma_upfifo_ctrl &=  ~(1<<0);
		}
}

///////////////////////////////////////////
/*
cicoutbitwidth is 42bits. But the internal bus is DW bits.
so we can shrink th 42bits to DB bits by this register
*/

void cobra4_audio_cfg_cicout_shrink(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_cic;
		temp &= ~(0x3f << 9);
		temp |= ((value & 0x3f) << 9);
	
		cobra4_audio->audio_cfg_cic = temp;
	
}

/*
its range is from 0 to 5

*/
void cobra4_audio_cfg_cic_stages(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_cic;
		temp &= ~(0xf << 4);
		temp |= ((value & 0xf) << 4);
	
		cobra4_audio->audio_cfg_cic = temp;
	
}

void cobra4_audio_cfg_log2_cic_decimation(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_cic;
		temp &= ~(0xf);
		temp |= (value & 0xf);
	
		cobra4_audio->audio_cfg_cic = temp;
	
}

/////////////////////////////

/*
"1: high speed fir filter is  bypassed
 0: high speed fir filter is not bypassed"
*/
void cobra4_audio_cfg_high_speed_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 9);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 9);
	 }
	 
}	

/*
"1: to delay the pdm data is  bypassed
 0: to delay the pdm data is not bypass"

*/
void cobra4_audio_cfg_pdm_data_delay_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 8);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 8);
	 }
	 
}

/*
"1: volume slide is bypassed
 0: volume slide is not bypassed"
*/
void cobra4_audio_cfg_v_s_tof_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 7);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 7);
	 }
	 
}

/*
"1: volume hand control is bypassed
 0: volume hand control is not bypassed"

*/
void cobra4_audio_cfg_v_h_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 6);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 6);
	 }
	 
}

/*
"1: iir-group is bypassed
0: iir-groupl is not bypassed"

*/
void cobra4_audio_cfg_iir_group_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 5);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 5);
	 }
	 
}

/*
"1: high pass irr(del the DC) is bypassed
 0: high pass irr(del the DC) is not bypassed"

*/
void cobra4_audio_cfg_hp_iir_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 4);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 4);
	 }
	 
}

/*
"1: cic is bypassed
 0: cic is not bypassed"
*/
void cobra4_audio_cfg_cic_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 3);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 3);
	 }
	 
}

/*
"1: hbf2 is bypassed
 0: hbf2 is not bypassed"

*/
void cobra4_audio_cfg_hbf2_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 2);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 2);
	 }
	 
}

/*
"1: hbf1 is bypassed
 0: hbf1 is not bypassed"
*/
void cobra4_audio_cfg_hbf1_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 1);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 1);
	 }
	 
}

/*
"1: hbf0 is bypassed
 0: hbf0 is not bypassed"
*/
void cobra4_audio_cfg_hbf0_bypass_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_bypass |= (1 << 0);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_bypass &= ~(1 << 0);
	 }
	 
}

////////////////////////////////////////////////////////////////////////////
/*
"0: 32 bits (cobra3 not support)
1: 24bits (cobra3 not support)
2: 20bits   (cobra3 not support)
3: 16bits
4: 12bits
5: 8bits
default: WD bits"

*/
void cobra4_audio_cfg_i2s_out_bitwidth(COBRA4_I2S_OUT_BITS bits)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_out_bitwidth;
	
		temp &= ~(0x7 << 3);
	
		if(bits == I2S_OUT_32BITS)
		{
				 temp |= (0x00 << 3);
		}
		else if(bits == I2S_OUT_24BITS)
		{
				 temp |= (0x01 << 3);
		}
		else if(bits == I2S_OUT_20BITS)
		{
				 temp |= (0x02 << 3);
		}
		else if(bits == I2S_OUT_16BITS)
		{
				 temp |= (0x03 << 3);
		}
		else if(bits == I2S_OUT_12BITS)
		{
				 temp |= (0x04 << 3);
		}
		else if(bits == I2S_OUT_8BITS)
		{
				 temp |= (0x05 << 3);
		}
		else if(bits == I2S_OUT_WDBITS)
		{
				 temp |= (0x06 << 3);
		}
		
	
		cobra4_audio->audio_cfg_out_bitwidth = temp;
	
}

/*
"0: 32 bits (cobra3 not support)
1: 24bits (cobra3 not support)
2: 20bits   (cobra3 not support)
3: 16bits
4: 12bits
5: 8bits
default: WD bits"

*/
void cobra4_audio_cfg_dma_out_bitwidth(COBRA4_DMA_OUT_BITS bits)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_out_bitwidth;
	
		temp &= ~(0x7);
	
		if(bits == DMA_OUT_32BITS)
		{
				 temp |= (0x00);
		}
		else if(bits == DMA_OUT_24BITS)
		{
				 temp |= (0x01);
		}
		else if(bits == DMA_OUT_20BITS)
		{
				 temp |= (0x02);
		}
		else if(bits == DMA_OUT_16BITS)
		{
				 temp |= (0x03);
		}
		else if(bits == DMA_OUT_12BITS)
		{
				 temp |= (0x04);
		}
		else if(bits == DMA_OUT_8BITS)
		{
				 temp |= (0x05);
		}
		else if(bits == DMA_OUT_WDBITS)
		{
				 temp |= (0x06);
		}
		cobra4_audio->audio_cfg_out_bitwidth = temp;
}

/////////////////////////////////////////////////////////
/*
 the dcoffset of right channel adc
*/
void cobra4_audio_cfg_right_dc_offset(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_dc_offset;
		temp &= ~(0xfff << 12);
		temp |= ((value & 0xfff) << 12);
	
		cobra4_audio->audio_cfg_dc_offset = temp;
}

/*
the dcoffset of left channel adc

*/
void cobra4_audio_cfg_left_dc_offset(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_dc_offset;
		temp &= ~(0xfff);
		temp |= ((value & 0xfff));
	
		cobra4_audio->audio_cfg_dc_offset = temp;
}

//////////////////////////////////////////////////////////////////////////
/*
"
0: src_out
1: adc_intf_in
2: adc_intf_in
3: signed_adc_in
4: adc_intf_out
5: src_in
6: cic_in
7: cic_out
8: hbf0_in
9: hbf0_out
10: hbf1_in
11: hbf1_out
12: hbf2_in
13: hbf2_out
14: hp_iir_in
15: hp_iir_out
16: iir_group_in
17: iir_group_out
18: high_speed_fir_in
19: high_speed_fir_out
20: volume_hand_in
21: volume_hand_out
22: volume_slide_in
23: volume_slide_out
24: agc_in
25: envelope_out
26: detector_out
27: noise
28: agc_out
29: online_tool_out
default:0"
*/
void cobra4_audio_cfg_who_to_sft_mux(COBRA4_WHO_TO_SFT_DMA who)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_who_to_out;
		temp &= ~(0x1f << 10);
		if(who == SRC_OUT)
		{
				temp |= (0x0 << 10);
		}
		else if(who == ADC_INTF_IN)
		{
				temp |= (0x1 << 10);
		}
		else if(who == SIGNED_ADC_IN)
		{
				temp |= (0x3 << 10);
		}
		else if(who == ADC_INTF_OUT)
		{
				temp |= (0x4 << 10);
		}
		else if(who == SRC_IN)
		{
				temp |= (0x5 << 10);
		}
		else if(who == CIC_IN)
		{
				temp |= (6 << 10);
		}
		else if(who == CIC_OUT)
		{
				temp |= (7 << 10);
		}
		else if(who == HBF0_IN)
		{
				temp |= (8 << 10);
		}
		else if(who == HBF0_OUT)
		{
				temp |= (9 << 10);
		}
		else if(who == HBF1_IN)
		{
				temp |= (10 << 10);
		}
		else if(who == HBF1_OUT)
		{
				temp |= (11 << 10);
		}
		else if(who == HBF2_IN)
		{
				temp |= (12 << 10);
		}
		else if(who == HBF2_OUT)
		{
				temp |= (13 << 10);
		}
		
		else if(who == HP_IIR_IN)
		{
				temp |= (14 << 10);
		}
		else if(who == HP_IIR_OUT)
		{
				temp |= (15 << 10);
		}
		else if(who == IIR_GROUP_IN)
		{
				temp |= (16 << 10);
		}
		else if(who == IIR_GROUP_OUT)
		{
				temp |= (17 << 10);
		}
		else if(who == HIGH_SPEED_FIR_IN)
		{
				temp |= (18 << 10);
		}
		else if(who == HIGH_SPEED_FIR_OUT)
		{
				temp |= (19 << 10);
		}
		
		else if(who == VOLUME_HAND_IN)
		{
				temp |= (20 << 10);
		}
		else if(who == VOLUME_HAND_OUT)
		{
				temp |= (21 << 10);
		}
		
		else if(who == VOLUME_SLIDE_IN)
		{
				temp |= (22 << 10);
		}
		else if(who == VOLUME_SLIDE_OUT)
		{
				temp |= (23 << 10);
		}
		else if(who == AGC_IN)
		{
				temp |= (24 << 10);
		}
		else if(who == ENVELOPE_OUT)
		{
				temp |= (25 << 10);
		}
		else if(who == DETECTOR_OUT)
		{
				temp |= (26 << 10);
		}
		else if(who == NOISE)
		{
				temp |= (27 << 10);
		}
		else if(who == AGC_OUT)
		{
				temp |= (28 << 10);
		}
		else 
		{
				temp |= (0 << 10);
		}	
	  cobra4_audio->audio_cfg_who_to_out = temp;
}


/*
"0: src_out
1: hbf1_out
2: hbf2_out
3: hp_iir_out
4: hp_iir_in
4: agc_out 
default: 0"
*/
void cobra4_audio_cfg_who_to_i2s_mux(COBRA4_WHO_TO_I2S who)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_who_to_out;
		temp &= ~(0x1f << 5);
	
		if(who == I2S_SRC_OUT)
		{
			  temp |= (0 << 5);
		}
		else if(who == I2S_HBF1_OUT)
		{
			  temp |= (1 << 5);
		}
		else if(who == I2S_HBF2_OUT)
		{
			  temp |= (2 << 5);
		}
		else if(who == I2S_HP_IIR_OUT)
		{
			  temp |= (3 << 5);
		}
		else if(who == I2S_HP_IIR_IN)
		{
			  temp |= (4 << 5);
		}
		else if(who == I2S_AGC_OUT)
		{
			  temp |= (5 << 5);
		}
		
		cobra4_audio->audio_cfg_who_to_out = temp;
}

void cobra4_audio_cfg_who_to_dma_mux (COBRA4_WHO_TO_SFT_DMA who)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_who_to_out;
		temp &= ~(0x1f);
		if(who == SRC_OUT)
		{
				temp |= (0x0);
		}
		else if(who == ADC_INTF_IN)
		{
				temp |= (0x1);
		}
		else if(who == SIGNED_ADC_IN)
		{
				temp |= (0x3);
		}
		else if(who == ADC_INTF_OUT)
		{
				temp |= (0x4);
		}
		else if(who == SRC_IN)
		{
				temp |= (0x5);
		}
		else if(who == CIC_IN)
		{
				temp |= (6);
		}
		else if(who == CIC_OUT)
		{
				temp |= (7);
		}
		else if(who == HBF0_IN)
		{
				temp |= (8);
		}
		else if(who == HBF0_OUT)
		{
				temp |= (9);
		}
		else if(who == HBF1_IN)
		{
				temp |= (10);
		}
		else if(who == HBF1_OUT)
		{
				temp |= (11);
		}
		else if(who == HBF2_IN)
		{
				temp |= (12);
		}
		else if(who == HBF2_OUT)
		{
				temp |= (13);
		}
		
		else if(who == HP_IIR_IN)
		{
				temp |= (14);
		}
		else if(who == HP_IIR_OUT)
		{
				temp |= (15);
		}
		else if(who == IIR_GROUP_IN)
		{
				temp |= (16);
		}
		else if(who == IIR_GROUP_OUT)
		{
				temp |= (17);
		}
		else if(who == HIGH_SPEED_FIR_IN)
		{
				temp |= (18);
		}
		else if(who == HIGH_SPEED_FIR_OUT)
		{
				temp |= (19 );
		}
		
		else if(who == VOLUME_HAND_IN)
		{
				temp |= (20 );
		}
		else if(who == VOLUME_HAND_OUT)
		{
				temp |= (21 );
		}
		
		else if(who == VOLUME_SLIDE_IN)
		{
				temp |= (22 );
		}
		else if(who == VOLUME_SLIDE_OUT)
		{
				temp |= (23);
		}
		else if(who == AGC_IN)
		{
				temp |= (24);
		}
		else if(who == ENVELOPE_OUT)
		{
				temp |= (25);
		}
		else if(who == DETECTOR_OUT)
		{
				temp |= (26);
		}
		else if(who == NOISE)
		{
				temp |= (27);
		}
		else if(who == AGC_OUT)
		{
				temp |= (28);
		}
		else 
		{
				temp |= (0 );
		}	
	  cobra4_audio->audio_cfg_who_to_out = temp;
}

//////////////////////////
/*
"value range: 0~(2^15-1)  
the time to wait a zero-crossing point"
*/
void cobra4_audio_cfg_zero_crossing_timeout_value(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_zero_crossing_timeout_value;
	
		temp &= ~(0xffff);	
		temp |= value&0xffff;
	
		cobra4_audio->audio_cfg_zero_crossing_timeout_value = temp;

}

/////////////////////
/*
from 0 to 63, 1db per step

*/
void cobra4_audio_cfg_v_s_tof_decline_start_point(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_volume_slide_ctrl;
	
		temp &= ~(0x3f << 17);	
		temp |= ((value&0x3f) << 17);
	
		cobra4_audio->audio_cfg_volume_slide_ctrl = temp;
}

/*
from 0 to 63, -1db per step

*/
void cobra4_audio_cfg_v_s_tof_climb_start_point(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_volume_slide_ctrl;
	
		temp &= ~(0x3f << 11);	
		temp |= ((value&0x3f) << 11);
	
		cobra4_audio->audio_cfg_volume_slide_ctrl = temp;
}

/*
 the clock number per step. Clock is src_out_clk
*/
void cobra4_audio_cfg_v_s_tof_interval(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_volume_slide_ctrl;
	
		temp &= ~(0xff << 3);	
		temp |= ((value&0xff) << 3);
	
		cobra4_audio->audio_cfg_volume_slide_ctrl = temp;
}

/*
"1: when find a zero-cross point ,step number increase
0: when upedge of clock come, step number increase"

*/
void cobra4_audio_cfg_v_s_tof_zero_crossing_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_volume_slide_ctrl |= (1 << 2);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_volume_slide_ctrl &= ~(1 << 2);
	 }
	 
}

/*
"1: the volume slide of the right channel is enabled
0: the volume slide of the right channel is disabled"

*/
void cobra4_audio_cfg_v_s_tof_rch_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_volume_slide_ctrl |= (1 << 1);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_volume_slide_ctrl &= ~(1 << 1);
	 }
	 
}

/*
"1: the volume slide of the left channel is enabled
0: the volume slide of the left channel is disabled"
*/
void cobra4_audio_cfg_v_s_tof_lch_en(unsigned int en)
{
	 if(en)
	 {
				cobra4_audio->audio_cfg_volume_slide_ctrl |= (1 << 0);
	 }
	 else
	 {
				cobra4_audio->audio_cfg_volume_slide_ctrl &= ~(1 << 0);
	 }
	 
}

//////////////////////////////
/*
b1 cofficient. high pass iir is one-order iir. 

*/
void cobra4_audio_cfg_hp_iir_b1( int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_hp_iir_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_hp_iir_num = temp;
}

/*
b0 cofficient. high pass iir is one-order iir. 
*/
void cobra4_audio_cfg_hp_iir_b0(int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_hp_iir_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_hp_iir_num = temp;
}

///////////////////
/*
a1 cofficient. high pass iir is one-order iir. 
*/
void cobra4_audio_cfg_hp_iir_a1(int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_hp_iir_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_hp_iir_den = temp;
}

/*
"0: clk_1fs
1: clk_2fs
2: clk_4fs
3: clk_8fs
4: mclk
default: 0"
*/
void cobra4_audio_clk_sft_speed(COBRA4_SPEED_CLK clk)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_clk_speed;
	
		temp &= ~(0x7 << 24);
		if(clk == SPEED_CLK_1FS)
		{
			  temp |= (0 << 24);
		}
		else if(clk == SPEED_CLK_2FS)
		{
			  temp |= (1 << 24);
		}
		else if(clk == SPEED_CLK_4FS)
		{
			  temp |= (2 << 24);
		}
		else if(clk == SPEED_CLK_8FS)
		{
			  temp |= (3 << 24);
		}
		else if(clk == SPEED_CLK_MCLK)
		{
			  temp |= (4 << 24);
		}
		cobra4_audio->audio_cfg_clk_speed = temp;
}

/*
cfg_iir_group1_clk_speed 
*/
void cobra4_audio_cfg_iir_group1_clk_speed(COBRA4_SPEED_CLK clk)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_clk_speed;
	
		temp &= ~(0x7 << 21);
		if(clk == SPEED_CLK_1FS)
		{
			  temp |= (0 << 21);
		}
		else if(clk == SPEED_CLK_2FS)
		{
			  temp |= (1 << 21);
		}
		else if(clk == SPEED_CLK_4FS)
		{
			  temp |= (2 << 21);
		}
		else if(clk == SPEED_CLK_8FS)
		{
			  temp |= (3 << 21);
		}
		else if(clk == SPEED_CLK_MCLK)
		{
			  temp |= (4 << 21);
		}
		cobra4_audio->audio_cfg_clk_speed = temp;
}

/*
cfg_iir_group_clk_speed 
*/
void cobra4_audio_cfg_iir_group_clk_speed(COBRA4_SPEED_CLK clk)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_clk_speed;
	
		temp &= ~(0x7 << 18);
		if(clk == SPEED_CLK_1FS)
		{
			  temp |= (0 << 18);
		}
		else if(clk == SPEED_CLK_2FS)
		{
			  temp |= (1 << 18);
		}
		else if(clk == SPEED_CLK_4FS)
		{
			  temp |= (2 << 18);
		}
		else if(clk == SPEED_CLK_8FS)
		{
			  temp |= (3 << 18);
		}
		else if(clk == SPEED_CLK_MCLK)
		{
			  temp |= (4 << 18);
		}
		cobra4_audio->audio_cfg_clk_speed = temp;
}

/*
cfg_tx_fs_clk_speed 

*/
void cobra4_audio_cfg_tx_fs_clk_speed(COBRA4_SPEED_CLK clk)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_clk_speed;
	
		temp &= ~(0x7 << 15);
		if(clk == SPEED_CLK_1FS)
		{
			  temp |= (0 << 15);
		}
		else if(clk == SPEED_CLK_2FS)
		{
			  temp |= (1 << 15);
		}
		else if(clk == SPEED_CLK_4FS)
		{
			  temp |= (2 << 15);
		}
		else if(clk == SPEED_CLK_8FS)
		{
			  temp |= (3 << 15);
		}
		else if(clk == SPEED_CLK_MCLK)
		{
			  temp |= (4 << 15);
		}
		cobra4_audio->audio_cfg_clk_speed = temp;
}

/*
cfg_high_speed_fir_clk_speed 
*/
void cobra4_audio_cfg_high_speed_fir_clk_speed(COBRA4_SPEED_CLK clk)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_clk_speed;
	
		temp &= ~(0x7 << 12);
		if(clk == SPEED_CLK_1FS)
		{
			  temp |= (0 << 12);
		}
		else if(clk == SPEED_CLK_2FS)
		{
			  temp |= (1 << 12);
		}
		else if(clk == SPEED_CLK_4FS)
		{
			  temp |= (2 << 12);
		}
		else if(clk == SPEED_CLK_8FS)
		{
			  temp |= (3 << 12);
		}
		else if(clk == SPEED_CLK_MCLK)
		{
			  temp |= (4 << 12);
		}
		cobra4_audio->audio_cfg_clk_speed = temp;
}

/*
cfg_cic_clkout_speed 
 
*/
void cobra4_audio_cfg_cic_clkout_speed(COBRA4_SPEED_CLK clk)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_clk_speed;
	
		temp &= ~(0x7 << 9);
		if(clk == SPEED_CLK_1FS)
		{
			  temp |= (0 << 9);
		}
		else if(clk == SPEED_CLK_2FS)
		{
			  temp |= (1 << 9);
		}
		else if(clk == SPEED_CLK_4FS)
		{
			  temp |= (2 << 9);
		}
		else if(clk == SPEED_CLK_8FS)
		{
			  temp |= (3 << 9);
		}
		else if(clk == SPEED_CLK_MCLK)
		{
			  temp |= (4 << 9);
		}
		cobra4_audio->audio_cfg_clk_speed = temp;
}

/*
clk_hbf0_speed 
*/
void cobra4_audio_clk_hbf0_speed(COBRA4_SPEED_CLK clk)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_clk_speed;
	
		temp &= ~(0x7 << 6);
		if(clk == SPEED_CLK_1FS)
		{
			  temp |= (0 << 6);
		}
		else if(clk == SPEED_CLK_2FS)
		{
			  temp |= (1 << 6);
		}
		else if(clk == SPEED_CLK_4FS)
		{
			  temp |= (2 << 6);
		}
		else if(clk == SPEED_CLK_8FS)
		{
			  temp |= (3 << 6);
		}
		else if(clk == SPEED_CLK_MCLK)
		{
			  temp |= (4 << 6);
		}
		cobra4_audio->audio_cfg_clk_speed = temp;
}

/*
clk_hbf1_speed 

*/
void cobra4_audio_clk_hbf1_speed(COBRA4_SPEED_CLK clk)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_clk_speed;
	
		temp &= ~(0x7 << 3);
		if(clk == SPEED_CLK_1FS)
		{
			  temp |= (0 << 3);
		}
		else if(clk == SPEED_CLK_2FS)
		{
			  temp |= (1 << 3);
		}
		else if(clk == SPEED_CLK_4FS)
		{
			  temp |= (2 << 3);
		}
		else if(clk == SPEED_CLK_8FS)
		{
			  temp |= (3 << 3);
		}
		else if(clk == SPEED_CLK_MCLK)
		{
			  temp |= (4 << 3);
		}
		cobra4_audio->audio_cfg_clk_speed = temp;
}

/*
clk_hbf2_speed 
*/
void cobra4_audio_clk_hbf2_speed(COBRA4_SPEED_CLK clk)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_clk_speed;
	
		temp &= ~(0x7 << 0);
		if(clk == SPEED_CLK_1FS)
		{
			  temp |= (0 << 0);
		}
		else if(clk == SPEED_CLK_2FS)
		{
			  temp |= (1 << 0);
		}
		else if(clk == SPEED_CLK_4FS)
		{
			  temp |= (2 << 0);
		}
		else if(clk == SPEED_CLK_8FS)
		{
			  temp |= (3 << 0);
		}
		else if(clk == SPEED_CLK_MCLK)
		{
			  temp |= (4 << 0);
		}
		cobra4_audio->audio_cfg_clk_speed = temp;
}

/*
fir order range: from 1 to 32
*/
void cobra4_audio_cfg_high_speed_fir_order(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_high_speed_fir;
	
		temp &= ~(0x3f << 16);	
		temp |= ((value&0x3f) << 16);
	
		cobra4_audio->audio_cfg_high_speed_fir = temp;
}

/*
"cfg_fast_clk_ratio  = xtal_pll_clk/fast_clk
The fast_clk is used to complete the fir computation of high-speed-fir in one src_out clock' period."

*/
void cobra4_audio_cfg_fast_clk_ratio(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_high_speed_fir;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_high_speed_fir = temp;
}
////////////////////////////////////////////////////////////
//iir0 b2 coffecient in iir-group

void cobra4_audio_cfg_iir0_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir0_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir0_num = temp;
}

//iir0 b1 coffecient in iir-group

void cobra4_audio_cfg_iir0_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir0_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir0_num = temp;
}

//iir0 a2 coffecient in iir-group
void cobra4_audio_cfg_iir0_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir0_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir0_den = temp;
}

//iir0 a1 coffecient in iir-group
void cobra4_audio_cfg_iir0_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir0_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir0_den = temp;
}

//////////////////////////////////////////////////////////
//iir1 b2 coffecient in iir-group
void cobra4_audio_cfg_iir1_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir1_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir1_num = temp;
}

//iir1 b1 coffecient in iir-group

void cobra4_audio_cfg_iir1_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir1_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir1_num = temp;
}

//iir1 a2 coffecient in iir-group
void cobra4_audio_cfg_iir1_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir1_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir1_den = temp;
}

//iir1 a1 coffecient in iir-group
void cobra4_audio_cfg_iir1_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir1_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir1_den = temp;
}
///////////////////////////////////////////////////////////

//iir2 b2 coffecient in iir-group
void cobra4_audio_cfg_iir2_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir2_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir2_num = temp;
}

//iir2 b1 coffecient in iir-group

void cobra4_audio_cfg_iir2_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir2_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir2_num = temp;
}

//iir2 a2 coffecient in iir-group
void cobra4_audio_cfg_iir2_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir2_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir2_den = temp;
}

//iir2 a1 coffecient in iir-group
void cobra4_audio_cfg_iir2_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir2_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir2_den = temp;
}

///////////////////////////////////////////////////////
//iir3 b2 coffecient in iir-group
void cobra4_audio_cfg_iir3_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir3_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir3_num = temp;
}

//iir3 b1 coffecient in iir-group

void cobra4_audio_cfg_iir3_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir3_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir3_num = temp;
}

//iir3 a2 coffecient in iir-group
void cobra4_audio_cfg_iir3_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir3_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir3_den = temp;
}

//iir3 a1 coffecient in iir-group
void cobra4_audio_cfg_iir3_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir3_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir3_den = temp;
}

///////////////////////////////////////////////////////////////////////////////
//iir4 b2 coffecient in iir-group
void cobra4_audio_cfg_iir4_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir4_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir4_num = temp;
}

//iir4 b1 coffecient in iir-group

void cobra4_audio_cfg_iir4_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir4_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir4_num = temp;
}

//iir4 a2 coffecient in iir-group
void cobra4_audio_cfg_iir4_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir4_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir4_den = temp;
}

//iir4 a1 coffecient in iir-group
void cobra4_audio_cfg_iir4_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir4_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir4_den = temp;
}
/////////////////////////////////////////////////////////////////
//iir5 b2 coffecient in iir-group
void cobra4_audio_cfg_iir5_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir5_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir5_num = temp;
}

//iir5 b1 coffecient in iir-group

void cobra4_audio_cfg_iir5_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir5_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir5_num = temp;
}

//iir5 a2 coffecient in iir-group
void cobra4_audio_cfg_iir5_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir5_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir5_den = temp;
}

//iir5 a1 coffecient in iir-group
void cobra4_audio_cfg_iir5_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir5_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir5_den = temp;
}
//////////////////////////////////////////////////////
//iir6 b2 coffecient in iir-group
void cobra4_audio_cfg_iir6_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir6_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir6_num = temp;
}

//iir6 b1 coffecient in iir-group

void cobra4_audio_cfg_iir6_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir6_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir6_num = temp;
}

//iir6 a2 coffecient in iir-group
void cobra4_audio_cfg_iir6_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir6_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir6_den = temp;
}

//iir6 a1 coffecient in iir-group
void cobra4_audio_cfg_iir6_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir6_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir6_den = temp;
}

////////////////////////////////////////////////////////////////////
//iir7 b2 coffecient in iir-group
void cobra4_audio_cfg_iir7_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir7_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir7_num = temp;
}

//iir7 b1 coffecient in iir-group

void cobra4_audio_cfg_iir7_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir7_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir7_num = temp;
}

//iir7 a2 coffecient in iir-group
void cobra4_audio_cfg_iir7_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir7_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir7_den = temp;
}

//iir7 a1 coffecient in iir-group
void cobra4_audio_cfg_iir7_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir7_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir7_den = temp;
}
////////////////////////////////////////////////////////////
//iir8 b2 coffecient in iir-group
void cobra4_audio_cfg_iir8_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir8_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir8_num = temp;
}

//iir8 b1 coffecient in iir-group
void cobra4_audio_cfg_iir8_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir8_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir8_num = temp;
}

//iir8 a2 coffecient in iir-group
void cobra4_audio_cfg_iir8_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir8_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir8_den = temp;
}

//iir8 a1 coffecient in iir-group
void cobra4_audio_cfg_iir8_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir8_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir8_den = temp;
}
////////////////////////////////////////////////////////////
//iir9 b2 coffecient in iir-group
void cobra4_audio_cfg_iir9_b2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir9_num;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir9_num = temp;
}

//iir9 b1 coffecient in iir-group
void cobra4_audio_cfg_iir9_b1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir9_num;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir9_num = temp;
}

//iir9 a2 coffecient in iir-group
void cobra4_audio_cfg_iir9_a2(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir9_den;
	
		temp &= ~(0xffff << 16);	
		temp |= ((value&0xffff) << 16);
	
		cobra4_audio->audio_cfg_iir9_den = temp;
}

//iir9 a1 coffecient in iir-group
void cobra4_audio_cfg_iir9_a1(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir9_den;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir9_den = temp;
}
////////////////////////////////////////////////////////////

//iir0 g0 coffecient in iir-group
void cobra4_audio_cfg_iir0_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir0_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir0_g0 = temp;
}

//iir1 g0 coffecient in iir-group
void cobra4_audio_cfg_iir1_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir1_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir1_g0 = temp;
}

//iir2 g0 coffecient in iir-group
void cobra4_audio_cfg_iir2_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir2_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir2_g0 = temp;
}

//iir3 g0 coffecient in iir-group
void cobra4_audio_cfg_iir3_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir3_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir3_g0 = temp;
}

//iir4 g0 coffecient in iir-group
void cobra4_audio_cfg_iir4_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir4_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir4_g0 = temp;
}

//iir5 g0 coffecient in iir-group
void cobra4_audio_cfg_iir5_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir5_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir5_g0 = temp;
}

//iir6 g0 coffecient in iir-group
void cobra4_audio_cfg_iir6_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir6_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir6_g0 = temp;
}

//iir7 g0 coffecient in iir-group
void cobra4_audio_cfg_iir7_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir7_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir7_g0 = temp;
}

//iir8 g0 coffecient in iir-group
void cobra4_audio_cfg_iir8_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir8_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir8_g0 = temp;
}

//iir9 g0 coffecient in iir-group
void cobra4_audio_cfg_iir9_g0(unsigned int value)
{
	  volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_iir9_g0;
	
		temp &= ~(0xffff);	
		temp |= (value&0xffff);
	
		cobra4_audio->audio_cfg_iir9_g0 = temp;
}
////////////////////////////////////////////////////////
/*
"for the right channel
[6]        0: amplify the data , 1: shrink the data
[5:1]      6db per step
[0]        1: additional 3.5db   0:0db"
*/
void cobra4_audio_cfg_manual_gain_coarse_shift_r_amplify_or_shrink_data(AMPLIFY_OR_SHRINK type)
{
		if(type == SHRINK_DATA)
		{
				cobra4_audio->audio_cfg_manual_gain_coarse |= (1<<13);
		}
		else if(type == AMPLIFY_DATA)
		{
				cobra4_audio->audio_cfg_manual_gain_coarse &= ~(1<<13);
		}
}

void cobra4_audio_cfg_manual_gain_coarse_shift_r_additional_3_5_db(unsigned int en) 
{
		if(en)
		{
				cobra4_audio->audio_cfg_manual_gain_coarse |= (1<<7);
		}
		else 
		{
				cobra4_audio->audio_cfg_manual_gain_coarse &= ~(1<<7);
		}
}

void cobra4_audio_cfg_manual_gain_coarse_shift_r_amplify_shrink_value(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_manual_gain_coarse;
	
		temp &= ~(0x1f << 8);	
		temp |= ((value&0x1f) << 8);
	
		cobra4_audio->audio_cfg_manual_gain_coarse = temp;
	
}
/*
"for the left channel
[6]        0: amplify the data , 1: shrink the data
[5:1]      6db per step
[0]        1: additional 3.5db   0:0db"
*/
void cobra4_audio_cfg_manual_gain_coarse_shift_l_amplify_or_shrink_data(AMPLIFY_OR_SHRINK type)
{
		if(type == SHRINK_DATA)
		{
				cobra4_audio->audio_cfg_manual_gain_coarse |= (1<<6);
		}
		else if(type == AMPLIFY_DATA)
		{
				cobra4_audio->audio_cfg_manual_gain_coarse &= ~(1<<6);
		}
}

void cobra4_audio_cfg_manual_gain_coarse_shift_l_additional_3_5_db(unsigned int en) 
{
		if(en)
		{
				cobra4_audio->audio_cfg_manual_gain_coarse |= (1<<0);
		}
		else 
		{
				cobra4_audio->audio_cfg_manual_gain_coarse &= ~(1<<0);
		}
}

void cobra4_audio_cfg_manual_gain_coarse_shift_l_amplify_shrink_value(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_manual_gain_coarse;
	
		temp &= ~(0x1f << 1);	
		temp |= ((value&0x1f) << 1);
	
		cobra4_audio->audio_cfg_manual_gain_coarse = temp;
	
}

///////////////////////////////////////////////////////////////////////////////////////
/*
"for the right channel
[11] signed flag
[10] int bit
[9:0] fractional bits
every step: 0.1db
range: -5.9db ~5.9db"
*/
void cobra4_audio_cfg_manual_gain_fine_mul_r_signed_flag(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_manual_gain_fine |= (1 << 23);
		}
		else
		{
				cobra4_audio->audio_cfg_manual_gain_fine &= ~(1 << 23);
		}
}

void cobra4_audio_cfg_manual_gain_fine_mul_r_int_bits(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_manual_gain_fine |= (1 << 22);
		}
		else
		{
				cobra4_audio->audio_cfg_manual_gain_fine &= ~(1 << 22);
		}
}

void cobra4_audio_cfg_manual_gain_fine_mul_r_fractional_bits(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_manual_gain_fine;
	
		temp &= ~(0x3ff << 12);	
		temp |= ((value&0x3ff) << 12);
	
		cobra4_audio->audio_cfg_manual_gain_fine = temp;	
}

/*
"for the left channel
[11] signed flag
[10] int bit
[9:0] fractional bits
every step: 0.1db
range: -5.9db ~5.9db"
*/
void cobra4_audio_cfg_manual_gain_fine_mul_l_signed_flag(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_manual_gain_fine |= (1 << 11);
		}
		else
		{
				cobra4_audio->audio_cfg_manual_gain_fine &= ~(1 << 11);
		}
}

void cobra4_audio_cfg_manual_gain_fine_mul_l_int_bits(unsigned int en)
{
		if(en)
		{
				cobra4_audio->audio_cfg_manual_gain_fine |= (1 << 10);
		}
		else
		{
				cobra4_audio->audio_cfg_manual_gain_fine &= ~(1 << 10);
		}
}

void cobra4_audio_cfg_manual_gain_fine_mul_l_fractional_bits(unsigned int value)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_manual_gain_fine;
	
		temp &= ~(0x3ff);	
		temp |= (value&0x3ff);
	
		cobra4_audio->audio_cfg_manual_gain_fine = temp;	
}

/*
"0: gain is 0db
 1: only fine gain 
 2: only coarse gain
 3: coarse gain plus fine gain
 Because 3.5db is not very accurate, so mode 3 is not very accurate. "
*/
void cobra4_audio_cfg_manual_gain_mode(MANUAL_GAIN_MODE mode)
{
		volatile unsigned int temp;
		
		temp = cobra4_audio->audio_cfg_manual_gain_mode;
		
		temp &= ~0x03;
	
		if(mode == ZERO_DB_MODE)
		{
				temp |= 0x00;
		}
		else if(mode == FINE_GAIN_MODE)
		{
				temp |= 0x01;
		}
		else if(mode == COARSE_GAIN_MODE)
		{
				temp |= 0x02;
		}
		else if(mode == COARSE_PLUS_FINE_MODE)
		{
				temp |= 0x03;
		}
		cobra4_audio->audio_cfg_manual_gain_mode = temp;
}

///////////////////////////////////////////////
/*
when read the address, if the upfifo is not emapty, upfifo_rdat can be renewed
upfifo_rdat0

*/
unsigned int cobra4_audio_upfifo_rdat0(void)
{
		return cobra4_audio->audio_upfifo_rdat0;
}
/*
when read the address, if the upfifo is not emapty, upfifo_rdat can be renewed
upfifo_rdat1

*/
unsigned int cobra4_audio_upfifo_rdat1(void)
{
		return cobra4_audio->audio_upfifo_rdat1;
}
/*
when read the address, if the upfifo is not emapty, upfifo_rdat can be renewed
upfifo_rdat2

*/
unsigned int cobra4_audio_upfifo_rdat2(void)
{
		return cobra4_audio->audio_upfifo_rdat2;
}
/*
when read the address, if the upfifo is not emapty, upfifo_rdat can be renewed
upfifo_rdat3

*/
unsigned int cobra4_audio_upfifo_rdat3(void)
{
		return cobra4_audio->audio_upfifo_rdat3;
}
/*
when read the address, if the upfifo is not emapty, upfifo_rdat can be renewed
upfifo_rdat4

*/
unsigned int cobra4_audio_upfifo_rdat4(void)
{
		return cobra4_audio->audio_upfifo_rdat4;
}
/*
when read the address, if the upfifo is not emapty, upfifo_rdat can be renewed
upfifo_rdat5

*/
unsigned int cobra4_audio_upfifo_rdat5(void)
{
		return cobra4_audio->audio_upfifo_rdat5;
}
/*
when read the address, if the upfifo is not emapty, upfifo_rdat can be renewed
upfifo_rdat6

*/
unsigned int cobra4_audio_upfifo_rdat6(void)
{
		return cobra4_audio->audio_upfifo_rdat6;
}
/*
when read the address, if the upfifo is not emapty, upfifo_rdat can be renewed
upfifo_rdat7

*/
unsigned int cobra4_audio_upfifo_rdat7(void)
{
		return cobra4_audio->audio_upfifo_rdat7;
}

/*
"the interrupt of outport
when cfg_stciky_int_en=1,interrupt is sticky_int
when cfg_stciky_int_en=0,interrupt is -sticky_int"
the sticky int
upfifo_almostfull_sticky_int
upfifo_almostempty_sticky_int
upfifo_empty_sticky_int
upfifo_overflow_sticky_int
upfifo_underflow_sticky_int
upfifo_full_sticky_int

#define  AUDIO_INTERRUPT												0x0080
#define  AUDIO_STICKY_INT												0x0040
#define  AUDIO_UPFIFO_ALMOSTFULL_STICKY_INT			0x0020
#define  AUDIO_UPFIFO_ALMOSTEMPTY_STICKY_INT		0x0010
#define  AUDIO_UPFIFO_EMPTY_STICKY_INT					0x0008
#define  AUDIO_UPFIFO_OVERFLOW_STICKY_INT				0x0004
#define  AUDIO_UPFIFO_UNDERFLOW_STICKY_INT			0x0002
#define  AUDIO_UPFIFO_FULL_STICKY_INT						0x0001

*/
unsigned int cobra4_audio_int_status(void)
{
		return cobra4_audio->audio_int_status;
}

/*
"1: for the right channel,the decline of volume slide  is done
0: normal"
"1:  for the right channel,the clime of volume slide  is done
0: normal"
"1: for the left channel,the decline of volume slide  is done
0: normal"
"1:  for the left channel,the clime of volume slide  is done
0: normal"

#define AUDIO_V_S_TF_DECLINE_DONE_R					0x08
#define AUDIO_V_S_TF_CLIME_DONE_R						0x04
#define AUDIO_V_S_TF_DECLINE_DONE_L					0x02
#define AUDIO_V_S_TF_CLIME_DONE_L						0x01
*/
unsigned int cobra4_audio_volume_slide_status(void)
{
		return cobra4_audio->audio_volume_slide_status;
}


/*
"[7:4] upfifo rfilled number , range: 0~8
[3:0] upfifo wfilled number , range: 0~8"

*/
unsigned int cobra4_audio_upfifo_filled_num_r(void)
{
		return (((cobra4_audio->audio_upfifo_status)&(0x0f << 12)) >> 12);
}

unsigned int cobra4_audio_upfifo_filled_num_l(void)
{
		return (((cobra4_audio->audio_upfifo_status)&(0x0f << 8)) >> 8);
}

/*
"1: upfifo is almostempty
 0: upfifo is not almostempty"

"1: upfifo is empty
 0: upfifo is not empty"

"1: upfifo is almostfull
 0: upfifo is not almostfull"

"1: upfifo is full
 0: upfifo is not full"

"1: upfifo is underflow
 0: upfifo is not underflow"

"1: upfifo is overflow
 0: upfifo is not overflow"

#define AUDIO_UPFIFO_ALMOSTEMPTY					0x20
#define AUDIO_UPFIFO_EMPTY								0x10
#define AUDIO_UPFIFO_ALMOSTFULL						0x08
#define AUDIO_UPFIFO_FULL 								0x04
#define AUDIO_UPFIFO_UNDERFLOW						0x02
#define AUDIO_UPFIFO_OVERFLOW							0x01
*/
unsigned int cobra4_audio_audio_upfifo_status(void)
{
		return (cobra4_audio->audio_upfifo_status&0x3f);
}

/*
audioadc_fifo_rfilled number of left channel, range 0~8
audioadc_fifo_wfilled number of left channel, range 0~8

"1: left channel audioadc  is almost empty
 0: left channel audioadc   is not almost empty"

"1: left channel audioadc  is  empty
 0: left channel audioadc   is not  empty"

"1: left channel audioadc  is almost full
 0: left channel audioadc   is not almost full"

"1: left channel audioadc  is  full
 0: left channel audioadc   is not  full"

"1: left channel audioadc  is  underflow
 0: left channel audioadc   is not  underflow"

"1: left channel audioadc  is  overflow
 0: left channel audioadc   is not  overflow"

#define AUDIO_UPFIFO_ALMOSTEMPTY					0x20
#define AUDIO_UPFIFO_EMPTY								0x10
#define AUDIO_UPFIFO_ALMOSTFULL						0x08
#define AUDIO_UPFIFO_FULL 								0x04
#define AUDIO_UPFIFO_UNDERFLOW						0x02
#define AUDIO_UPFIFO_OVERFLOW							0x01

*/
//audioadc_fifo_rfilled number of left channel, range 0~8
unsigned int cobra4_audio_audioadc_fifo_rfilled_num_l(void)
{
		return (((cobra4_audio->audio_audioadc_left_channel_fifo_status)&(0x0f << 12)) >> 12);
}

unsigned int cobra4_audio_audioadc_fifo_wfilled_num_l(void)
{
		return (((cobra4_audio->audio_audioadc_left_channel_fifo_status)&(0x0f << 8)) >> 8);
}

/*
#define AUDIO_UPFIFO_ALMOSTEMPTY					0x20
#define AUDIO_UPFIFO_EMPTY								0x10
#define AUDIO_UPFIFO_ALMOSTFULL						0x08
#define AUDIO_UPFIFO_FULL 								0x04
#define AUDIO_UPFIFO_UNDERFLOW						0x02
#define AUDIO_UPFIFO_OVERFLOW							0x01
*/
unsigned int cobra4_audio_audio_audioadc_left_channel_fifo_status(void)
{
		return ((cobra4_audio->audio_audioadc_left_channel_fifo_status)&0x3f);
}

/*
audioadc_fifo_rfilled number of right channel, range 0~8
audioadc_fifo_wfilled number of right channel, range 0~8

*/

unsigned int cobra4_audio_audioadc_fifo_rfilled_num_r(void)
{
		return (((cobra4_audio->audio_audioadc_right_channel_fifo_status)&(0x0f << 12)) >> 12);
}

unsigned int cobra4_audio_audioadc_fifo_wfilled_num_r(void)
{
		return (((cobra4_audio->audio_audioadc_right_channel_fifo_status)&(0x0f << 8)) >> 8);
}

unsigned int cobra4_audio_audio_audioadc_right_channel_fifo_status(void)
{
		return (cobra4_audio->audio_audioadc_right_channel_fifo_status & 0x3f);
}

unsigned int * cobra4_audio_read_upfifo_address(void)
{
	return (unsigned int *)(&cobra4_audio->audio_upfifo_rdat0);
}

//////////////////////////APP////////////////////////////////////

void app_audio_debug_information(void)
{
	printf("cobra4_audio->audio_cfg_ctrl = 0x%x \n", cobra4_audio->audio_cfg_ctrl);
	printf("cobra4_audio->audio_cfg_mclk = 0x%x \n", cobra4_audio->audio_cfg_mclk);
	printf("cobra4_audio->audio_cfg_pdm = 0x%x \n", cobra4_audio->audio_cfg_pdm);
	printf("cobra4_audio->audio_cfg_interrupt_sticky = 0x%x \n", cobra4_audio->audio_cfg_interrupt_sticky);
	printf("cobra4_audio->audio_cfg_iir_group = 0x%x \n", cobra4_audio->audio_cfg_iir_group);
	printf("cobra4_audio->audio_cfg_upfifo_almost_threshold = 0x%x \n", cobra4_audio->audio_cfg_upfifo_almost_threshold);
	printf("cobra4_audio->audio_cfg_cic = 0x%x \n", cobra4_audio->audio_cfg_cic);
	printf("cobra4_audio->audio_cfg_bypass = 0x%x \n", cobra4_audio->audio_cfg_bypass);
	printf("cobra4_audio->audio_cfg_out_bitwidth = 0x%x \n", cobra4_audio->audio_cfg_out_bitwidth);
	printf("cobra4_audio->audio_cfg_who_to_out = 0x%x \n", cobra4_audio->audio_cfg_who_to_out);
	printf("cobra4_audio->audio_cfg_zero_crossing_timeout_value = 0x%x \n", cobra4_audio->audio_cfg_zero_crossing_timeout_value);
	printf("cobra4_audio->audio_cfg_volume_slide_ctrl = 0x%x \n", cobra4_audio->audio_cfg_volume_slide_ctrl);
	printf("cobra4_audio->audio_cfg_hp_iir_num = 0x%x \n", cobra4_audio->audio_cfg_hp_iir_num);
	printf("cobra4_audio->audio_cfg_hp_iir_den = 0x%x \n", cobra4_audio->audio_cfg_hp_iir_den);
	printf("cobra4_audio->audio_cfg_clk_speed = 0x%x \n", cobra4_audio->audio_cfg_clk_speed);
	printf("cobra4_audio->audio_cfg_manual_gain_coarse = 0x%x \n", cobra4_audio->audio_cfg_manual_gain_coarse);
	printf("cobra4_audio->audio_cfg_manual_gain_fine = 0x%x \n", cobra4_audio->audio_cfg_manual_gain_fine);
	printf("cobra4_audio->audio_cfg_manual_gain_mode = 0x%x \n", cobra4_audio->audio_cfg_manual_gain_mode);
	
	printf("cobra4_audio->audio_cfg_dma_upfifo_ctrl = 0x%x \n", cobra4_audio->audio_cfg_dma_upfifo_ctrl);
	printf("cobra4_audio->audio_cfg_adc_data_intf = %d \n", cobra4_audio->audio_cfg_adc_data_intf);
//	printf("cobra4_audio->audio_cfg_manual_gain_fine = %d \n", cobra4_audio->audio_cfg_manual_gain_fine);
	
}
void app_audio_pdm_init(struct audio_env_tag * p_param)
{
		uint8_t cic_log2decimation;
		uint8_t	cic_stage;
		int8_t	shrink;
		uint8_t pclk_div;
	
		cic_log2decimation = 0;
		cic_stage = 0;
		shrink    = 0;
		pclk_div  = 0;
	
		cic_log2decimation = log(p_param->osr/pow(2,(3- p_param->bypass_hbnum)))/log(2);
    cic_stage = 5;
    if((cic_log2decimation*cic_stage + 1 - p_param->out_bitwidth)>0)
    {
        shrink = (cic_log2decimation*cic_stage + 1 - p_param->out_bitwidth);
    } 
		else
    {
        shrink = 0;
    }
		
		shrink -= 1;
		
		if(shrink < 0)
		{
				shrink = 0;
		}
		
//    printf("cic_log2decimation = %d\n",cic_log2decimation);
//    printf("cic_stage = %d\n",cic_stage);
//    printf("shrink = %d\n",shrink);		

		if(p_param->clock_source == MCLK_FROM_PLL)
		{
				cobra4_aon_wk_protection_en(1);
				cobra4_aon_wk_d2a_ldoanaforpll_en(1);
				cobra4_aon_wk_protection_en(0);
				cobra4_pll_set_frenquence(196.608);
		}


		cobra4_audio_cfg_agc_ram_clk_is_hclk_en(0);		
		cobra4_audio_cfg_xtal1_pll0_sel(p_param->clock_source);
		cobra4_audio_cfg_clk_en(1);
		
		cobra4_audio_cfg_i2s_enable(p_param->i2s_enable);
		
		cobra4_audio_cfg_adc0_pdm1(PDM_INTERFACE);
		
		/*CFG_MCLK*/
    pclk_div = ((int)((p_param->pclk_khz/(p_param->osr*p_param->sample_1fs_clk_kHz))+0.5));
		
		cobra4_audio_cfg_osr_by_mclk(p_param->osr);
		
	//	cobra4_audio_cfg_mclk_ratio(1);
		cobra4_audio_cfg_mclk_ratio(pclk_div);

		 /*CFG_PDM*/
    cobra4_audio_cfg_pdm_data_delay_phase(0);
    cobra4_audio_cfg_pdm_four_wire_is_2mono(1);
    cobra4_audio_cfg_pdm_left_two_wire_is_stereo(1);
    cobra4_audio_cfg_pdm_right_posedge(1);
    cobra4_audio_cfg_pdm_left_posedge(0);
		
		cobra4_audio_cfg_int_timeout2(0x3b);
		cobra4_audio_cfg_int_timeout1(0xc0);
		
		cobra4_audio_cfg_iir_group_mode(IIR_GROUP_MODE5);
		
		cobra4_audio_cfg_upfifo_almostempty_threshold(4);
		cobra4_audio_cfg_upfifo_almostfull_threshold(4);
		
		cobra4_audio_cfg_cicout_shrink(shrink);
		cobra4_audio_cfg_cic_stages(cic_stage);
		cobra4_audio_cfg_log2_cic_decimation(cic_log2decimation);
		
////////////////////////////////////////////////////////
		cobra4_audio_cfg_high_speed_bypass_en(1);
		cobra4_audio_cfg_pdm_data_delay_bypass_en(1);
		cobra4_audio_cfg_v_s_tof_bypass_en(1);
		
		if(p_param->agc_enable)
		{
				cobra4_audio_cfg_v_h_bypass_en(1);
		}
		else
		{
				cobra4_audio_cfg_v_h_bypass_en(0);
		}
		
		switch(p_param->bypass_hbnum)
		{
			case 3:
				cobra4_audio_cfg_hbf2_bypass_en(1);
				cobra4_audio_cfg_hbf1_bypass_en(1);
				cobra4_audio_cfg_hbf0_bypass_en(1);
				break;
			case 2:
				cobra4_audio_cfg_hbf1_bypass_en(1);
				cobra4_audio_cfg_hbf0_bypass_en(1);
				break;
			case 1:
				cobra4_audio_cfg_hbf0_bypass_en(1);
				break;
			default:
				break;
		}
		
		cobra4_audio_cfg_i2s_out_bitwidth(I2S_OUT_16BITS);
		cobra4_audio_cfg_dma_out_bitwidth(DMA_OUT_WDBITS);
		/*CFG_WHO_TO_OUT*/		
		if(p_param->agc_enable)
		{
				cobra4_audio_cfg_who_to_i2s_mux(I2S_AGC_OUT);
		}
		else
		{
				cobra4_audio_cfg_who_to_i2s_mux(I2S_SRC_OUT);
		}
		cobra4_audio_cfg_who_to_dma_mux(SRC_OUT);
		
		cobra4_audio_cfg_zero_crossing_timeout_value(0x1900);
	
		cobra4_audio_cfg_v_s_tof_decline_start_point(6);
		cobra4_audio_cfg_v_s_tof_climb_start_point(6);
		cobra4_audio_cfg_v_s_tof_interval(0xcf);
		cobra4_audio_cfg_v_s_tof_zero_crossing_en(1);
		cobra4_audio_cfg_v_s_tof_rch_en(1);
		cobra4_audio_cfg_v_s_tof_lch_en(1);
		

		cobra4_audio_cfg_hp_iir_b1(0x80c0);
		cobra4_audio_cfg_hp_iir_b0(0x7f40);
		cobra4_audio_cfg_hp_iir_a1(0x8180);
		
/*CFG_CLK_SPEED*/
		cobra4_audio_cfg_cic_clkout_speed(SPEED_CLK_8FS - p_param->bypass_hbnum);
		cobra4_audio_clk_hbf0_speed(SPEED_CLK_8FS);
		cobra4_audio_clk_hbf1_speed(SPEED_CLK_4FS);
		cobra4_audio_clk_hbf2_speed(SPEED_CLK_2FS);
//    set_cfg_cic_clkout_speed(0x3-bypass_hbnum,src);
//    set_clk_hbf0_speed(0x3,src);
//    set_clk_hbf1_speed(0x2,src);
//    set_clk_hbf2_speed(0x1,src);

		cobra4_audio_cfg_manual_gain_coarse_shift_r_amplify_or_shrink_data(AMPLIFY_DATA);
		cobra4_audio_cfg_manual_gain_coarse_shift_r_additional_3_5_db(0);
		cobra4_audio_cfg_manual_gain_coarse_shift_r_amplify_shrink_value(2);
		
		cobra4_audio_cfg_manual_gain_coarse_shift_l_amplify_or_shrink_data(AMPLIFY_DATA);
		cobra4_audio_cfg_manual_gain_coarse_shift_l_additional_3_5_db(0);
		cobra4_audio_cfg_manual_gain_coarse_shift_l_amplify_shrink_value(2);
		
		cobra4_audio_cfg_manual_gain_fine_mul_r_signed_flag(0);
		cobra4_audio_cfg_manual_gain_fine_mul_r_int_bits(1);
		cobra4_audio_cfg_manual_gain_fine_mul_r_fractional_bits(0);
		cobra4_audio_cfg_manual_gain_fine_mul_l_signed_flag(0);
		cobra4_audio_cfg_manual_gain_fine_mul_l_int_bits(1);
		cobra4_audio_cfg_manual_gain_fine_mul_l_fractional_bits(0);
		
		cobra4_audio_cfg_manual_gain_mode(COARSE_GAIN_MODE);
		
///*CFG_CTRL*/
//    set_cfg_fast_clk_is_hclk(src,0);
//    set_cfg_hp_iir_en(src,1);
//    set_cfg_left_right_swap(src,1);
//    set_cfg_right_en(src,1);
//    set_cfg_left_en(src,1);		

		cobra4_audio_cfg_fast_clk_is_hclk_en(0);
		cobra4_audio_cfg_hp_iir_en(1);
		cobra4_audio_cfg_left_right_swap(0);
		cobra4_audio_cfg_right_en(1);
		cobra4_audio_cfg_left_en(1);
		
		cobra4_audio_cfg_adc_data_msb_swap_r(1);
		cobra4_audio_cfg_adc_data_msb_swap_l(1);
		cobra4_audio_cfg_adc_raw_data_en(0);
		
///*CFG_DMA_UPFIFO_CTRL*/
//    set_cfg_dma_req_by_almostempty(src,1);
//    set_cfg_upfifo_dma_en(src,1);
//    set_cfg_upfifo_en(src,1);		

		cobra4_audio_cfg_dma_req_by_almostempty_en(1);
		cobra4_audio_cfg_upfifo_dma_en(1);
		cobra4_audio_cfg_upfifo_en(1);
		
		if(p_param->agc_enable)
		{
			 //agc µÄ³õÊ¼»¯
		}
		
//		app_audio_debug_information();
		
//		cobra_pcrm_i2s0_tx_work_mode(MODE_MCLK_COME_FROM_PLL);
//		cobra_pcrm_i2s0_tx_mclk_output_source(TX_MCLK_OUTPUT_APLL);
//		cobra_pcrm_i2s0_clk_en(1);
//		cobra_map_pinmodeset(PIN27_INDEX, PIN27_I2S0_TX_MCLK);
}



void app_cobra_audio_disable(void)
{
	cobra4_audio_cfg_right_en(0);
	cobra4_audio_cfg_left_en(0);
	cobra4_audio_cfg_clk_en(0);
//	cobra4_audio_cfg_fast_clk_is_hclk_en(0);

	cobra4_audio_cfg_upfifo_en(0);
	cobra4_audio_cfg_upfifo_dma_en(0);
	
//	cobra_pdm_debug();

}

void app_cobra_audio_enable(void)
{
	cobra4_audio_cfg_right_en(1);
	cobra4_audio_cfg_left_en(1);
	cobra4_audio_cfg_clk_en(1);
//	cobra4_audio_cfg_fast_clk_is_hclk_en(1);
	
	cobra4_audio_cfg_upfifo_en(1);
	cobra4_audio_cfg_upfifo_dma_en(1);
	
//	cobra_pdm_debug();
}

#endif