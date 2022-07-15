/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_pdm.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/9

	Description :   


	Changed Log	:

		Liujian  2019/4/9			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include <math.h>
#include "cobra_pdm.h"
#include "cobra_system.h"

#if COBRA_PDM_EN
#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_PDM_TypeDef * cobra_pdm		__attribute__((section("retention_area"),zero_init)); 
#else
static volatile COBRA_PDM_TypeDef * cobra_pdm;
#endif

void cobra_pdm_init(void)
{
		cobra_pdm = (COBRA_PDM_TypeDef * )COBRA_PDM_BASE;
	
}

void cobra_pdm_debug(void)
{

		printf("cfg_pdm_ctrl 0x%x \n", cobra_pdm->cfg_pdm_ctrl);
		printf("cfg_pdm_ctrl1 0x%x \n", cobra_pdm->cfg_pdm_ctrl1);
		printf("cfg_clk_gen 0x%x \n", cobra_pdm->cfg_clk_gen);
		printf("upfifo_interrupt 0x%x \n", cobra_pdm->upfifo_interrupt);
		printf("upfifo_threshold 0x%x \n", cobra_pdm->upfifo_threshold);
		printf("upfifo_ctrl 0x%x \n", cobra_pdm->upfifo_ctrl);
		printf("log2_cic_decimation 0x%x \n", cobra_pdm->log2_cic_decimation);
		printf("cfg_bypass 0x%x \n", cobra_pdm->cfg_bypass);
		printf("hp_iir_coff0 0x%x \n", cobra_pdm->hp_iir_coff0);
		printf("cfg_pdm_data_delay_phase 0x%x \n", cobra_pdm->cfg_pdm_data_delay_phase);
		printf("cfg_gain_steps 0x%x \n", cobra_pdm->cfg_gain_steps);
		printf("cfg_sampling 0x%x \n", cobra_pdm->cfg_sampling);
		printf("upfifo_status 0x%x \n", cobra_pdm->upfifo_status);	
	
//		cobra_pdm->cfg_pdm_ctrl = 0x07;
//		cobra_pdm->cfg_pdm_ctrl1 = 0x0b;
//		cobra_pdm->cfg_clk_gen = 12;
//		cobra_pdm->upfifo_interrupt = 0x1f;
//		cobra_pdm->upfifo_threshold = 4;
//	
//		cobra_pdm->upfifo_ctrl = 0x3f;
//		
//		cobra_pdm->log2_cic_decimation = 4;
//		cobra_pdm->cfg_bypass = 0;
//		cobra_pdm->hp_iir_coff0 = 0x80567fd5;
//		cobra_pdm->cfg_pdm_data_delay_phase = 3;
//		cobra_pdm->cfg_gain_steps = 15;
//		cobra_pdm->cfg_sampling = 0;
}

/*
 * "0: pdm clock disable
	1: pdm clock enable"
 *
 */
void rdw_pdm_cfg_clk_pdm_enable(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_pdm_ctrl |= 0x01;
	}
	else
	{
		p_pdm->cfg_pdm_ctrl &= ~0x01;
	}
}


/*
 * "0: left channel disable
	1: left channel enable"
 *
 *
 */
void rdw_pdm_cfg_left_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_pdm_ctrl |= 0x02;
	}
	else
	{
		p_pdm->cfg_pdm_ctrl &= ~0x02;
	}
}


/*
 * "0: right channel disable
	1: right channel enable"
 *
 */
void rdw_pdm_cfg_right_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_pdm_ctrl |= 0x04;
	}
	else
	{
		p_pdm->cfg_pdm_ctrl &= ~0x04;
	}
}

/*
 * "0: normal
	1: left  channel data(from interface) switch to right channel,
	   right channel data(from interface) switch to left channel"
 *
 */
void rdw_pdm_cfg_left_right_swap_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_pdm_ctrl |= 0x08;
	}
	else
	{
		p_pdm->cfg_pdm_ctrl &= ~0x08;
	}
}

/*
 * "0: left channel is sampled by the negedge clk of pdm_clk
	1: left channel is sampled by the posedge clk of pdm_clk "
 *
 */
void rdw_pdm_cfg_left_pdm_to_cic_clkedge_sel(volatile COBRA_PDM_TypeDef * p_pdm, PDM_SAMPLE_CLK_EDGE  edge)
{
	if(edge == PDM_SAMPLE_CLK_NEGEDGE)
	{
		p_pdm->cfg_pdm_ctrl &= ~(1 << 4);
	}
	else if(edge == PDM_SAMPLE_CLK_POSEDGE)
	{
		p_pdm->cfg_pdm_ctrl |= (1 << 4);
	}
}

/*
 * "0: right channel is sampled by the negedge clk of pdm_clk
	1: right channel is sampled by the posedge clk of pdm_clk "
 *
 */
void rdw_pdm_cfg_right_pdm_to_cic_clkedge_sel(volatile COBRA_PDM_TypeDef * p_pdm, PDM_SAMPLE_CLK_EDGE  edge)
{
	if(edge == PDM_SAMPLE_CLK_NEGEDGE)
	{
		p_pdm->cfg_pdm_ctrl &= ~(1 << 5);
	}
	else if(edge == PDM_SAMPLE_CLK_POSEDGE)
	{
		p_pdm->cfg_pdm_ctrl |= (1 << 5);
	}
}

/*
 * "0: left two pin is stero type
	1: right two pin is stero type"
 *
 */
void rdw_pdm_cfg_pdm_who_two_wire_is_stereo(volatile COBRA_PDM_TypeDef * p_pdm, PDM_WHO_IS_STERO  who)
{
	if(who == LEFT_PIN_IS_STERO)
	{
		p_pdm->cfg_pdm_ctrl &= ~(1 << 6);
	}
	else if(who == RIGHT_PIN_IS_STERO)
	{
		p_pdm->cfg_pdm_ctrl |= (1 << 6);
	}
}

/*
 * when enable, pdm_data0 and pdmclk0  is a set of mono,
 * pdm_data1 and pdmclk1 is a set of mono.
 *
 */
void rdw_pdm_cfg_pdm_four_wire_is_2mono(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_pdm_ctrl |= (1 << 7);
	}
	else
	{
		p_pdm->cfg_pdm_ctrl &= ~(1 << 7);
	}
}

/*
 * 	"0: cic disable
	 1: cic enable"
 *
 */
void rdw_pdm_cfg_cic_enable(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_pdm_ctrl1 |= (1 << 0);
	}
	else
	{
		p_pdm->cfg_pdm_ctrl1 &= ~(1 << 0);
	}
}
/*
 * "0: iir disable
	1: iir enable"
 *
 */
void rdw_pdm_cfg_hp_iir_enable(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_pdm_ctrl1 |= (1 << 1);
	}
	else
	{
		p_pdm->cfg_pdm_ctrl1 &= ~(1 << 1);
	}
}
/*
 * "0: osr=64
	1: osr=128"
 *
 */
void rdw_pdm_cfg_osr_128_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_pdm_ctrl1 |= (1 << 2);
	}
	else
	{
		p_pdm->cfg_pdm_ctrl1 &= ~(1 << 2);
	}
}

/*
 * "0: to i2s disable
	1: i2s enable
    (We advice this enable should be late 24 clk_2fs(or 12 clk_1fs) periods to the others enable
       for avoiding iir's random data in the beginning.)"
 *
 */
void rdw_pdm_cfg_i2s_enable(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_pdm_ctrl1 |= (1 << 3);
	}
	else
	{
		p_pdm->cfg_pdm_ctrl1 &= ~(1 << 3);
	}
}


void rdw_pdm_cfg_pdm_bitwid(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int bit_width)
{
		p_pdm->cfg_pdm_ctrl1  &= ~(0x03 << 4);
		p_pdm->cfg_pdm_ctrl1  |= ((bit_width&0x03) << 4);
}


/*
 * 	ratio=pclk/pdm_clk
 *
 */
void rdw_pdm_cfg_pdm_ratio_by_pclk(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int ratio)
{
	p_pdm->cfg_clk_gen =  (ratio&0xff);
}

/*
 * "0: upfifo_full_interrupt can be sent to the cpu
	1: upfifo_full_interrupt is masked"
 *
 */
void rdw_pdm_cfg_upfifo_full_interrupt_mask(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int mask_en)
{
	if(mask_en)
	{
		p_pdm->upfifo_interrupt |= (1 << 0);
	}
	else
	{
		p_pdm->upfifo_interrupt &= ~(1 << 0);
	}
}

/*
 * "0: upfifo_underflow_interrupt can be sent to the cpu
	1: upfifo_underflow_interrupt is masked"
 *
 */
void rdw_pdm_cfg_upfifo_underflow_interrupt_mask(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int mask_en)
{
	if(mask_en)
	{
		p_pdm->upfifo_interrupt |= (1 << 1);
	}
	else
	{
		p_pdm->upfifo_interrupt &= ~(1 << 1);
	}
}

/*
 * "0: upfifo_overflow_interrupt can be sent to the cpu
	1: upfifo_overflow_interrupt is masked"
 *
 */
void rdw_pdm_cfg_upfifo_overflow_interrupt_mask(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int mask_en)
{
	if(mask_en)
	{
		p_pdm->upfifo_interrupt |= (1 << 2);
	}
	else
	{
		p_pdm->upfifo_interrupt &= ~(1 << 2);
	}
}


/*
 * "0: upfifo_empty_interrupt can be sent to the cpu
	1: upfifo_empty_interrupt is masked"
 *
 */
void rdw_pdm_cfg_upfifo_empty_interrupt_mask(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int mask_en)
{
	if(mask_en)
	{
		p_pdm->upfifo_interrupt |= (1 << 3);
	}
	else
	{
		p_pdm->upfifo_interrupt &= ~(1 << 3);
	}
}


/*
 * "0: upfifo_almostfull_interrupt can be sent to the cpu
	1: upfifo_almostfull_interrupt is masked"
 *
 */
void rdw_pdm_cfg_upfifo_almostfull_interrupt_mask(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int mask_en)
{
	if(mask_en)
	{
		p_pdm->upfifo_interrupt |= (1 << 4);
	}
	else
	{
		p_pdm->upfifo_interrupt &= ~(1 << 4);
	}
}


/*
 * upfifo overflow threshold
 *
 * default is 4
 */
void rdw_pdm_cfg_upfifo_threshold(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int threshold)
{
	p_pdm->upfifo_threshold =  (threshold&0x0f);
}


/*
 * upfifo enable
 * (dma upload enable. We advice this enable should be late 24 clk_2fs(or 12 clk_1fs) periods to
 *  the others enable  for avoiding iir's random data in the beginning.)
 *
 */
void rdw_pdm_cfg_upfifo_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->upfifo_ctrl |= (1 << 0);
	}
	else
	{
		p_pdm->upfifo_ctrl &= ~(1 << 0);
	}
}

/*
 * upfifo dma enable
 *
 */
void rdw_pdm_cfg_upfifo_dma_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->upfifo_ctrl |= (1 << 1);
	}
	else
	{
		p_pdm->upfifo_ctrl &= ~(1 << 1);
	}
}

/*
 * The tranfer has some error when the upfifo to dma . This bit should be configured by cpu
 *
 */
void rdw_pdm_cfg_dma_error_signal(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->upfifo_ctrl |= (1 << 2);
	}
	else
	{
		p_pdm->upfifo_ctrl &= ~(1 << 2);
	}
}

/*
 * when enable, upfifo overflow can  stop dma
 *
 */
void rdw_pdm_cfg_dma_stop_by_upfifo_overflow_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->upfifo_ctrl |= (1 << 3);
	}
	else
	{
		p_pdm->upfifo_ctrl &= ~(1 << 3);
	}
}
/*
 * when enable, upfifo underflow can  stop dma
 *
 */
void rdw_pdm_cfg_dma_stop_by_upfifo_underflow_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->upfifo_ctrl |= (1 << 4);
	}
	else
	{
		p_pdm->upfifo_ctrl &= ~(1 << 4);
	}
}
/*
 * when enable, almostfull can be used to trig the dma
 *
 */
void rdw_pdm_cfg_dma_req_by_almostfull(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->upfifo_ctrl |= (1 << 5);
	}
	else
	{
		p_pdm->upfifo_ctrl &= ~(1 << 5);
	}
}
/*
 * the log2 of num which is  the decimation ratio of cic
 *
 */
void rdw_pdm_cfg_log2_cic_decimation(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int decimation)
{
	p_pdm->log2_cic_decimation = decimation&0x0f;
	
	printf("log2_cic_decimation = %d \n", p_pdm->log2_cic_decimation);
}



/*
 * "0: disable hp_iir_bypass
	1: enable hp_iir_byapss"
 *
 */
void rdw_pdm_cfg_hp_iir_bypass_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_bypass |= (1 << 0);
	}
	else
	{
		p_pdm->cfg_bypass &= ~(1 << 0);
	}
}



/*
 * "0: disable pdm_data_delay_bypass
	1: enable pdm_data_delay_bypass"
 *
 */
void rdw_pdm_cfg_pdm_data_delay_bypass_en(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_bypass |= (1 << 1);
	}
	else
	{
		p_pdm->cfg_bypass &= ~(1 << 1);
	}
}

/*
 * 1 order iir , s0 coffeicent
 *
 */
void rdw_pdm_cfg_hp_iir_s0(volatile COBRA_PDM_TypeDef * p_pdm,  int s0_coffeicent)
{
	p_pdm->hp_iir_coff0 &= ~0xffff;
	p_pdm->hp_iir_coff0 |= s0_coffeicent&0xffff;
}

/*
 * 1 order iir , a1 coffeicent
 *
 */
void rdw_pdm_cfg_hp_iir_1order_a1(volatile COBRA_PDM_TypeDef * p_pdm,  int a1_coffeicent)
{
	p_pdm->hp_iir_coff0 &= ~((unsigned int)0xffff << 16);
	p_pdm->hp_iir_coff0 |= ((a1_coffeicent&0xffff) << 16);
}

/*
 * how many pclk cycles delay. At least 2 clock( different clock domain need)
 *
 */
void rdw_pdm_cfg_pdm_data_delay_phase(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int delay_cycle)
{
	p_pdm->cfg_pdm_data_delay_phase = delay_cycle & 0x0f;
}

/*
 * 	0: no attenuation
 * 	1: 6db attenuation
 * 	2: 12db attenuation
 * 	.................
 * 	7:42db attenuation,
 *
 * 	8:6db increase,
 * 	9:12db increase
 * 	.....
 * 	15:48db increase
 *
 */
void rdw_pdm_cfg_gain_steps(volatile COBRA_PDM_TypeDef * p_pdm, PDM_GAIN_STEP step)
{
	p_pdm->cfg_gain_steps &= ~0x0f;
	p_pdm->cfg_gain_steps |= step;
}

/*
 * 0:disable,1:enable
 *
 */
void rdw_pdm_cfg_sampling_enable(volatile COBRA_PDM_TypeDef * p_pdm, unsigned int en)
{
	if(en)
	{
		p_pdm->cfg_sampling |= (1 << 0);
	}
	else
	{
		p_pdm->cfg_sampling &= ~(1 << 0);
	}
}
/*
 *  3'd0:cic_out,
 *  3'd1:cic_out1,
 *  3'd2:cic_out2,
 *  3'd3:hbf1_out,
 *  3'd4:hbf2_out,
 *  3'd5,hp_iir_out,
 *  3'd6:gain_out,
 *  3'd7: pdm_pin
 *
 */
void rdw_pdm_cfg_sampling_object_sel(volatile COBRA_PDM_TypeDef * p_pdm, PDM_SAMPLING_OBJECT object)
{
	p_pdm->cfg_sampling &= ~(0x07 << 1);
	p_pdm->cfg_sampling |= (object << 1);
}

/*
 * read fifo data
 */
unsigned int rdw_pdm_read_upfifo(volatile COBRA_PDM_TypeDef * p_pdm)
{
	return p_pdm->read_upfifo;
}

unsigned int * rdw_pdm_read_upfifo_address(volatile COBRA_PDM_TypeDef * p_pdm)
{
	return (unsigned int *)(&p_pdm->read_upfifo);
}



/*
 *  upfifo_overflow
	upfifo_underflow
	upfifo_full
	upfifo_almostfull
	upfifo_empty
	reserved
	upfifo_filled_num
	reserved

#define PDM_STATE_UPFIFO_OVERFLOW   	0x01
#define PDM_STATE_UPFIFO_UNDERFLOW   	0x02
#define PDM_STATE_UPFIFO_FULL	   		0x04
#define PDM_STATE_UPFIFO_ALMOSTFULL   	0x08
#define PDM_STATE_UPFIFO_EMPTY		    0x10

 *
 */
unsigned int rdw_pdm_upfifo_status(volatile COBRA_PDM_TypeDef * p_pdm)
{
	return p_pdm->upfifo_status;
}


/*
 * upfifo_filled_num
 *
 */
unsigned int rdw_pdm_upfifo_filled_num(volatile COBRA_PDM_TypeDef * p_pdm)
{
	return (p_pdm->upfifo_status >> 8)&0xff;
}


/**********************************************************************************************************************


 PDM common API




************************************************************************************************************************/
/*
 * "0: pdm clock disable
	1: pdm clock enable"
 *
 */
void cobra_pdm_cfg_clk_pdm_enable(pdm_id index, unsigned int en)
{
		if(index == PDM0)
		{
			rdw_pdm_cfg_clk_pdm_enable(cobra_pdm, en);
		}
	
}

/*
 * "0: left channel disable
	1: left channel enable"
 *
 *
 */
void cobra_pdm_cfg_left_en(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_left_en(cobra_pdm, en);
	}


}

/*
 * "0: right channel disable
	1: right channel enable"
 *
 */
void cobra_pdm_cfg_right_en(pdm_id index,  unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_right_en(cobra_pdm, en);
	}
}

/*
 * "0: normal
	1: left  channel data(from interface) switch to right channel,
	   right channel data(from interface) switch to left channel"
 *
 */
void cobra_pdm_cfg_left_right_swap_en(pdm_id index,  unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_left_right_swap_en(cobra_pdm, en);
	}

}

/*
 * "0: left channel is sampled by the negedge clk of pdm_clk
	1: left channel is sampled by the posedge clk of pdm_clk "
 *
 */
void cobra_pdm_cfg_left_pdm_to_cic_clkedge_sel(pdm_id index, PDM_SAMPLE_CLK_EDGE  edge)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_left_pdm_to_cic_clkedge_sel(cobra_pdm, edge);
	}

}

/*
 * "0: right channel is sampled by the negedge clk of pdm_clk
	1: right channel is sampled by the posedge clk of pdm_clk "
 *
 */
void cobra_pdm_cfg_right_pdm_to_cic_clkedge_sel(pdm_id index, PDM_SAMPLE_CLK_EDGE  edge)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_right_pdm_to_cic_clkedge_sel(cobra_pdm, edge);
	}

}


/*
 * "0: left two pin is stero type
	1: right two pin is stero type"
 *
 */
void cobra_pdm_cfg_pdm_who_two_wire_is_stereo(pdm_id index,  PDM_WHO_IS_STERO  who)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_pdm_who_two_wire_is_stereo(cobra_pdm, who);
	}

}

/*
 * when enable, pdm_data0 and pdmclk0  is a set of mono,
 * pdm_data1 and pdmclk1 is a set of mono.
 *
 */
void cobra_pdm_cfg_pdm_four_wire_is_2mono(pdm_id index,  unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_pdm_four_wire_is_2mono(cobra_pdm, en);
	}

}

/*
 * 	"0: cic disable
	 1: cic enable"
 *
 */
void cobra_pdm_cfg_cic_enable(pdm_id index,  unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_cic_enable(cobra_pdm, en);
	}

}

/*
 * "0: iir disable
	1: iir enable"
 *
 */
void cobra_pdm_cfg_hp_iir_enable(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_hp_iir_enable(cobra_pdm, en);
	}

}

/*
 * "0: osr=64
	1: osr=128"
 *
 */
void cobra_pdm_cfg_osr_128_en(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_osr_128_en(cobra_pdm, en);
	}

}

/*
 * "0: to i2s disable
	1: i2s enable
    (We advice this enable should be late 24 clk_2fs(or 12 clk_1fs) periods to the others enable
       for avoiding iir's random data in the beginning.)"
 *
 */
void cobra_pdm_cfg_i2s_enable(pdm_id index,  unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_i2s_enable(cobra_pdm, en);
	}

}

void cobra_pdm_cfg_pdm_bit_width(pdm_id index,  unsigned int bitwidth)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_pdm_bitwid(cobra_pdm, bitwidth);
	}

}


/*
 * 	ratio=pclk/pdm_clk
 *
 */
void cobra_pdm_cfg_pdm_ratio_by_pclk(pdm_id index, unsigned int ratio)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_pdm_ratio_by_pclk(cobra_pdm, ratio);
	}

}


/*
 * "0: upfifo_full_interrupt can be sent to the cpu
	1: upfifo_full_interrupt is masked"
 *
 */
void cobra_pdm_cfg_upfifo_full_interrupt_mask(pdm_id index, unsigned int mask_en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_upfifo_full_interrupt_mask(cobra_pdm, mask_en);
	}

}


/*
 * "0: upfifo_underflow_interrupt can be sent to the cpu
	1: upfifo_underflow_interrupt is masked"
 *
 */
void cobra_pdm_cfg_upfifo_underflow_interrupt_mask(pdm_id index, unsigned int mask_en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_upfifo_underflow_interrupt_mask(cobra_pdm, mask_en);
	}

}


/*
 * "0: upfifo_overflow_interrupt can be sent to the cpu
	1: upfifo_overflow_interrupt is masked"
 *
 */
void cobra_pdm_cfg_upfifo_overflow_interrupt_mask(pdm_id index,  unsigned int mask_en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_upfifo_overflow_interrupt_mask(cobra_pdm, mask_en);
	}

}


/*
 * "0: upfifo_empty_interrupt can be sent to the cpu
	1: upfifo_empty_interrupt is masked"
 *
 */
void cobra_pdm_cfg_upfifo_empty_interrupt_mask(pdm_id index, unsigned int mask_en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_upfifo_empty_interrupt_mask(cobra_pdm, mask_en);
	}

}


/*
 * "0: upfifo_almostfull_interrupt can be sent to the cpu
	1: upfifo_almostfull_interrupt is masked"
 *
 */
void cobra_pdm_cfg_upfifo_almostfull_interrupt_mask(pdm_id index, unsigned int mask_en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_upfifo_almostfull_interrupt_mask(cobra_pdm, mask_en);
	}

}


/*
 * upfifo overflow threshold
 *
 * default is 4
 */
void cobra_pdm_cfg_upfifo_threshold(pdm_id index, unsigned int threshold)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_upfifo_threshold(cobra_pdm, threshold);
	}

}

/*
 * upfifo enable
 * (dma upload enable. We advice this enable should be late 24 clk_2fs(or 12 clk_1fs) periods to
 *  the others enable  for avoiding iir's random data in the beginning.)
 *
 */
void cobra_pdm_cfg_upfifo_en(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_upfifo_en(cobra_pdm, en);
	}

}


/*
 * upfifo dma enable
 *
 */
void cobra_pdm_cfg_upfifo_dma_en(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_upfifo_dma_en(cobra_pdm, en);
	}

}

/*
 * The tranfer has some error when the upfifo to dma . This bit should be configured by cpu
 *
 */
void cobra_pdm_cfg_dma_error_signal(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_dma_error_signal(cobra_pdm, en);
	}

}

/*
 * when enable, upfifo overflow can  stop dma
 *
 */
void cobra_pdm_cfg_dma_stop_by_upfifo_overflow_en(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_dma_stop_by_upfifo_overflow_en(cobra_pdm, en);
	}

}

/*
 * when enable, upfifo underflow can  stop dma
 *
 */
void cobra_pdm_cfg_dma_stop_by_upfifo_underflow_en(pdm_id index,  unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_dma_stop_by_upfifo_underflow_en(cobra_pdm, en);
	}

}

/*
 * when enable, almostfull can be used to trig the dma
 *
 */
void cobra_pdm_cfg_dma_req_by_almostfull(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_dma_req_by_almostfull(cobra_pdm, en);
	}

}

/*
 * the log2 of num which is  the decimation ratio of cic
 *
 */
void cobra_pdm_cfg_log2_cic_decimation(pdm_id index, unsigned int decimation)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_log2_cic_decimation(cobra_pdm, decimation);
	}

}

/*
 * "0: disable hp_iir_bypass
	1: enable hp_iir_byapss"
 *
 */
void cobra_pdm_cfg_hp_iir_bypass_en(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_hp_iir_bypass_en(cobra_pdm, en);
	}

}

/*
 * "0: disable pdm_data_delay_bypass
	1: enable pdm_data_delay_bypass"
 *
 */
void cobra_pdm_cfg_pdm_data_delay_bypass_en(pdm_id index,  unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_pdm_data_delay_bypass_en(cobra_pdm, en);
	}

}

/*
 * 1 order iir , s0 coffeicent
 *
 */
void cobra_pdm_cfg_hp_iir_s0(pdm_id index,  int s0_coffeicent)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_hp_iir_s0(cobra_pdm, s0_coffeicent);
	}

}

/*
 * 1 order iir , a1 coffeicent
 *
 */
void cobra_pdm_cfg_hp_iir_1order_a1(pdm_id index,  int a1_coffeicent)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_hp_iir_1order_a1(cobra_pdm, a1_coffeicent);
	}

}

/*
 * how many pclk cycles delay. At least 2 clock( different clock domain need)
 *
 */
void cobra_pdm_cfg_pdm_data_delay_phase(pdm_id index, unsigned int delay_cycle)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_pdm_data_delay_phase(cobra_pdm, delay_cycle);
	}

}

/*
 * 	0: no attenuation
 * 	1: 6db attenuation
 * 	2: 12db attenuation
 * 	.................
 * 	7:42db attenuation,
 *
 * 	8:6db increase,
 * 	9:12db increase
 * 	.....
 * 	15:48db increase
 *
 */
void cobra_pdm_cfg_gain_steps(pdm_id index, PDM_GAIN_STEP step)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_gain_steps(cobra_pdm, step);
	}

}

/*
 * 0:disable,1:enable
 *
 */
void cobra_pdm_cfg_sampling_enable(pdm_id index, unsigned int en)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_sampling_enable(cobra_pdm, en);
	}

}

/*
 *  3'd0:cic_out,
 *  3'd1:cic_out1,
 *  3'd2:cic_out2,
 *  3'd3:hbf1_out,
 *  3'd4:hbf2_out,
 *  3'd5,hp_iir_out,
 *  3'd6:gain_out,
 *  3'd7: pdm_pin
 *
 */
void cobra_pdm_cfg_sampling_object_sel(pdm_id index, PDM_SAMPLING_OBJECT object)
{
	if(index == PDM0)
	{
		rdw_pdm_cfg_sampling_object_sel(cobra_pdm, object);
	}

}

/*
 * read fifo data
 */
unsigned int cobra_pdm_read_upfifo(pdm_id index)
{
	if(index == PDM0)
	{
		return rdw_pdm_read_upfifo(cobra_pdm);
	}

	return 0;
}

unsigned int * cobra_pdm_read_upfifo_address(pdm_id index)
{
		if(index == PDM0)
		{
			return rdw_pdm_read_upfifo_address(cobra_pdm);
		}
		return NULL;
}

/*
 *  upfifo_overflow
	upfifo_underflow
	upfifo_full
	upfifo_almostfull
	upfifo_empty
	reserved
	upfifo_filled_num
	reserved

#define PDM_STATE_UPFIFO_OVERFLOW   	0x01
#define PDM_STATE_UPFIFO_UNDERFLOW   	0x02
#define PDM_STATE_UPFIFO_FULL	   		0x04
#define PDM_STATE_UPFIFO_ALMOSTFULL   	0x08
#define PDM_STATE_UPFIFO_EMPTY		    0x10

 *
 */
unsigned int cobra_pdm_upfifo_status(pdm_id index)
{
	if(index == PDM0)
	{
		return rdw_pdm_upfifo_status(cobra_pdm);
	}
	return 0;
}

/*
 * upfifo_filled_num
 *
 */
unsigned int cobra_pdm_upfifo_filled_num(pdm_id index)
{
	if(index == PDM0)
	{
		return rdw_pdm_upfifo_filled_num(cobra_pdm);
	}
	return 0;
}


//application API

#define IIR_1FS_COEFF0   0.998689944140625*pow(2,15)
#define IIR_1FS_COEFF1   -0.99737548828125*pow(2,15)

void app_cobra_pdm_init(pdm_id index, uint8_t dma_en, uint8_t osr, float pclk_kHz1)
{
	float temp = cobra_system_get_m3_apb2_clk();
	
	uint8_t temp_osr;
	
	printf("apb clk temp = %f \n", temp);
	
	temp /= 1000;
	
	cobra_pdm_cfg_left_pdm_to_cic_clkedge_sel(index, PDM_SAMPLE_CLK_POSEDGE);
	cobra_pdm_cfg_right_pdm_to_cic_clkedge_sel(index, PDM_SAMPLE_CLK_POSEDGE);
	cobra_pdm_cfg_pdm_who_two_wire_is_stereo(index, RIGHT_PIN_IS_STERO);
	cobra_pdm_cfg_clk_pdm_enable(index, 1);

	cobra_pdm_cfg_pdm_data_delay_bypass_en(index, 1);

	cobra_pdm_cfg_pdm_ratio_by_pclk(index, ((int)((temp/(osr*15.625f))+0.5)));
	cobra_pdm_cfg_log2_cic_decimation(index, log2(osr/4));
	
	
	cobra_pdm_cfg_hp_iir_s0(index, IIR_1FS_COEFF0);
	cobra_pdm_cfg_hp_iir_1order_a1(index, IIR_1FS_COEFF1);
		
	cobra_pdm_cfg_gain_steps(index, GAIN_STEP_12DB_ATTENUATION);
	

	cobra_pdm_cfg_pdm_bit_width(index, 2);
	
	cobra_pdm_cfg_cic_enable(index, 1);
	cobra_pdm_cfg_i2s_enable(index, 1);
	cobra_pdm_cfg_hp_iir_enable(index, 1);

	if(osr == 128)
	{
		cobra_pdm_cfg_osr_128_en(index, 1);
	}

	cobra_pdm_cfg_sampling_enable(index, 1);
	
	cobra_pdm_cfg_sampling_object_sel(index, SAMPLING_GAIN_OUT);

//	cobra_soft_delay_ms(1);

	cobra_pdm_cfg_upfifo_en(index, 1);

	if(dma_en)
	{
		cobra_pdm_cfg_upfifo_dma_en(index, 1);
	}
}

void app_cobra_pdm_disable(pdm_id index)
{
	cobra_pdm_cfg_left_en(index, 0);
	cobra_pdm_cfg_right_en(index, 0);
	cobra_pdm_cfg_clk_pdm_enable(index, 0);

	cobra_pdm_cfg_upfifo_en(index, 0);
	cobra_pdm_cfg_upfifo_dma_en(index, 0);
	
//	cobra_pdm_debug();

}

void app_cobra_pdm_enable(pdm_id index)
{
	cobra_pdm_cfg_left_en(index, 1);
	cobra_pdm_cfg_right_en(index, 1);
	cobra_pdm_cfg_clk_pdm_enable(index, 1);

	cobra_pdm_cfg_upfifo_en(index, 1);
	cobra_pdm_cfg_upfifo_dma_en(index, 1);
	
	cobra_pdm_debug();
}


#endif














