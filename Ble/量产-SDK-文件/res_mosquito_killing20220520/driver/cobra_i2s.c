/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_i2s.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/8

	Description :   


	Changed Log	:

		Liujian  2019/4/8			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_i2s.h"


#if COBRA_I2S_EN

#include "math.h"
#include "cobra_pcrm.h"
#include "cobra4_aon_wk.h"
#include "cobra_pll.h"

#if (COBRA_POWER_DOWN_ENABLE)
#ifdef COBRA_I2S_NEW
volatile COBRA_I2S_TypeDef *  cobra_i2s0		__attribute__((section("retention_area"),zero_init));
volatile COBRA_I2S_TypeDef *  cobra_i2s1		__attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_I2S_TypeDef *  cobra_i2s0		__attribute__((section("retention_area"),zero_init));
static volatile COBRA_I2S_TypeDef *  cobra_i2s1		__attribute__((section("retention_area"),zero_init));
#endif

#else

#ifdef COBRA_I2S_NEW
volatile COBRA_I2S_TypeDef *  cobra_i2s0;
volatile COBRA_I2S_TypeDef *  cobra_i2s1;
#else
static volatile COBRA_I2S_TypeDef *  cobra_i2s0;
static volatile COBRA_I2S_TypeDef *  cobra_i2s1;
#endif

#endif

void cobra_i2s_init(void)
{
		cobra_i2s0 = (COBRA_I2S_TypeDef * )COBRA_I2S0_BASE;
		cobra_i2s1 = (COBRA_I2S_TypeDef * )COBRA_I2S1_BASE;
}

/*
 * "0: tx_rx use seperated ws and sclk signal
	1: tx_rx use common ws and common sclk signal when tx and rx is master mode "
 *
 */
void rdw_i2s_cfg_txrx_common_mode_en(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TXRX_COMMON_MODE |= 0x01;
	}
	else
	{
		p_i2s->CFG_TXRX_COMMON_MODE &= ~0x01;
	}
}

/*
 * "0: tx and rx is idle
	1: tx and rx begin to work when tx and rx is master mode "
 *
 */
void rdw_i2s_cfg_txrx_common_mode_syn(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TXRX_COMMON_MODE |= 0x02;
	}
	else
	{
		p_i2s->CFG_TXRX_COMMON_MODE &= ~0x02;
	}
}

/*
 * 0: tx close      1:tx enable
 *
 */
void rdw_i2s_tx_ctrl_enable(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TX_CTRL |= 0x01;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x01;
	}
}

/*
 * 0: TX Module is slave
 * 1: TX Module is master
 *
 */
void rdw_i2s_tx_ctrl_master_en(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TX_CTRL |= 0x02;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x02;
	}
}

/*
 *  0: CPU wite dnfifo0  to I2S for transmitting
 *  1: Internal port write dnfifo1 to I2S for transmitting
 *
 */
void rdw_i2s_tx_ctrl_who_to_i2s(volatile COBRA_I2S_TypeDef *p_i2s, I2S_WHO_TO_I2S source)
{
	if(source == INTERNAL_WRITE_DNFIFO1)
	{
		p_i2s->CFG_TX_CTRL |= 0x04;
	}
	else if(source == CPU_WRITE_DNFIFO0)
	{
		p_i2s->CFG_TX_CTRL &= ~0x04;
	}
}

/*
 * 	0: dnfifo0 close  (dnfifo0 for cpu)
 * 	1: dnfifo0 enable
 *
 */
void rdw_i2s_tx_ctrl_in0_dnfifo_enable(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TX_CTRL |= 0x08;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x08;
	}
}

/*
 *  0: dnfifo1 close  (dnfifo1 for internal port)
 *  1: dnfifo1 enable
 *
 */
void rdw_i2s_tx_ctrl_in1_dnfifo_enable(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TX_CTRL |= 0x10;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x10;
	}
}

/*
 * when special ,set initial WS , we should set it 1.
 * In general, we need not set it if we can follow the given timing.
 *
 */
void rdw_i2s_tx_ctrl_ws_init_vld(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TX_CTRL |= 0x20;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x20;
	}
}

/*
 *  0: initial WS is 0
 * 	1: initial WS is 1
 *
 */
void rdw_i2s_tx_ctrl_ws_init(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int value)
{
	if(value)
	{
		p_i2s->CFG_TX_CTRL |= 0x40;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x40;
	}
}

/*
 * when special ,set initial bit_delay, we should set it 1.
 * In general, we need not set it if we can follow the given timing.
 *
 */
void rdw_i2s_tx_ctrl_bit_delay_vld(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TX_CTRL |= 0x80;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x80;
	}
}

/*
 *   0: need not delay one cycle (ex. RJD)
     1: need delay one cycle (ex. I2S)
 *
 */
void rdw_i2s_tx_ctrl_bit_delay(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int value)
{
	if(value)
	{
		p_i2s->CFG_TX_CTRL |= 0x100;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x100;
	}
}

/*
 *  when special ,we should set it 1.
 *  In general, we need not set it if we can follow the given timing.
 *
 */
void rdw_i2s_tx_ctrl_left_aligned_vld(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TX_CTRL |= 0x200;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x200;
	}
}

/*
 * "0: right aligned
	1: left aligned"
 *
 */
void rdw_i2s_tx_ctrl_left_aligned_en(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TX_CTRL |= 0x400;
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~0x400;
	}
}

/*
 * "mode=PCM_SHORT_FRAME,PCM_LONG_FRAME,
	0: use default slot num
	1: use slot num that cpu config"
 *
 */
void rdw_i2s_tx_ctrl_tx_slot_num_vld(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_TX_CTRL |= (1<<12);
	}
	else
	{
		p_i2s->CFG_TX_CTRL &= ~(1 << 12);
	}
}

/*
 * "4'd0:I2S ,
 *  4'd1:LJD,
 *  4'd2:RJD,
 *  4'd3:PCM_STERO
	4'd4:PCM_MONO,
	4'd5:PCM_SHORT_FRAME,
	4'd6:PCM_LONG_FRAME"
 *
 */
void rdw_i2s_cf_tx_mode(volatile COBRA_I2S_TypeDef *p_i2s, i2s_work_mode mode)
{
	 switch(mode)
	 {
	 	 case I2S_MODE:
	 		 p_i2s->CF_TX_MODE &= ~0xf;
	 		 p_i2s->CF_TX_MODE |= 0;
	 		 break;
	 	case LEFT_JUSTED_MODE:
			 p_i2s->CF_TX_MODE &= ~0xf;
			 p_i2s->CF_TX_MODE |= 1;
			 break;
	 	case RIGHT_JUSTED_MODE:
	 		 p_i2s->CF_TX_MODE &= ~0xf;
	 		 p_i2s->CF_TX_MODE |= 2;
	 		 break;
	 	case PCM_STERO_MODE:
	 		 p_i2s->CF_TX_MODE &= ~0xf;
	 		 p_i2s->CF_TX_MODE |= 3;
	 		 break;
	 	case PCM_MONO_MODE:
	 		 p_i2s->CF_TX_MODE &= ~0xf;
	 		 p_i2s->CF_TX_MODE |= 4;
	 		 break;
		case PCM_SHORT_FRAME_MODE:
			 p_i2s->CF_TX_MODE &= ~0xf;
			 p_i2s->CF_TX_MODE |= 5;
			 break;
		case PCM_LONG_FRAME:
			 p_i2s->CF_TX_MODE &= ~0xf;
			 p_i2s->CF_TX_MODE |= 6;
			 break;
	 	 default:
	 		 break;
	 }
}

/*
 * it should be set in (>=4 && <=32);
 * or set in (>=34 && <=64)
 *
 */
void rdw_i2s_tx_biwidth(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int width)
{
	p_i2s->CFG_TX_BITWIDTH_OF_WORD = (width&0xff);
}

/*
 * when set ,the condition should be ok:
 * 1. >=4,
 * 2. cfg_tx_sampling_point_half_of_fs >=cfg_tx_bitwidth_of_word
 *
 */
void rdw_i2s_tx_sampling_point_half_of_fs(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int sample_point)
{
	p_i2s->CFG_TX_SAMPLING_POINT_HALF_OF_FS = (sample_point&0xff);
}

/*
 * it should be set >=2 when the  interface is set pcm long frame format
 *
 */
void rdw_i2s_tx_pcm_long_frame_holding(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int holding)
{
	p_i2s->CFG_TX_PCM_LONG_FRAME_HOLDING = (holding&0xff);
}

/*
 * 	mono:  1~14 , stero, 2~14
 *
 */
void rdw_i2s_cfg_tx_in0_dnfifo_almostfull_threshold(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int value)
{
	p_i2s->CFG_TX_IN0_DNFIFO_ALMOSTFULL_THRESHOLD = (value&0xff);
}

/*
 * range: 1~8. we advice it is 2
 *
 */
void rdw_i2s_cfg_tx_in1_dnfifo_almostfull_threshold(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int value)
{
	p_i2s->CFG_TX_IN1_DNFIFO_ALMOSTFULL_THRESHOLD = (value&0xff);
}

/*
 * "mode=PCM_SHORT_FRAME,PCM_LONG_FRAME,
	slot num , range:1 ~4. when set 0, it still 1."
 *
 */
void rdw_i2s_tx_slot_num(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int slot_number)
{
	p_i2s->CFG_TX_SLOT_NUM = (slot_number&0xff);
}

/*
 * 0: rx close      1:rx enable
 *
 */
void rdw_i2s_cfg_rx_ctrl_rx_enable(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_RX_CTRL |= 0x01;
	}
	else
	{
		p_i2s->CFG_RX_CTRL &= ~0x01;
	}
}

/*
 * 0: rx Module is slave
 * 1: rx Module is master
 *
 */
void rdw_i2s_cfg_rx_ctrl_rx_master(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_RX_CTRL |= 0x02;
	}
	else
	{
		p_i2s->CFG_RX_CTRL &= ~0x02;
	}
}

/*
 *  0: rx data to cpu
 *  1: rx data to internal port
 *
 */
void rdw_i2s_cfg_rx_ctrl_i2s_to_who(volatile COBRA_I2S_TypeDef *p_i2s, I2S_I2S_TO_WHO dest)
{
	if(dest == RX_DATA_TO_INTERNAL)
	{
		p_i2s->CFG_RX_CTRL |= 0x04;
	}
	else if(dest == RX_DATA_TO_CPU)
	{
		p_i2s->CFG_RX_CTRL &= ~0x04;
	}
}

/*
 * 0: upfifo0 close  (upfifo0 for cpu)
 * 1: upfifo0 enable
 *
 */
void rdw_i2s_cfg_rx_ctrl_out0_fifo_ena(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_RX_CTRL |= 0x08;
	}
	else
	{
		p_i2s->CFG_RX_CTRL &= ~0x08;
	}
}

/*
 *  when special ,set initial WS , we should set it 1.
 *  In general, we need not set it if we can follow the given timing.
 *
 */
void rdw_i2s_cfg_rx_ctrl_ws_init_vld(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_RX_CTRL |= 0x10;
	}
	else
	{
		p_i2s->CFG_RX_CTRL &= ~0x10;
	}
}

/*
 * "0: initial WS is 0
	1; initial WS is 1"
 *
 */
void rdw_i2s_cfg_rx_ctrl_ws_init(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int value)
{
	if(value)
	{
		p_i2s->CFG_RX_CTRL |= 0x20;
	}
	else
	{
		p_i2s->CFG_RX_CTRL &= ~0x20;
	}
}

/*
 * when special ,set initial bit_delay, we should set it 1.
 * In general, we need not set it if we can follow the given timing.
 *
 */
void rdw_i2s_cfg_rx_ctrl_bit_delay_vld(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_RX_CTRL |= 0x40;
	}
	else
	{
		p_i2s->CFG_RX_CTRL &= ~0x40;
	}
}

/*
 * "0: need not delay one cycle (ex. RJD)
	1: need delay one cycle (ex. I2S)"
 *
 */
void rdw_i2s_cfg_rx_ctrl_bit_delay(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int value)
{
	if(value)
	{
		p_i2s->CFG_RX_CTRL |= 0x80;
	}
	else
	{
		p_i2s->CFG_RX_CTRL &= ~0x80;
	}
}

/*
 * "mode=PCM_SHORT_FRAME,PCM_LONG_FRAME,
	0: use default slot num
	1: use slot num that cpu config"
 *
 */
void rdw_i2s_cfg_rx_ctrl_rx_slot_num_vld(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_RX_CTRL |= 1<<10;
	}
	else
	{
		p_i2s->CFG_RX_CTRL &= ~(1<<10);
	}
}

/*
 * "4'd0:I2S ,
 *  4'd1:LJD,
 *  4'd2:RJD,
 *  4'd3:PCM_STERO
	4'd4:PCM_MONO,
	4'd5:PCM_SHORT_FRAME,
	4'd6:PCM_LONG_FRAME"
 *
 */
void rdw_i2s_cfg_rx_mode(volatile COBRA_I2S_TypeDef *p_i2s, i2s_work_mode  mode)
{
	switch(mode)
	{
		case I2S_MODE:
			p_i2s->CFG_RX_MODE &= ~0xf;
			p_i2s->CFG_RX_MODE |= 0x00;
			break;
		case LEFT_JUSTED_MODE:
			p_i2s->CFG_RX_MODE &= ~0xf;
			p_i2s->CFG_RX_MODE |= 0x01;
			break;
		case RIGHT_JUSTED_MODE:
			p_i2s->CFG_RX_MODE &= ~0xf;
			p_i2s->CFG_RX_MODE |= 0x02;
			break;
		case PCM_STERO_MODE:
			p_i2s->CFG_RX_MODE &= ~0xf;
			p_i2s->CFG_RX_MODE |= 0x03;
			break;
		case PCM_MONO_MODE:
			p_i2s->CFG_RX_MODE &= ~0xf;
			p_i2s->CFG_RX_MODE |= 0x04;
			break;
		case PCM_SHORT_FRAME_MODE:
			p_i2s->CFG_RX_MODE &= ~0xf;
			p_i2s->CFG_RX_MODE |= 0x05;
			break;
		case PCM_LONG_FRAME:
			p_i2s->CFG_RX_MODE &= ~0xf;
			p_i2s->CFG_RX_MODE |= 0x06;
			break;
		default:
			break;
	}
}

/*
 *  it should be set in (>=4 && <=32);
 *  or set in (>=34 && <=64)
 *  when mode=PCM_SHORT_FRAME,PCM_LONG_FRAME, cfg_rx_bitwidth_of_word is the  bits of one slot,
 *  other mode is the single channel bits.
 *
 */
void rdw_i2s_rx_bitwidth_of_word(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int width)
{
	p_i2s->CFG_RX_BITWIDTH_OF_WORD = (width&0xff);
}

/*
 * when set ,the condition should be ok:
 * 1. >=4,
 * 2.cfg_tx_sampling_point_half_of_fs >=cfg_tx_bitwidth_of_word;
 *   when mode=PCM_MONO,PCM_SHORT_FRAME,PCM_LONG_FRAME,cfg_rx_sampling_point_half_of_fs is the full of fs.
 *
 */
void rdw_i2s_rx_sampling_point_half_of_fs(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int sample_point)
{
	p_i2s->CFG_RX_SAMPLING_POINT_HALF_OF_FS = (sample_point&0xff);
}

/*
 * it should be set >=2 when the  interface is set pcm long frame format
 *
 */
void rdw_i2s_rx_pcm_long_frame_holding(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int holding)
{
	p_i2s->CFG_RX_PCM_LONG_FRAME_HOLDING = (holding&0xff);
}

/*
 * "mode=PCM_SHORT_FRAME,PCM_LONG_FRAME,
	slot num , range:1 ~4; when set 0, it still 1."
 *
 */
void rdw_i2s_rx_slot_num(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int slot_number)
{
	p_i2s->CFG_RX_SLOT_NUM = (slot_number&0xf);
}


/*
 * "0: dma disable
	1: dma enable"
 *
 */
void rdw_i2s_cfg_dma_tx_enable(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->DMA_CTRL_SEL |= 0x01;
	}
	else
	{
		p_i2s->DMA_CTRL_SEL &= ~0x01;
	}
}

/*
 * "0: when fifo overflow or underflow, dma still req
	1: when fifo overflow or underflow, dma no req "
 *
 */
void rdw_i2s_cfg_dma_tx_stop_by_overrun_interrupt(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->DMA_CTRL_SEL |= 0x02;
	}
	else
	{
		p_i2s->DMA_CTRL_SEL &= ~0x02;
	}
}

/*
 * when software learn dma transfer has some error from dma arbitor,
 * software write dma_error , dma req can be disabled.
 *
 */
void rdw_i2s_cfg_dma_tx_error_signal(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->DMA_CTRL_SEL |= 0x04;
	}
	else
	{
		p_i2s->DMA_CTRL_SEL &= ~0x04;
	}
}

/*
 * dma request by fifo's almostfull
 *
 */
void rdw_i2s_cfg_dma_tx_req_by_almostfull(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->DMA_CTRL_SEL |= 0x08;
	}
	else
	{
		p_i2s->DMA_CTRL_SEL &= ~0x08;
	}
}

/*
 * "0: dma disable
	1: dma enable"
 *
 */
void rdw_i2s_cfg_dma_rx_enable(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->DMA_CTRL_SEL |= 0x10;
	}
	else
	{
		p_i2s->DMA_CTRL_SEL &= ~0x10;
	}
}

/*
 * "0: when fifo overflow or underflow, dma still req
	1: when fifo overflow or underflow, dma no req "
 *
 */
void rdw_i2s_cfg_dma_rx_stop_by_overrun_interrupt(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->DMA_CTRL_SEL |= 0x20;
	}
	else
	{
		p_i2s->DMA_CTRL_SEL &= ~0x20;
	}
}

/*
 * when software learn dma transfer has some error from dma arbitor, software write dma_error ,
 * dma req can be disabled.
 *
 */
void rdw_i2s_cfg_dma_rx_error_signal(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->DMA_CTRL_SEL |= 0x40;
	}
	else
	{
		p_i2s->DMA_CTRL_SEL &= ~0x40;
	}
}

/*
 * dma request by fifo's almostfull
 *
 */
void rdw_i2s_cfg_dma_rx_req_by_almostfull(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->DMA_CTRL_SEL |= 0x80;
	}
	else
	{
		p_i2s->DMA_CTRL_SEL &= ~0x80;
	}
}

/*
 * the threshold to give a signal of almostfull of infifo0
 *
 */
void rdw_i2s_in0_almostfull_threshold(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int threshold)
{
	p_i2s->DMA_CTRL_SEL &= ~(0xff << 8);
	p_i2s->DMA_CTRL_SEL |= ((threshold&0xff) << 8);
}

/*
 * the threshold to give a signal of almostfull of upfifo0
 *
 */
void rdw_i2s_out0_almostfull_threshold(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int threshold)
{
	p_i2s->DMA_CTRL_SEL &= ~(0xff << 16);
	p_i2s->DMA_CTRL_SEL |= ((threshold&0xff) << 16);
}

/*
 * the data that cpu write to i2s
 *
 */
void rdw_i2s_in0_dnfifo_wen(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int value)
{
	p_i2s->IN0_DNFIFO_WRITE_DATA = value;
}

/*
 * the data that cpu read from i2s
 *
 */
unsigned int rdw_i2s_out0_upfifo_ren(volatile COBRA_I2S_TypeDef *p_i2s)
{
	return p_i2s->OUT0_UPFIFO_READ_DATA;
}


/*
 * upfifo0 full  interrupt mask
 *
 */
void rdw_i2s_out0_upfifo_full_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x01;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x01;
	}
}

/*
 * upfifo0 underflow interrupt mask
 *
 */
void rdw_i2s_out0_upfifo_underflow_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x02;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x02;
	}
}

/*
 * upfifo0 overflow interrupt mask
 *
 */
void rdw_i2s_out0_upfifo_overflow_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x04;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x04;
	}
}

/*
 * upfifo0 empty interrupt mask
 *
 */
void rdw_i2s_out0_upfifo_empty_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x08;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x08;
	}
}

/*
 * upfifo0 almost_full interrupt mask
 *
 */
void rdw_i2s_out0_upfifo_almostfull_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x10;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x10;
	}
}

/*
 * dnfifo0 full  interrupt mask
 *
 */
void rdw_i2s_in0_dnfifo_full_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x20;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x20;
	}
}

/*
 * dnfifo0 underflow interrupt mask
 *
 */
void rdw_i2s_in0_dnfifo_underflow_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x40;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x40;
	}
}

/*
 * dnfifo0 overflow interrupt mask
 *
 */
void rdw_i2s_in0_dnfifo_overflow_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x80;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x80;
	}
}

/*
 *  dnfifo0 empty interrupt mask
 *
 */
void rdw_i2s_in0_dnfifo_empty_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x100;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x100;
	}
}

/*
 * dnfifo0 almost_full interrupt mask
 *
 */
void rdw_i2s_in0_dnfifo_almostfull_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x200;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x200;
	}
}

/*
 *  dnfifo1 full interrupt mask
 *
 */
void rdw_i2s_in1_dnfifo_full_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 0x400;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~0x400;
	}
}

/*
 * dnfifo1 underflow interrupt mask
 *
 */
void rdw_i2s_in1_dnfifo_underflow_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 1<<11;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~(1<<11);
	}
}

/*
 * dnfifo1 overflow interrupt mask
 *
 */
void rdw_i2s_in1_dnfifo_overflow_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 1<<12;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~(1<<12);
	}
}

/*
 * dnfifo1 empty interrupt mask
 *
 */
void rdw_i2s_in1_dnfifo_empty_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 1<<13;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~(1<<13);
	}
}

/*
 * dnfifo1 almost_full interrupt mask
 *
 */
void rdw_i2s_in1_dnfifo_almostfull_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 1<<14;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~(1<<14);
	}
}

/*
 * upfifo0 interrupt main mask
 *
 */
void rdw_i2s_out0_upfifo_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 1<<15;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~(1<<15);
	}
}

/*
 * dnfifo0 interrupt main mask
 *
 */
void rdw_i2s_in0_dnfifo_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 1<<16;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~(1<<16);
	}
}


/*
 * dnfifo1 interrupt main mask
 *
 */
void rdw_i2s_in1_dnfifo_interrupt_mask(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->CFG_INTERRUPT_MASK |= 1<<17;
	}
	else
	{
		p_i2s->CFG_INTERRUPT_MASK &= ~(1<<17);
	}
}

/*
 * rx ws signal swap to tx
 *
 */
void rdw_i2s_tx_ws_to_rx(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->WS_SWAP |= 0x01;
	}
	else
	{
		p_i2s->WS_SWAP &= ~(0x01);
	}
}

/*
 * tx ws signal swap to rx
 *
 */
void rdw_i2s_rx_ws_to_tx(volatile COBRA_I2S_TypeDef *p_i2s, unsigned int en)
{
	if(en)
	{
		p_i2s->WS_SWAP |= 0x02;
	}
	else
	{
		p_i2s->WS_SWAP &= ~(0x02);
	}
}

/*
 * 	upfifo0 overflow
	upfifo0 underflow
	upfifo0 full
	upfifo0 almostfull
	upfifo0 empty

	upfifo0 filled num
#define STATE_OUT0_UPFIFO_OVERFLOW			0x01
#define STATE_OUT0_UPFIFO_UNDERFLOW			0x02
#define STATE_OUT0_UPFIFO_FULL				0x04
#define STATE_OUT0_UPFIFO_ALMOSTFULL		0x08
#define STATE_OUT0_UPFIFO_EMPTY				0x10

 *
 */
unsigned int rdw_i2s_rx_out0_upfifo_statu(volatile COBRA_I2S_TypeDef *p_i2s)
{
	return p_i2s->RX_OUT0_UPFIFO_STATU;
}

/*
 * upfifo0 filled num
 *
 */
unsigned int rdw_i2s_out0_upfifo_filled_num(volatile COBRA_I2S_TypeDef *p_i2s)
{
	return (p_i2s->RX_OUT0_UPFIFO_STATU >> 8)&0xff;
}

/*
 * 	 dnfifo0 overflow
	 dnfifo0 underflow
	 dnfifo0 full
	 dnfifo0 almostfull
	 dnfifo0 empty

 	 in0_dnfifo_filled_num={3'b0,in0_dnfifo_rfilled_num,3'b0,in0_dnfifo_wfilled_num}

#define STATE_IN0_UPFIFO_OVERFLOW		0x01
#define STATE_IN0_UPFIFO_UNDERFLOW		0x02
#define STATE_IN0_UPFIFO_FULL			0x04
#define STATE_IN0_UPFIFO_ALMOSTFULL		0x08
#define STATE_IN0_UPFIFO_EMPTY			0x10

 *
 */
unsigned int rdw_i2s_tx_in0_dnfifo_status(volatile COBRA_I2S_TypeDef *p_i2s)
{
	return p_i2s->TX_IN0_DNFIFO_STATUS;
}

/*
 *  in0_dnfifo_filled_num={3'b0,in0_dnfifo_rfilled_num,3'b0,in0_dnfifo_wfilled_num}
 *
 */
unsigned int rdw_i2s_in0_dnfifo_wfilled_num(volatile COBRA_I2S_TypeDef *p_i2s)
{
	return (p_i2s->TX_IN0_DNFIFO_STATUS >> 8)&0x1f;
}

unsigned int rdw_i2s_in0_dnfifo_rfilled_num(volatile COBRA_I2S_TypeDef *p_i2s)
{
	return (p_i2s->TX_IN0_DNFIFO_STATUS >> 16)&0x1f;
}


/*
 * 	 dnfifo1 overflow
	 dnfifo1 underflow
	 dnfifo1 full
	 dnfifo1 almostfull
	 dnfifo1 empty

 	 in1_dnfifo_filled_num={3'b0,in1_dnfifo_rfilled_num,3'b0,in1_dnfifo_wfilled_num}

#define STATE_IN1_UPFIFO_OVERFLOW		0x01
#define STATE_IN1_UPFIFO_UNDERFLOW		0x02
#define STATE_IN1_UPFIFO_FULL			0x04
#define STATE_IN1_UPFIFO_ALMOSTFULL		0x08
#define STATE_IN1_UPFIFO_EMPTY			0x10

 *
 */
unsigned int rdw_i2s_tx_in1_dnfifo_status(volatile COBRA_I2S_TypeDef *p_i2s)
{
	return p_i2s->TX_IN1_DNFIFO_STATUS;
}


/*
 *  in1_dnfifo_filled_num={3'b0,in1_dnfifo_rfilled_num,3'b0,in1_dnfifo_wfilled_num}
 *
 */
unsigned int rdw_i2s_in1_dnfifo_wfilled_num(volatile COBRA_I2S_TypeDef *p_i2s)
{
	return (p_i2s->TX_IN1_DNFIFO_STATUS >> 8)&0x1f;
}

unsigned int rdw_i2s_in1_dnfifo_rfilled_num(volatile COBRA_I2S_TypeDef *p_i2s)
{
	return (p_i2s->TX_IN1_DNFIFO_STATUS >> 16)&0x1f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * "0: tx_rx use seperated ws and sclk signal
	1: tx_rx use common ws and common sclk signal when tx and rx is master mode "
 *
 */
void cobra_i2s_cfg_txrx_common_mode_en(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_txrx_common_mode_en(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_txrx_common_mode_en(cobra_i2s1, en);
	}

}

/*
 * "0: tx and rx is idle
	1: tx and rx begin to work when tx and rx is master mode "
 *
 */
void cobra_i2s_cfg_txrx_common_mode_syn(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_txrx_common_mode_syn(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_txrx_common_mode_syn(cobra_i2s1, en);
	}

}

/*
 * 0: tx close      1:tx enable
 *
 */
void cobra_i2s_tx_ctrl_enable(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_enable(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_enable(cobra_i2s1, en);
	}
}

/*
 * 0: TX Module is slave
 * 1: TX Module is master
 *
 */
void cobra_i2s_tx_ctrl_master_en(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_master_en(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_master_en(cobra_i2s1, en);
	}

}


/*
 *  0: CPU wite dnfifo0  to I2S for transmitting
 *  1: Internal port write dnfifo1 to I2S for transmitting
 *
 */
void cobra_i2s_tx_ctrl_who_to_i2s(i2s_id index, I2S_WHO_TO_I2S source)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_who_to_i2s(cobra_i2s0, source);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_who_to_i2s(cobra_i2s1, source);
	}
}

/*
 * 	0: dnfifo0 close  (dnfifo0 for cpu)
 * 	1: dnfifo0 enable
 *
 */
void cobra_i2s_tx_ctrl_in0_dnfifo_enable(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_in0_dnfifo_enable(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_in0_dnfifo_enable(cobra_i2s1, en);
	}

}

/*
 *  0: dnfifo1 close  (dnfifo1 for internal port)
 *  1: dnfifo1 enable
 *
 */
void cobra_i2s_tx_ctrl_in1_dnfifo_enable(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_in1_dnfifo_enable(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_in1_dnfifo_enable(cobra_i2s1, en);
	}

}

/*
 * when special ,set initial WS , we should set it 1.
 * In general, we need not set it if we can follow the given timing.
 *
 */
void cobra_i2s_tx_ctrl_ws_init_vld(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_ws_init_vld(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_ws_init_vld(cobra_i2s1, en);
	}

}

/*
 *  0: initial WS is 0
 * 	1: initial WS is 1
 *
 */
void cobra_i2s_tx_ctrl_ws_init(i2s_id index,  unsigned int value)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_ws_init(cobra_i2s0, value);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_ws_init(cobra_i2s1, value);
	}

}

/*
 * when special ,set initial bit_delay, we should set it 1.
 * In general, we need not set it if we can follow the given timing.
 *
 */
void cobra_i2s_tx_ctrl_bit_delay_vld(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_bit_delay_vld(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_bit_delay_vld(cobra_i2s1, en);
	}

}

/*
 *   0: need not delay one cycle (ex. RJD)
     1: need delay one cycle (ex. I2S)
 *
 */
void cobra_i2s_tx_ctrl_bit_delay(i2s_id index, unsigned int value)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_bit_delay(cobra_i2s0, value);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_bit_delay(cobra_i2s1, value);
	}

}

/*
 *  when special ,we should set it 1.
 *  In general, we need not set it if we can follow the given timing.
 *
 */
void cobra_i2s_tx_ctrl_left_aligned_vld(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_left_aligned_vld(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_left_aligned_vld(cobra_i2s1, en);
	}

}

/*
 * "0: right aligned
	1: left aligned"
 *
 */
void cobra_i2s_tx_ctrl_left_aligned_en(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_left_aligned_en(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_left_aligned_en(cobra_i2s1, en);
	}

}

/*
 * "mode=PCM_SHORT_FRAME,PCM_LONG_FRAME,
	0: use default slot num
	1: use slot num that cpu config"
 *
 */
void cobra_i2s_tx_ctrl_tx_slot_num_vld(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ctrl_tx_slot_num_vld(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ctrl_tx_slot_num_vld(cobra_i2s1, en);
	}

}

/*
 * "4'd0:I2S ,
 *  4'd1:LJD,
 *  4'd2:RJD,
 *  4'd3:PCM_STERO
	4'd4:PCM_MONO,
	4'd5:PCM_SHORT_FRAME,
	4'd6:PCM_LONG_FRAME"
 *
 */
void cobra_i2s_cf_tx_mode(i2s_id index,  i2s_work_mode mode)
{
	if(index == I2S0)
	{
		rdw_i2s_cf_tx_mode(cobra_i2s0, mode);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cf_tx_mode(cobra_i2s1, mode);
	}

}

/*
 * it should be set in (>=4 && <=32);
 * or set in (>=34 && <=64)
 *
 */
void cobra_i2s_tx_biwidth(i2s_id index, unsigned int width)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_biwidth(cobra_i2s0, width);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_biwidth(cobra_i2s1, width);
	}

}

/*
 * when set ,the condition should be ok:
 * 1. >=4,
 * 2. cfg_tx_sampling_point_half_of_fs >=cfg_tx_bitwidth_of_word
 *
 */
void cobra_i2s_tx_sampling_point_half_of_fs(i2s_id index,  unsigned int sample_point)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_sampling_point_half_of_fs(cobra_i2s0, sample_point);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_sampling_point_half_of_fs(cobra_i2s1, sample_point);
	}

}

/*
 * it should be set >=2 when the  interface is set pcm long frame format
 *
 */
void cobra_i2s_tx_pcm_long_frame_holding(i2s_id index,  unsigned int holding)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_pcm_long_frame_holding(cobra_i2s0, holding);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_pcm_long_frame_holding(cobra_i2s1, holding);
	}

}

/*
 * 	mono:  1~14 , stero, 2~14
 *
 */
void cobra_i2s_cfg_tx_in0_dnfifo_almostfull_threshold(i2s_id index, unsigned int value)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_tx_in0_dnfifo_almostfull_threshold(cobra_i2s0, value);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_tx_in0_dnfifo_almostfull_threshold(cobra_i2s1, value);
	}

}

/*
 * range: 1~8. we advice it is 2
 *
 */
void cobra_i2s_cfg_tx_in1_dnfifo_almostfull_threshold(i2s_id index, unsigned int value)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_tx_in1_dnfifo_almostfull_threshold(cobra_i2s0, value);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_tx_in1_dnfifo_almostfull_threshold(cobra_i2s1, value);
	}

}

/*
 * "mode=PCM_SHORT_FRAME,PCM_LONG_FRAME,
	slot num , range:1 ~4. when set 0, it still 1."
 *
 */
void cobra_i2s_tx_slot_num(i2s_id index, unsigned int slot_number)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_slot_num(cobra_i2s0, slot_number);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_slot_num(cobra_i2s1, slot_number);
	}

}

/*
 * 0: rx close      1:rx enable
 *
 */
void cobra_i2s_cfg_rx_ctrl_rx_enable(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_ctrl_rx_enable(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_ctrl_rx_enable(cobra_i2s1, en);
	}

}

/*
 * 0: rx Module is slave
 * 1: rx Module is master
 *
 */
void cobra_i2s_cfg_rx_ctrl_rx_master(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_ctrl_rx_master(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_ctrl_rx_master(cobra_i2s1, en);
	}

}


/*
 *  0: rx data to cpu
 *  1: rx data to internal port
 *
 */
void cobra_i2s_cfg_rx_ctrl_i2s_to_who(i2s_id index, I2S_I2S_TO_WHO dest)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_ctrl_i2s_to_who(cobra_i2s0, dest);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_ctrl_i2s_to_who(cobra_i2s1, dest);
	}

}

/*
 * 0: upfifo0 close  (upfifo0 for cpu)
 * 1: upfifo0 enable
 *
 */
void cobra_i2s_cfg_rx_ctrl_out0_fifo_ena(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_ctrl_out0_fifo_ena(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_ctrl_out0_fifo_ena(cobra_i2s1, en);
	}

}


/*
 *  when special ,set initial WS , we should set it 1.
 *  In general, we need not set it if we can follow the given timing.
 *
 */
void cobra_i2s_cfg_rx_ctrl_ws_init_vld(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_ctrl_ws_init_vld(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_ctrl_ws_init_vld(cobra_i2s1, en);
	}

}

/*
 * "0: initial WS is 0
	1; initial WS is 1"
 *
 */
void cobra_i2s_cfg_rx_ctrl_ws_init(i2s_id index, unsigned int value)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_ctrl_ws_init(cobra_i2s0, value);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_ctrl_ws_init(cobra_i2s1, value);
	}

}

/*
 * when special ,set initial bit_delay, we should set it 1.
 * In general, we need not set it if we can follow the given timing.
 *
 */
void cobra_i2s_cfg_rx_ctrl_bit_delay_vld(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_ctrl_bit_delay_vld(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_ctrl_bit_delay_vld(cobra_i2s1, en);
	}

}


/*
 * "0: need not delay one cycle (ex. RJD)
	1: need delay one cycle (ex. I2S)"
 *
 */
void cobra_i2s_cfg_rx_ctrl_bit_delay(i2s_id index, unsigned int value)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_ctrl_bit_delay(cobra_i2s0, value);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_ctrl_bit_delay(cobra_i2s1, value);
	}

}

/*
 * "mode=PCM_SHORT_FRAME,PCM_LONG_FRAME,
	0: use default slot num
	1: use slot num that cpu config"
 *
 */
void cobra_i2s_cfg_rx_ctrl_rx_slot_num_vld(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_ctrl_rx_slot_num_vld(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_ctrl_rx_slot_num_vld(cobra_i2s1, en);
	}

}


/*
 * "4'd0:I2S ,
 *  4'd1:LJD,
 *  4'd2:RJD,
 *  4'd3:PCM_STERO
	4'd4:PCM_MONO,
	4'd5:PCM_SHORT_FRAME,
	4'd6:PCM_LONG_FRAME"
 *
 */
void cobra_i2s_cfg_rx_mode(i2s_id index, i2s_work_mode  mode)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_rx_mode(cobra_i2s0, mode);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_rx_mode(cobra_i2s1, mode);
	}

}

/*
 *  it should be set in (>=4 && <=32);
 *  or set in (>=34 && <=64)
 *  when mode=PCM_SHORT_FRAME,PCM_LONG_FRAME, cfg_rx_bitwidth_of_word is the  bits of one slot,
 *  other mode is the single channel bits.
 *
 */
void cobra_i2s_rx_bitwidth_of_word(i2s_id index, unsigned int width)
{
	if(index == I2S0)
	{
		rdw_i2s_rx_bitwidth_of_word(cobra_i2s0, width);
	}
	else if(index == I2S1)
	{
		rdw_i2s_rx_bitwidth_of_word(cobra_i2s1, width);
	}

}


/*
 * when set ,the condition should be ok:
 * 1. >=4,
 * 2.cfg_tx_sampling_point_half_of_fs >=cfg_tx_bitwidth_of_word;
 *   when mode=PCM_MONO,PCM_SHORT_FRAME,PCM_LONG_FRAME,cfg_rx_sampling_point_half_of_fs is the full of fs.
 *
 */
void cobra_i2s_rx_sampling_point_half_of_fs(i2s_id index, unsigned int sample_point)
{
	if(index == I2S0)
	{
		rdw_i2s_rx_sampling_point_half_of_fs(cobra_i2s0, sample_point);
	}
	else if(index == I2S1)
	{
		rdw_i2s_rx_sampling_point_half_of_fs(cobra_i2s1, sample_point);
	}

}


/*
 * it should be set >=2 when the  interface is set pcm long frame format
 *
 */
void cobra_i2s_rx_pcm_long_frame_holding(i2s_id index, unsigned int holding)
{
	if(index == I2S0)
	{
		rdw_i2s_rx_pcm_long_frame_holding(cobra_i2s0, holding);
	}
	else if(index == I2S1)
	{
		rdw_i2s_rx_pcm_long_frame_holding(cobra_i2s1, holding);
	}

}

/*
 * "mode=PCM_SHORT_FRAME,PCM_LONG_FRAME,
	slot num , range:1 ~4; when set 0, it still 1."
 *
 */
void cobra_i2s_rx_slot_num(i2s_id index, unsigned int slot_number)
{
	if(index == I2S0)
	{
		rdw_i2s_rx_slot_num(cobra_i2s0, slot_number);
	}
	else if(index == I2S1)
	{
		rdw_i2s_rx_slot_num(cobra_i2s1, slot_number);
	}
	
}

/*
 * "0: dma disable
	1: dma enable"
 *
 */
void cobra_i2s_cfg_dma_tx_enable(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_dma_tx_enable(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_dma_tx_enable(cobra_i2s1, en);
	}

}

/*
 * "0: when fifo overflow or underflow, dma still req
	1: when fifo overflow or underflow, dma no req "
 *
 */
void cobra_i2s_cfg_dma_tx_stop_by_overrun_interrupt(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_dma_tx_stop_by_overrun_interrupt(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_dma_tx_stop_by_overrun_interrupt(cobra_i2s1, en);
	}

}

/*
 * when software learn dma transfer has some error from dma arbitor,
 * software write dma_error , dma req can be disabled.
 *
 */
void cobra_i2s_cfg_dma_tx_error_signal(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_dma_tx_error_signal(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_dma_tx_error_signal(cobra_i2s1, en);
	}

}

/*
 * dma request by fifo's almostfull
 *
 */
void cobra_i2s_cfg_dma_tx_req_by_almostfull(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_dma_tx_req_by_almostfull(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_dma_tx_req_by_almostfull(cobra_i2s1, en);
	}

}


/*
 * "0: dma disable
	1: dma enable"
 *
 */
void cobra_i2s_cfg_dma_rx_enable(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_dma_rx_enable(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_dma_rx_enable(cobra_i2s1, en);
	}

}

/*
 * "0: when fifo overflow or underflow, dma still req
	1: when fifo overflow or underflow, dma no req "
 *
 */
void cobra_i2s_cfg_dma_rx_stop_by_overrun_interrupt(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_dma_rx_stop_by_overrun_interrupt(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_dma_rx_stop_by_overrun_interrupt(cobra_i2s1, en);
	}
}

/*
 * when software learn dma transfer has some error from dma arbitor, software write dma_error ,
 * dma req can be disabled.
 *
 */
void cobra_i2s_cfg_dma_rx_error_signal(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_dma_rx_error_signal(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_dma_rx_error_signal(cobra_i2s1, en);
	}

}


/*
 * dma request by fifo's almostfull
 *
 */
void cobra_i2s_cfg_dma_rx_req_by_almostfull(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_cfg_dma_rx_req_by_almostfull(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_cfg_dma_rx_req_by_almostfull(cobra_i2s1, en);
	}

}


/*
 * the threshold to give a signal of almostfull of infifo0
 *
 */
void cobra_i2s_in0_almostfull_threshold(i2s_id index,  unsigned int threshold)
{
	if(index == I2S0)
	{
		rdw_i2s_in0_almostfull_threshold(cobra_i2s0, threshold);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in0_almostfull_threshold(cobra_i2s1, threshold);
	}

}

/*
 * the threshold to give a signal of almostfull of upfifo0
 *
 */
void cobra_i2s_out0_almostfull_threshold(i2s_id index,  unsigned int threshold)
{
	if(index == I2S0)
	{
		rdw_i2s_out0_almostfull_threshold(cobra_i2s0, threshold);
	}
	else if(index == I2S1)
	{
		rdw_i2s_out0_almostfull_threshold(cobra_i2s1, threshold);
	}
}

/*
 * the data that cpu write to i2s
 *
 */
void cobra_i2s_in0_dnfifo_wen(i2s_id index, unsigned int value)
{
	if(index == I2S0)
	{
		rdw_i2s_in0_dnfifo_wen(cobra_i2s0, value);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in0_dnfifo_wen(cobra_i2s1, value);
	}

}

/*
 * the data that cpu read from i2s
 *
 */
unsigned int cobra_i2s_out0_upfifo_ren(i2s_id index)
{
	if(index == I2S0)
	{
		return rdw_i2s_out0_upfifo_ren(cobra_i2s0);
	}
	else if(index == I2S1)
	{
		return rdw_i2s_out0_upfifo_ren(cobra_i2s1);
	}
	return 0;
}


/*
 * upfifo0 full  interrupt mask
 *
 */
void cobra_i2s_out0_upfifo_full_interrupt_mask(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_out0_upfifo_full_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_out0_upfifo_full_interrupt_mask(cobra_i2s1, en);
	}
}

/*
 * upfifo0 underflow interrupt mask
 *
 */
void cobra_i2s_out0_upfifo_underflow_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_out0_upfifo_underflow_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_out0_upfifo_underflow_interrupt_mask(cobra_i2s1, en);
	}

}


/*
 * upfifo0 overflow interrupt mask
 *
 */
void cobra_i2s_out0_upfifo_overflow_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_out0_upfifo_overflow_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_out0_upfifo_overflow_interrupt_mask(cobra_i2s1, en);
	}
}

/*
 * upfifo0 empty interrupt mask
 *
 */
void cobra_i2s_out0_upfifo_empty_interrupt_mask(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_out0_upfifo_empty_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_out0_upfifo_empty_interrupt_mask(cobra_i2s1, en);
	}
}


/*
 * upfifo0 almost_full interrupt mask
 *
 */
void cobra_i2s_out0_upfifo_almostfull_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_out0_upfifo_almostfull_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_out0_upfifo_almostfull_interrupt_mask(cobra_i2s1, en);
	}
}


/*
 * dnfifo0 full  interrupt mask
 *
 */
void cobra_i2s_in0_dnfifo_full_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in0_dnfifo_full_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in0_dnfifo_full_interrupt_mask(cobra_i2s1, en);
	}

}

/*
 * dnfifo0 underflow interrupt mask
 *
 */
void cobra_i2s_in0_dnfifo_underflow_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in0_dnfifo_underflow_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in0_dnfifo_underflow_interrupt_mask(cobra_i2s1, en);
	}
}


/*
 * dnfifo0 overflow interrupt mask
 *
 */
void cobra_i2s_in0_dnfifo_overflow_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in0_dnfifo_overflow_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in0_dnfifo_overflow_interrupt_mask(cobra_i2s1, en);
	}

}

/*
 *  dnfifo0 empty interrupt mask
 *
 */
void cobra_i2s_in0_dnfifo_empty_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in0_dnfifo_empty_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in0_dnfifo_empty_interrupt_mask(cobra_i2s1, en);
	}
}


/*
 * dnfifo0 almost_full interrupt mask
 *
 */
void cobra_i2s_in0_dnfifo_almostfull_interrupt_mask(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in0_dnfifo_almostfull_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in0_dnfifo_almostfull_interrupt_mask(cobra_i2s1, en);
	}
}


/*
 *  dnfifo1 full interrupt mask
 *
 */
void cobra_i2s_in1_dnfifo_full_interrupt_mask(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in1_dnfifo_full_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in1_dnfifo_full_interrupt_mask(cobra_i2s1, en);
	}
}


/*
 * dnfifo1 underflow interrupt mask
 *
 */
void cobra_i2s_in1_dnfifo_underflow_interrupt_mask(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in1_dnfifo_underflow_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in1_dnfifo_underflow_interrupt_mask(cobra_i2s1, en);
	}
}

/*
 * dnfifo1 overflow interrupt mask
 *
 */
void cobra_i2s_in1_dnfifo_overflow_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in1_dnfifo_overflow_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in1_dnfifo_overflow_interrupt_mask(cobra_i2s1, en);
	}

}


/*
 * dnfifo1 empty interrupt mask
 *
 */
void cobra_i2s_in1_dnfifo_empty_interrupt_mask(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in1_dnfifo_empty_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in1_dnfifo_empty_interrupt_mask(cobra_i2s1, en);
	}
}

/*
 * dnfifo1 almost_full interrupt mask
 *
 */
void cobra_i2s_in1_dnfifo_almostfull_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in1_dnfifo_almostfull_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in1_dnfifo_almostfull_interrupt_mask(cobra_i2s1, en);
	}

}


/*
 * upfifo0 interrupt main mask
 *
 */
void cobra_i2s_out0_upfifo_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_out0_upfifo_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_out0_upfifo_interrupt_mask(cobra_i2s1, en);
	}

}

/*
 * dnfifo0 interrupt main mask
 *
 */
void cobra_i2s_in0_dnfifo_interrupt_mask(i2s_id index,  unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in0_dnfifo_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in0_dnfifo_interrupt_mask(cobra_i2s1, en);
	}
}

/*
 * dnfifo1 interrupt main mask
 *
 */
void cobra_i2s_in1_dnfifo_interrupt_mask(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_in1_dnfifo_interrupt_mask(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_in1_dnfifo_interrupt_mask(cobra_i2s1, en);
	}

}

/*
 * rx ws signal swap to tx
 *
 */
void cobra_i2s_tx_ws_to_rx(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_tx_ws_to_rx(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_tx_ws_to_rx(cobra_i2s1, en);
	}

}


/*
 * tx ws signal swap to rx
 *
 */
void cobra_i2s_rx_ws_to_tx(i2s_id index, unsigned int en)
{
	if(index == I2S0)
	{
		rdw_i2s_rx_ws_to_tx(cobra_i2s0, en);
	}
	else if(index == I2S1)
	{
		rdw_i2s_rx_ws_to_tx(cobra_i2s1, en);
	}

}


/*
 * 	upfifo0 overflow
	upfifo0 underflow
	upfifo0 full
	upfifo0 almostfull
	upfifo0 empty

	upfifo0 filled num
#define STATE_OUT0_UPFIFO_OVERFLOW			0x01
#define STATE_OUT0_UPFIFO_UNDERFLOW			0x02
#define STATE_OUT0_UPFIFO_FULL				0x04
#define STATE_OUT0_UPFIFO_ALMOSTFULL		0x08
#define STATE_OUT0_UPFIFO_EMPTY				0x10

 *
 */
unsigned int cobra_i2s_rx_out0_upfifo_statu(i2s_id index)
{
	if(index == I2S0)
	{
		return rdw_i2s_rx_out0_upfifo_statu(cobra_i2s0);
	}
	else if(index == I2S1)
	{
		return rdw_i2s_rx_out0_upfifo_statu(cobra_i2s1);
	}

	return 0;
}

/*
 * upfifo0 filled num
 *
 */
unsigned int cobra_i2s_out0_upfifo_filled_num(i2s_id index)
{
	if(index == I2S0)
	{
		return rdw_i2s_out0_upfifo_filled_num(cobra_i2s0);
	}
	else if(index == I2S1)
	{
		return rdw_i2s_out0_upfifo_filled_num(cobra_i2s1);
	}
	return 0;
}

/*
 * 	 dnfifo0 overflow
	 dnfifo0 underflow
	 dnfifo0 full
	 dnfifo0 almostfull
	 dnfifo0 empty

 	 in0_dnfifo_filled_num={3'b0,in0_dnfifo_rfilled_num,3'b0,in0_dnfifo_wfilled_num}

#define STATE_IN0_UPFIFO_OVERFLOW		0x01
#define STATE_IN0_UPFIFO_UNDERFLOW		0x02
#define STATE_IN0_UPFIFO_FULL			0x04
#define STATE_IN0_UPFIFO_ALMOSTFULL		0x08
#define STATE_IN0_UPFIFO_EMPTY			0x10

 *
 */
unsigned int cobra_i2s_tx_in0_dnfifo_status(i2s_id index)
{
	if(index == I2S0)
	{
		return rdw_i2s_tx_in0_dnfifo_status(cobra_i2s0);
	}
	else if(index == I2S1)
	{
		return rdw_i2s_tx_in0_dnfifo_status(cobra_i2s1);
	}
	return 0;
}


/*
 *  in0_dnfifo_filled_num={3'b0,in0_dnfifo_rfilled_num,3'b0,in0_dnfifo_wfilled_num}
 *
 */
unsigned int cobra_i2s_in0_dnfifo_wfilled_num(i2s_id index)
{
	if(index == I2S0)
	{
		return rdw_i2s_in0_dnfifo_wfilled_num(cobra_i2s0);
	}
	else if(index == I2S1)
	{
		return rdw_i2s_in0_dnfifo_wfilled_num(cobra_i2s1);
	}
	return 0;
}

unsigned int cobra_i2s_in0_dnfifo_rfilled_num(i2s_id index)
{
	if(index == I2S0)
	{
		return rdw_i2s_in0_dnfifo_rfilled_num(cobra_i2s0);
	}
	else if(index == I2S1)
	{
		return rdw_i2s_in0_dnfifo_rfilled_num(cobra_i2s1);
	}
	return 0;
}

/*
 * 	 dnfifo1 overflow
	 dnfifo1 underflow
	 dnfifo1 full
	 dnfifo1 almostfull
	 dnfifo1 empty

 	 in1_dnfifo_filled_num={3'b0,in1_dnfifo_rfilled_num,3'b0,in1_dnfifo_wfilled_num}

#define STATE_IN1_UPFIFO_OVERFLOW		0x01
#define STATE_IN1_UPFIFO_UNDERFLOW		0x02
#define STATE_IN1_UPFIFO_FULL			0x04
#define STATE_IN1_UPFIFO_ALMOSTFULL		0x08
#define STATE_IN1_UPFIFO_EMPTY			0x10

 *
 */
unsigned int cobra_i2s_tx_in1_dnfifo_status(i2s_id index)
{
	if(index == I2S0)
	{
		return rdw_i2s_tx_in1_dnfifo_status(cobra_i2s0);
	}
	else if(index == I2S1)
	{
		return rdw_i2s_tx_in1_dnfifo_status(cobra_i2s1);
	}
	return 0;
}

/*
 *  in1_dnfifo_filled_num={3'b0,in1_dnfifo_rfilled_num,3'b0,in1_dnfifo_wfilled_num}
 *
 */
unsigned int cobra_i2s_in1_dnfifo_wfilled_num(i2s_id index)
{
	if(index == I2S0)
	{
		return rdw_i2s_in1_dnfifo_wfilled_num(cobra_i2s0);
	}
	else if(index == I2S1)
	{
		return rdw_i2s_in1_dnfifo_wfilled_num(cobra_i2s1);
	}
	return 0;
}

unsigned int cobra_i2s_in1_dnfifo_rfilled_num(i2s_id index)
{
	if(index == I2S0)
	{
		return rdw_i2s_in1_dnfifo_rfilled_num(cobra_i2s0);
	}
	else if(index == I2S1)
	{
		return rdw_i2s_in1_dnfifo_rfilled_num(cobra_i2s1);
	}
	return 0;
}

void * cobra_i2s_get_m3_i2s_read_address(i2s_id i2s_index)
{
		if(i2s_index == I2S0)
		{
				return (void *)(&(cobra_i2s0->OUT0_UPFIFO_READ_DATA));
		}
		else if(i2s_index == I2S1)
		{
				return (void *)(&(cobra_i2s1->OUT0_UPFIFO_READ_DATA));
		}
		return NULL;
}


void * cobra_i2s_get_m3_i2s_write_address(i2s_id i2s_index)
{
		if(i2s_index == I2S0)
		{
				return (void *)(&(cobra_i2s0->IN0_DNFIFO_WRITE_DATA));
		}
		else if(i2s_index == I2S1)
		{
				return (void *)(&(cobra_i2s1->IN0_DNFIFO_WRITE_DATA));
		}
		return NULL;
}



//-----------------------------------------------------------------------------------------
void cobra_i2s_set_rx_mode(i2s_id index, i2s_work_mode mode)
{

	cobra_i2s_cfg_rx_mode(index, mode);

}

void cobra_i2s_set_rx_sampling_point(i2s_id index, sample_point point)
{
	unsigned int sampling_point;

	switch(point)
	{
		case SAMPLE_16BIT:
			sampling_point = 16;
			break;		
		case SAMPLE_32BIT:
			sampling_point = 32;
			break;
		case SAMPLE_64BIT:
			sampling_point = 64;
			break;
		case SAMPLE_128BIT:
			sampling_point = 128;
			break;
		default:
			sampling_point = 64;
			break;
	}

	cobra_i2s_rx_sampling_point_half_of_fs(index, sampling_point);


}

void cobra_i2s_set_rx_bitwidth(i2s_id index, sample_word_width width)
{
	unsigned char value;

	switch(width)
	{
		case SAMPLE_WORD_8BIT:
			value = 8;
			break;
		case SAMPLE_WORD_16BIT:
			value = 16;
			break;
		case SAMPLE_WORD_24BIT:
			value = 24;
			break;
		case SAMPLE_WORD_32BIT:
			value = 32;
			break;
		case SAMPLE_WORD_64BIT:
			value = 64;
			break;
		default:
			value = 16;
	}

	cobra_i2s_rx_bitwidth_of_word(index, value);

}



void cobra_i2s_rx_enable(i2s_id index, bool isMaster)
{
	if(isMaster)
	{
		cobra_i2s_cfg_rx_ctrl_rx_master(index, 1);
		cobra_i2s_cfg_rx_ctrl_rx_enable(index, 1);
		cobra_i2s_cfg_rx_ctrl_out0_fifo_ena(index, 1);
	}
	else
	{
		cobra_i2s_cfg_rx_ctrl_rx_master(index, 0);
		cobra_i2s_cfg_rx_ctrl_rx_enable(index, 1);
		cobra_i2s_cfg_rx_ctrl_out0_fifo_ena(index, 1);
	}

}

void cobra_i2s_rx_disable(i2s_id index , bool isMaster)
{

	if(isMaster)
	{
		cobra_i2s_cfg_rx_ctrl_rx_enable(index, 0);
		cobra_i2s_cfg_rx_ctrl_out0_fifo_ena(index, 0);
	}
	else
	{
		cobra_i2s_cfg_rx_ctrl_rx_enable(index, 0);
		cobra_i2s_cfg_rx_ctrl_out0_fifo_ena(index, 0);
	}

}

uint32_t cobra_rx_out0_upfifo_statu(i2s_id index)
{
	return cobra_i2s_rx_out0_upfifo_statu(index);

}

void cobra_i2s_set_tx_mode(i2s_id index, i2s_work_mode mode)
{
	cobra_i2s_cf_tx_mode(index, mode);

}

void cobra_i2s_set_tx_sampling_point(i2s_id index, sample_point point)
{
	unsigned int sampling_point;

//	if((index != I2S0) && (index != I2S1))  return ;

	switch(point)
	{
		case SAMPLE_16BIT:
			sampling_point = 16;
			break;
		
		case SAMPLE_32BIT:
			sampling_point = 32;
			break;
		case SAMPLE_64BIT:
			sampling_point = 64;
			break;
		case SAMPLE_128BIT:
			sampling_point = 128;
			break;
		default:
			sampling_point = 64;
			break;
	}

	cobra_i2s_tx_sampling_point_half_of_fs(index, sampling_point);

}

void cobra_i2s_set_tx_bitwidth(i2s_id index, sample_word_width width)
{
	unsigned char value;
//	if((index != I2S0) && (index != I2S1))  return;

	switch(width)
	{
		case SAMPLE_WORD_8BIT:
			value = 8;
			break;
		case SAMPLE_WORD_16BIT:
			value = 16;
			break;
		case SAMPLE_WORD_24BIT:
			value = 24;
			break;
		case SAMPLE_WORD_32BIT:
			value = 32;
			break;
		case SAMPLE_WORD_64BIT:
			value = 64;
			break;
		default:
			value = 16;
	}

	cobra_i2s_tx_biwidth(index, value);

}

void cobra_i2s_tx_enable(i2s_id index, bool isMaster)
{

	if(isMaster)
	{
		cobra_i2s_tx_ctrl_master_en(index, 1);
		cobra_i2s_tx_ctrl_enable(index, 1);
		cobra_i2s_tx_ctrl_in0_dnfifo_enable(index, 1);
	}
	else
	{
		cobra_i2s_tx_ctrl_master_en(index, 0);
		cobra_i2s_tx_ctrl_enable(index, 1);
		cobra_i2s_tx_ctrl_in0_dnfifo_enable(index, 1);
	}

}

void cobra_i2s_tx_disable(i2s_id index, bool isMaster)
{

	if(isMaster)
	{
		cobra_i2s_tx_ctrl_master_en(index, 0);
		cobra_i2s_tx_ctrl_enable(index, 0);
		cobra_i2s_tx_ctrl_in0_dnfifo_enable(index, 0);
	}
	else
	{
		cobra_i2s_tx_ctrl_master_en(index, 0);
		cobra_i2s_tx_ctrl_enable(index, 0);
		cobra_i2s_tx_ctrl_in0_dnfifo_enable(index, 0);
	}


}


void cobra_i2s_src_tx_enable(i2s_id index, bool isMaster)
{
	if(isMaster)
	{
		cobra_i2s_tx_ctrl_master_en(index, 1);
		cobra_i2s_tx_ctrl_enable(index, 1);
		cobra_i2s_tx_ctrl_in1_dnfifo_enable(index, 1);
		cobra_i2s_tx_ctrl_who_to_i2s(index, INTERNAL_WRITE_DNFIFO1);
	}
	else
	{
		cobra_i2s_tx_ctrl_master_en(index, 0);
		cobra_i2s_tx_ctrl_enable(index, 1);
		cobra_i2s_tx_ctrl_in1_dnfifo_enable(index, 1);
		cobra_i2s_tx_ctrl_who_to_i2s(index, INTERNAL_WRITE_DNFIFO1);
	}

}





void cobra_set_tx_in_dnfifo_almostfull_threshold(i2s_id index, uint8_t infifo_type,uint8_t threshold)
{
	if(infifo_type == 0)
	{
		cobra_i2s_cfg_tx_in0_dnfifo_almostfull_threshold(index, threshold);
	}
	else
	{
		cobra_i2s_cfg_tx_in1_dnfifo_almostfull_threshold(index, threshold);
	}
	
}

uint32_t cobra_get_tx_in_dnfifo_status(i2s_id index, uint8_t infifo_type)
{

	if(infifo_type == 0)
	{
		return cobra_i2s_tx_in0_dnfifo_status(index);
	}
	else
	{
		return cobra_i2s_tx_in1_dnfifo_status(index);
	}
}

void cobra_i2s_send_data(i2s_id index, uint32_t data)
{
	while(cobra_get_tx_in_dnfifo_status(index, 0)&STATE_IN0_UPFIFO_FULL)
	{
		cobra_i2s_in0_dnfifo_wen(index, data);
	}

}





void cobra_i2s_invert_lrclk(i2s_id index, uint8_t inv)
{

	if(inv)
	{
		cobra_i2s_cfg_rx_ctrl_ws_init_vld(index, 1);
		cobra_i2s_cfg_rx_ctrl_ws_init(index, 0);
		
		cobra_i2s_tx_ctrl_ws_init_vld(index, 1);
		cobra_i2s_tx_ctrl_ws_init(index, 0);
	}
	else
	{
		cobra_i2s_cfg_rx_ctrl_ws_init_vld(index, 0);
		cobra_i2s_cfg_rx_ctrl_ws_init(index, 0);
		
		cobra_i2s_tx_ctrl_ws_init_vld(index, 0);
		cobra_i2s_tx_ctrl_ws_init(index, 0);
		
	}

}


void app_cobra_cfg_txrx_common_mode_en(i2s_id index, bool en)
{
	if(en)
	{
		cobra_i2s_cfg_txrx_common_mode_en(index, 1);
	}
	else
	{
		cobra_i2s_cfg_txrx_common_mode_en(index, 0);
		cobra_i2s_cfg_txrx_common_mode_syn(index, 0);
	}

}

void cobra_i2s_begin_work(i2s_id index)
{
	cobra_i2s_cfg_txrx_common_mode_en(index, 1);
	cobra_i2s_cfg_txrx_common_mode_syn(index, 1);

}

void cobra_i2s_rx_init(i2s_id index, i2s_work_mode mode,sample_word_width width, sample_point point,bool dma_en)
{
//	if((index != I2S0) && (index != I2S1))  return ;
	cobra_i2s_set_rx_mode(index, mode);
	cobra_i2s_set_rx_bitwidth(index, width);
	cobra_i2s_set_rx_sampling_point(index, point);
	app_cobra_cfg_txrx_common_mode_en(index, 1);
	if(dma_en)
	{
		cobra_i2s_cfg_dma_rx_req_by_almostfull(index, 0);
		cobra_i2s_cfg_dma_rx_enable(index, 1);
	}
	else
	{
		cobra_i2s_cfg_dma_rx_enable(index, 0);
	}

}

void cobra_i2s_tx_init(i2s_id index, i2s_work_mode mode, sample_word_width width, sample_point point, bool dma_en)
{
//	if((index != I2S0) && (index != I2S1))  return ;

	cobra_i2s_set_tx_mode(index, mode);
	cobra_i2s_set_tx_bitwidth(index, width);
	cobra_i2s_set_tx_sampling_point(index, point);

	if(dma_en)
	{
		cobra_i2s_cfg_dma_tx_enable(index, 1);
		cobra_i2s_cfg_dma_tx_stop_by_overrun_interrupt(index, 1);
		cobra_i2s_cfg_dma_tx_req_by_almostfull(index, 1);
	}
	else
	{
		cobra_i2s_cfg_dma_tx_enable(index, 0);
		cobra_i2s_cfg_dma_tx_stop_by_overrun_interrupt(index, 0);
		cobra_i2s_cfg_dma_tx_req_by_almostfull(index, 0);

	}

}

static uint32_t cobra_i2s_greatest_common_divisor(uint32_t first_num, uint32_t second_num)
{
		uint32_t temp;
		uint32_t a;
		uint32_t b;
	
		a = first_num;
		b = second_num;
		while(a%b != 0)
		{
				temp =  a % b;
				a = b;
				b = temp;
		}
		return b;
}


//i2s_clk_mode_set(0,16,16000,16,2,32,I2S_TX,TX_MODULE_MASTER);
//void app_cobra_i2s_set_clock_mode(I2S_INDEX i2s_index, double sample_clk_kHz, uint32_t clk_in_Khz, \
//			uint8_t bit_num, uint8_t m_s_Div, uint16_t sample_ponit, uint8_t i2s_iSTx, bool isMaster)
//				
void app_cobra_i2s_set_clock_mode(struct i2s_env_tag  * i2s_env)
{
    double	mclk_kHz,sclk_kHz;
	
    uint16_t x_Div,y_Div;
    
		int8_t i;
    
		sclk_kHz = i2s_env->sample_rate*i2s_env->bit_num*2*(i2s_env->sample_ponit/i2s_env->bit_num);
    
		mclk_kHz =  sclk_kHz * i2s_env->m_s_div;
    
		x_Div = mclk_kHz*2/cobra_i2s_greatest_common_divisor(i2s_env->clk_in_khz, mclk_kHz*2);
    
		y_Div = i2s_env->clk_in_khz/cobra_i2s_greatest_common_divisor(i2s_env->clk_in_khz, mclk_kHz*2);
    
		for(i = 2; y_Div>0xFF||x_Div>0xFF; i--)
    {
        x_Div = pow(10,i);
        y_Div =(uint16_t) (i2s_env->clk_in_khz*pow(10,i)/(mclk_kHz*2));
    }
		
		
#if 1
    printf("sclk_kHz %lf,mclk_kHz %lf\n", sclk_kHz, mclk_kHz);
    printf("x_Div %d,y_Div %d\n", x_Div, y_Div);
#endif
		
    if(i2s_env->index == I2S0) 
		{
        if(i2s_env->sound_mode == I2S_PLAY) 
				{
            if(i2s_env->i2s_is_master) 
						{
								cobra_pcrm_i2s0_transmitter_mode(MASTER_MODE_PCLK_WITH_MCLK_OUTPUT);
							
            } 
						else 
						{
								cobra_pcrm_i2s0_transmitter_mode(TYPICAL_SLAVE_MODE_WITH_MCLK);
            }
            cobra_pcrm_i2s0_tx_mode_mclk_div(x_Div,y_Div);
            cobra_pcrm_i2s0_tx_mode_sclk_div(i2s_env->m_s_div - 1);
        } 
				else if(i2s_env->sound_mode == I2S_RECORD) 
				{
            if(i2s_env->i2s_is_master) 
						{
								cobra_pcrm_i2s0_receiver_mode(RECEIVER_MODE_PCLK_WITH_MCLK_OUTPUT);
            } 
						else 
						{
								cobra_pcrm_i2s0_receiver_mode(RECEIVER_TYPICAL_SLAVE_MODE_WITH_MCLK);
            }
            cobra_pcrm_i2s0_rx_mode_mclk_div(x_Div,y_Div);
            cobra_pcrm_i2s0_rx_mode_sclk_div(i2s_env->m_s_div - 1);
        }
        cobra_pcrm_i2s0_clk_en(1);

    } 
		else if(i2s_env->index == I2S1) 
		{
        if(i2s_env->sound_mode == I2S_PLAY) 
				{
            if(i2s_env->i2s_is_master) 
						{
								cobra_pcrm_i2s1_transmitter_mode(MASTER_MODE_PCLK_WITH_MCLK_OUTPUT);
								cobra_pcrm_i2s1_tx_mode_sclk_div(i2s_env->m_s_div - 1);
            } 
						else 
						{
								cobra_pcrm_i2s1_transmitter_mode(TYPICAL_SLAVE_MODE_WITH_MCLK);
            }
            cobra_pcrm_i2s1_tx_mode_mclk_div(x_Div,y_Div);
        } 
				else if(i2s_env->sound_mode == I2S_RECORD) 
				{
            if(i2s_env->i2s_is_master) 
						{
								cobra_pcrm_i2s1_receiver_mode(RECEIVER_MODE_PCLK_WITH_MCLK_OUTPUT);
								cobra_pcrm_i2s1_rx_mode_sclk_div(i2s_env->m_s_div - 1);
            } 
						else 
						{
								cobra_pcrm_i2s1_receiver_mode(RECEIVER_TYPICAL_SLAVE_MODE_WITH_MCLK);
            }
            cobra_pcrm_i2s1_rx_mode_mclk_div(x_Div,y_Div);
        }
        cobra_pcrm_i2s1_clk_en(1);
    }
		
}

void app_cobra_i2s_pll_mode(i2s_id index, uint8_t m_s_Div, I2S_SOUND_MODE mode)
{
    if(index == I2S0) 
		{
        if(mode == I2S_PLAY) 
				{
            cobra_pcrm_i2s0_transmitter_mode(MASTER_MODE_FORM_APLL);
            cobra_pcrm_i2s0_tx_mode_sclk_div(m_s_Div - 1);
        }
        else if(mode == I2S_RECORD)
				{
            cobra_pcrm_i2s0_receiver_mode(RECEIVER_MASTER_MODE_FORM_APLL);
            cobra_pcrm_i2s0_rx_mode_sclk_div(m_s_Div - 1);       
        }
        cobra_pcrm_i2s0_clk_en(1);
    }
    else 
		{

        if(mode == I2S_PLAY)  
				{
            cobra_pcrm_i2s1_transmitter_mode(MASTER_MODE_FORM_APLL);
            cobra_pcrm_i2s1_tx_mode_sclk_div(m_s_Div - 1);
        }
        else if(mode == I2S_RECORD)
				{
            cobra_pcrm_i2s1_receiver_mode(RECEIVER_MASTER_MODE_FORM_APLL);
            cobra_pcrm_i2s0_rx_mode_sclk_div(m_s_Div - 1);      
        }
        cobra_pcrm_i2s1_clk_en(1);
    }
}

void app_cobra_i2s_clk_config(i2s_id index, CLOCK_SOURCE clk_source, double sample_clk_kHz, sample_word_width s_width, sample_point point)
{
    uint16_t pll_sclk_div = 0;
		unsigned int width;
		unsigned int point_value;
    double mclk_kHz = 0;   
		switch(s_width)
		{
			case SAMPLE_WORD_8BIT:
				width = 8;
				break;
			case SAMPLE_WORD_16BIT:
				width = 16;
				break;
			case SAMPLE_WORD_24BIT:
				width = 24;
				break;
			case SAMPLE_WORD_32BIT:
				width = 32;
				break;
			case SAMPLE_WORD_64BIT:
				width = 64;
				break;
			default:
				width = 16;
		}
		
	
		switch(point)
		{
				case SAMPLE_16BIT:
				point_value = 16;
				break;
			
			case SAMPLE_32BIT:
				point_value = 32;
				break;
			case SAMPLE_64BIT:
				point_value = 64;
				break;
			case SAMPLE_128BIT:
				point_value = 128;
				break;
			default:
				point_value = 64;
				break;
		}

	
    if(clk_source == CLK_PLL)
    {
				cobra4_aon_wk_protection_en(1);
				cobra4_aon_wk_d2a_ldoanaforpll_en(1);
				cobra4_aon_wk_protection_en(0);
        if(sample_clk_kHz == 22.05 || sample_clk_kHz == 44.1 || sample_clk_kHz == 88.2 || sample_clk_kHz == 176.4)
        {
            cobra4_pll_set_frenquence(180.6336);
            mclk_kHz = 180.6336*1000/8;
            pll_sclk_div = mclk_kHz/(sample_clk_kHz*width*2*(point_value/width));        
        }
        else
        {
            cobra4_pll_set_frenquence(196.608);
            mclk_kHz = 196.608*1000/8;
            pll_sclk_div = 24576/(sample_clk_kHz*width*2*(point_value/width));
        }
				printf("pll_sclk_div = %d \n", pll_sclk_div);
        app_cobra_i2s_pll_mode(index, pll_sclk_div, I2S_PLAY);
        app_cobra_i2s_pll_mode(index, pll_sclk_div, I2S_RECORD);
    }
    else if(clk_source == CLK_48MHZ)
    {
				struct i2s_env_tag i2s_env;
			
        mclk_kHz = 12288;
        pll_sclk_div = mclk_kHz/(sample_clk_kHz*width*2*(point_value/width));
			
				i2s_env.i2s_is_master = true;
				i2s_env.sample_rate = sample_clk_kHz;     //16k
				i2s_env.bit_num = width;         //16bit, 8bit .....
				i2s_env.index = index;
				i2s_env.clk_in_khz = 48000;    //16M
				i2s_env.m_s_div = pll_sclk_div;
				i2s_env.sound_mode = I2S_RECORD;
				i2s_env.sample_ponit = point_value;
				app_cobra_i2s_set_clock_mode(&i2s_env);
			
				
				i2s_env.i2s_is_master = true;
				i2s_env.sample_rate = sample_clk_kHz;     //16k
				i2s_env.bit_num = width;         //16bit, 8bit .....
				i2s_env.index = index;
				i2s_env.clk_in_khz = 48000;    //16M
				i2s_env.m_s_div = pll_sclk_div;
				i2s_env.sound_mode = I2S_PLAY;
				i2s_env.sample_ponit = point_value;
				app_cobra_i2s_set_clock_mode(&i2s_env);
				
   //     init_sys_i2s(0,sample_clk_kHz,48000,width,pll_sclk_div,sampling_point,I2S_RX,RX_MODULE_MASTER);
   //     init_sys_i2s(0,sample_clk_kHz,48000,width,pll_sclk_div,sampling_point,I2S_TX,TX_MODULE_MASTER);
    }
}

#ifdef COBRA_I2S_NEW
uint32_t i2s_greatest_common_divisor(uint32_t first_num,uint32_t second_num)
{
	uint32_t temp,a,b;
	a = first_num;
	b = second_num;
	while(a%b != 0){
		temp =  a % b;
		a = b;
		b = temp;
	}
	return b;
}

void i2s_clk_mode_set(uint8_t i2s_type,double sample_clk_kHz,uint32_t clk_in_Khz,uint8_t bit_num,uint8_t m_s_Div,uint16_t sample_ponit,uint8_t i2s_iSTx,bool isMaster)
{
    double	mclk_kHz,sclk_kHz;
    uint16_t x_Div,y_Div;
    int8_t i;
    sclk_kHz = sample_clk_kHz*bit_num*2*(sample_ponit/bit_num);
    mclk_kHz =  sclk_kHz * m_s_Div;
    x_Div = mclk_kHz*2/i2s_greatest_common_divisor(clk_in_Khz,mclk_kHz*2);
    y_Div = clk_in_Khz/i2s_greatest_common_divisor(clk_in_Khz,mclk_kHz*2);
    for(i = 2; y_Div>0xFF||x_Div>0xFF; i--)
    {
        x_Div = pow(10,i);
        y_Div =(uint16_t) (clk_in_Khz*pow(10,i)/(mclk_kHz*2));
    }
#ifdef PRINT_REG
    printf("sclk_kHz %lf,mclk_kHz %lf\n",sclk_kHz,mclk_kHz);
    printf("x_Div %d,y_Div %d,m_s_Div %d\n",x_Div,y_Div,m_s_Div);
#endif
    if(i2s_type == 0) {
        if(i2s_iSTx == I2S_TX) {
            if(isMaster) {
								cobra_pcrm_i2s0_transmitter_mode(MASTER_MODE_PCLK_WITH_MCLK_OUTPUT);
							
            } else {
								cobra_pcrm_i2s0_transmitter_mode(TYPICAL_SLAVE_MODE_WITH_MCLK);
            }
            cobra_pcrm_i2s0_tx_mode_mclk_div(x_Div,y_Div);
            cobra_pcrm_i2s0_tx_mode_sclk_div(m_s_Div - 1);
        } else {
            if(isMaster) {
								cobra_pcrm_i2s0_receiver_mode(RECEIVER_MODE_PCLK_WITH_MCLK_OUTPUT);
            } else {
								cobra_pcrm_i2s0_receiver_mode(RECEIVER_TYPICAL_SLAVE_MODE_WITH_MCLK);
            }
            cobra_pcrm_i2s0_rx_mode_mclk_div(x_Div,y_Div);
            cobra_pcrm_i2s0_rx_mode_sclk_div(m_s_Div - 1);
        }
        cobra_pcrm_i2s0_clk_en(1);

    } else {
        if(i2s_iSTx == I2S_TX) {
            if(isMaster) {
								cobra_pcrm_i2s1_transmitter_mode(MASTER_MODE_PCLK_WITH_MCLK_OUTPUT);
								cobra_pcrm_i2s1_tx_mode_sclk_div(m_s_Div - 1);
            } else {
								cobra_pcrm_i2s1_transmitter_mode(TYPICAL_SLAVE_MODE_WITH_MCLK);
            }
            cobra_pcrm_i2s1_tx_mode_mclk_div(x_Div,y_Div);
        } else {
            if(isMaster) {
								cobra_pcrm_i2s1_receiver_mode(RECEIVER_MODE_PCLK_WITH_MCLK_OUTPUT);
								cobra_pcrm_i2s1_rx_mode_sclk_div(m_s_Div - 1);
            } else {
								cobra_pcrm_i2s1_receiver_mode(RECEIVER_TYPICAL_SLAVE_MODE_WITH_MCLK);
            }
            cobra_pcrm_i2s1_rx_mode_mclk_div(x_Div,y_Div);
        }
        cobra_pcrm_i2s1_clk_en(1);
    }
}

void i2s_pll_mode_set(char i2s_type,uint8_t m_s_Div,uint8_t i2s_iSTx)
{
    if(i2s_type == 0) {
        if(i2s_iSTx == I2S_TX) {
            cobra_pcrm_i2s0_transmitter_mode(MASTER_MODE_FORM_APLL);
            cobra_pcrm_i2s0_tx_mode_sclk_div(m_s_Div - 1);
        }
        else{
            cobra_pcrm_i2s0_receiver_mode(RECEIVER_MASTER_MODE_FORM_APLL);
            cobra_pcrm_i2s0_rx_mode_sclk_div(m_s_Div - 1);       
        }
        cobra_pcrm_i2s0_clk_en(1);
    }
    else {
        if(i2s_iSTx == I2S_TX) {
            cobra_pcrm_i2s1_transmitter_mode(MASTER_MODE_FORM_APLL);
            cobra_pcrm_i2s1_tx_mode_sclk_div(m_s_Div - 1);
        }
        else{
            cobra_pcrm_i2s1_receiver_mode(RECEIVER_MASTER_MODE_FORM_APLL);
            cobra_pcrm_i2s1_rx_mode_sclk_div(m_s_Div - 1);       
        }
        cobra_pcrm_i2s1_clk_en(1);
    }
}

void i2s_clk_config(char i2s_type,uint8_t clocksource,double sample_clk_kHz,uint8_t width,uint8_t sampling_point)
{
    uint16_t pll_sclk_div = 0;
    double mclk_kHz = 0;   
    if(clocksource == CLK_FROM_PLL)
    {
#if (COBRA_PLL_EN)
        cobra4_aon_wk_protection_en(1);
        cobra4_aon_wk_d2a_ldoanaforpll_en(1);
        cobra4_aon_wk_protection_en(0);
        if(sample_clk_kHz == 22.05 || sample_clk_kHz == 44.1 || sample_clk_kHz == 88.2 || sample_clk_kHz == 176.4)
        {
            set_pll_frenquence(180.6336);
            mclk_kHz = 180.6336*1000/8;
            pll_sclk_div = mclk_kHz/(sample_clk_kHz*width*2*(sampling_point/width));        
        }
        else
        {
            set_pll_frenquence(196.608);
            mclk_kHz = 196.608*1000/8;
            pll_sclk_div = 24576/(sample_clk_kHz*width*2*(sampling_point/width));
        }
        i2s_pll_mode_set(i2s_type,pll_sclk_div,I2S_TX);
        i2s_pll_mode_set(i2s_type,pll_sclk_div,I2S_RX);
#endif
    }
    else
    {
        mclk_kHz = 12288;
        pll_sclk_div = mclk_kHz/(sample_clk_kHz*width*2*(sampling_point/width));
        i2s_clk_mode_set(i2s_type,sample_clk_kHz,48000,width,pll_sclk_div,sampling_point,I2S_RX,RX_MODULE_MASTER);
        i2s_clk_mode_set(i2s_type,sample_clk_kHz,48000,width,pll_sclk_div,sampling_point,I2S_TX,TX_MODULE_MASTER);
    }
}
#endif
#endif











