//liujian add
//2018/6/29
#include "rdw_i2s.h"

static volatile RDW_I2S2_T   *rdw_i2s;

void rdw_init_i2s(void)
{
		rdw_i2s = I2S0;
	
		rdw_i2s_rx_disable(1);
		
		i2s_clk_en(0,0);
		
}


//-------------------------------------------------------------------------------
void rdw_i2s_set_rx_mode(uint8_t mode)
{
	rdw_i2s->CFG_RX_MODE = mode;
}

void rdw_i2s_set_rx_sampling_point(uint8_t sampling_point)
{
	rdw_i2s->CFG_RX_SAMPLING_POINT_HALF_OF_FS = sampling_point;
}

void rdw_i2s_set_rx_bitwidth(uint8_t width)
{
	rdw_i2s->CFG_RX_BITWIDTH_OF_WORD	= width;
}

void rdw_i2s_rx_enable(bool isMaster)
{
	if(isMaster)
	{
		rdw_i2s->CFG_RX_CTRL |=	RX_ENABLE_MASK|RX_MASTER_MASK|RX_UT0_FIFO_ENA_MASK;
	}
	else
	{
		rdw_i2s->CFG_RX_CTRL |=	RX_ENABLE_MASK|RX_UT0_FIFO_ENA_MASK;
	}
}

void rdw_i2s_rx_disable(bool isMaster)
{
	if(isMaster)
	{
		rdw_i2s->CFG_RX_CTRL &=	~(RX_ENABLE_MASK|RX_MASTER_MASK|RX_UT0_FIFO_ENA_MASK);
	}
	else
	{
		rdw_i2s->CFG_RX_CTRL &=	~(RX_ENABLE_MASK|RX_UT0_FIFO_ENA_MASK);
	}
}

uint32_t rdw_rx_out0_upfifo_statu(void)
{
	return rdw_i2s->RX_OUT0_UPFIFO_STATU;
}

uint32_t rdw_read_out0_upfifo_data(void)
{
	while((rdw_rx_out0_upfifo_statu()&OUT0_UPFIFO_EMPTY_MASK) == OUT0_UPFIFO_EMPTY_MASK);
	return rdw_i2s->OUT0_UPFIFO_READ_DATA;
}





void rdw_i2s_set_tx_mode(uint8_t mode)
{
	rdw_i2s->CF_TX_MODE = mode;
}

void rdw_i2s_set_tx_sampling_point(uint8_t sampling_point)
{
	rdw_i2s->CFG_TX_SAMPLING_POINT_HALF_OF_FS = sampling_point;
}

void rdw_i2s_set_tx_bitwidth(uint8_t width)
{
	rdw_i2s->CFG_TX_BITWIDTH_OF_WORD	= width;
}

void rdw_i2s_tx_enable(bool isMaster)
{
	if(isMaster)
	{
		rdw_i2s->CFG_TX_CTRL |=	TX_ENABLE_MASK|TX_MASTER_MASK|TX_IN0_DNFIFO_ENABLE_MASK;
	}
	else
	{
		rdw_i2s->CFG_TX_CTRL |=	TX_ENABLE_MASK|TX_IN0_DNFIFO_ENABLE_MASK;
	}
}

void rdw_i2s_src_tx_enable(bool isMaster)
{
	if(isMaster)
	{
		rdw_i2s->CFG_TX_CTRL |=	TX_ENABLE_MASK|TX_MASTER_MASK|TX_IN1_DNFIFO_ENABLE_MASK|TX_WHO_TO_I2S_MASK;
	}
	else
	{
		rdw_i2s->CFG_TX_CTRL |=	TX_ENABLE_MASK|TX_IN1_DNFIFO_ENABLE_MASK|TX_WHO_TO_I2S_MASK;
	}
}

void set_tx_in_dnfifo_almostfull_threshold(uint8_t infifo_type,uint8_t threshold)
{
	if(infifo_type == 0)
		rdw_i2s->CFG_TX_IN0_DNFIFO_ALMOSTFULL_THRESHOLD = threshold;
	else
		rdw_i2s->CFG_TX_IN1_DNFIFO_ALMOSTFULL_THRESHOLD = threshold;;
}

uint32_t rdw_tx_in_dnfifo_status(uint8_t infifo_type)
{
	if(infifo_type == 0)
		return rdw_i2s->TX_IN0_DNFIFO_STATUS;
	else
		return rdw_i2s->TX_IN1_DNFIFO_STATUS;
}

void rdw_i2s_write_in_dnfifo(uint32_t in0_fifo_data)
{
	while((rdw_tx_in_dnfifo_status(0)&IN_UPFIFO_FULL_MASK)==IN_UPFIFO_FULL_MASK);
	rdw_i2s->IN0_DNFIFO_WRITE_DATA = in0_fifo_data;
}

void rdw_i2s_ws_swap(uint8_t swap)
{
	rdw_i2s->WS_SWAP = swap;
}

void rdw_i2s_int_en(uint8_t mask)
{
	rdw_i2s->CFG_INTERRUPT_MASK &= ~mask; 
}

void rdw_i2s_int_mask(uint8_t mask)
{
	rdw_i2s->CFG_INTERRUPT_MASK |= mask; 
}

void rdw_i2s_dma_ctrl_sel(uint8_t sel)
{
	rdw_i2s->DMA_CTRL_SEL = sel; 
}

void rdw_i2s_invert_lrclk(uint8_t inv)
{
	if(inv)
	{
		rdw_i2s->CFG_RX_CTRL = (rdw_i2s->CFG_RX_CTRL|(3<<4));
	}
	else
	{
		rdw_i2s->CFG_RX_CTRL = (rdw_i2s->CFG_RX_CTRL&(~(3<<4)));
	}
}

void cfg_txrx_common_mode_en(bool en)
{
	if(en){
		rdw_i2s->CFG_TXRX_COMMON_MODE = CFG_TXRX_COMMON_MODE_EN;
	}else{
		rdw_i2s->CFG_TXRX_COMMON_MODE &= ~(CFG_TXRX_COMMON_MODE_EN|CFG_TXRX_COMMON_MODE_SYN);
	}
}

void cfg_txrx_common_mode_start(void)
{
	rdw_i2s->CFG_TXRX_COMMON_MODE = (CFG_TXRX_COMMON_MODE_EN|CFG_TXRX_COMMON_MODE_SYN);
}

void rdw_i2s_rx_init(uint8_t mode,uint8_t width,uint8_t sampling_point,bool dma_en,bool isMaster)
{
	rdw_i2s_set_rx_mode(mode);
	rdw_i2s_set_rx_bitwidth(width);
	rdw_i2s_set_rx_sampling_point(sampling_point);
	if(dma_en){
		//rdw_i2s->DMA_CTRL_SEL |= (DMA_RX_ENABLE_MASK|DMA_RX_REQ_BY_ALMOSTFULL_MASK);
		rdw_i2s->DMA_CTRL_SEL &= ~(DMA_RX_REQ_BY_ALMOSTFULL_MASK);
		rdw_i2s->DMA_CTRL_SEL |= (DMA_RX_ENABLE_MASK);
	}else{
		rdw_i2s->DMA_CTRL_SEL &= ~(DMA_RX_ENABLE_MASK);
	}
 // rdw_i2s_rx_enable(isMaster);
}

void rdw_i2s_tx_init(uint8_t mode,uint8_t width,uint8_t sampling_point,bool dma_en,bool isMaster)
{
	rdw_i2s_set_tx_mode(mode);
	rdw_i2s_set_tx_bitwidth(width);
	rdw_i2s_set_tx_sampling_point(sampling_point);
	if(dma_en){
		rdw_i2s->DMA_CTRL_SEL |= (DMA_TX_ENABLE_MASK|DMA_TX_STOP_BY_FIFO_OVERRUN_MASK|DMA_TX_REQ_BY_ALMOSTFULL_MASK);
	}else{
		rdw_i2s->DMA_CTRL_SEL &= ~(DMA_TX_ENABLE_MASK|DMA_TX_STOP_BY_FIFO_OVERRUN_MASK|DMA_TX_REQ_BY_ALMOSTFULL_MASK);
	}
	rdw_i2s_tx_enable(isMaster);
}









