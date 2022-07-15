/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_i2s.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/8

	Description :   
	
	Changed Log	:

		Liujian  2019/3/28		- Creation



*****************************************************************************/

#ifndef __COBRA_I2S_H__
#define __COBRA_I2S_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_I2S_EN

typedef struct cobra_i2s_type {
	
	  __attribute__ ((aligned (4))) volatile unsigned int RESERVED1[9];         												/*0x000-0x20*/
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_TXRX_COMMON_MODE;          								/*0x024*/	
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_TX_CTRL;          												/*0x028*/
	  __attribute__ ((aligned (4))) volatile unsigned int CF_TX_MODE;        												 		/*0x02C*/        
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_TX_BITWIDTH_OF_WORD;  										/*0x030*/           
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_TX_SAMPLING_POINT_HALF_OF_FS;  						/*0x034*/
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_TX_PCM_LONG_FRAME_HOLDING;      					/*0x038*/ 
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_TX_IN0_DNFIFO_ALMOSTFULL_THRESHOLD;     	/*0x03C*/
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_TX_IN1_DNFIFO_ALMOSTFULL_THRESHOLD;   		/*0x040*/      
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_TX_SLOT_NUM;      											  /*0x044*/ 
	  __attribute__ ((aligned (4))) volatile unsigned int RESERVED2[12];       													/*0x048-0x074*/ 
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_RX_CTRL;     								 					   	/*0x078*/
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_RX_MODE;       														/*0x07C*/
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_RX_BITWIDTH_OF_WORD;         							/*0x080*/     
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_RX_SAMPLING_POINT_HALF_OF_FS;         		/*0x084*/     
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_RX_PCM_LONG_FRAME_HOLDING;        				/*0x088*/     
	  __attribute__ ((aligned (4))) volatile unsigned int CFG_RX_SLOT_NUM;    					   						  /*0x08C*/   
	  __attribute__ ((aligned (4))) volatile unsigned int RESERVED3[14];																/*0x090-0xC4*/  
		__attribute__ ((aligned (4))) volatile unsigned int DMA_CTRL_SEL;    					   		 							/*0x0C8*/
		__attribute__ ((aligned (4))) volatile unsigned int IN0_DNFIFO_WRITE_DATA;    					    			/*0x0CC*/
		__attribute__ ((aligned (4))) volatile unsigned int OUT0_UPFIFO_READ_DATA;    					  		  	/*0x0D0*/
		__attribute__ ((aligned (4))) volatile unsigned int CFG_INTERRUPT_MASK;    					    					/*0x0D4*/
		__attribute__ ((aligned (4))) volatile unsigned int WS_SWAP;    					  										  /*0x0D8*/
		__attribute__ ((aligned (4))) volatile unsigned int RESERVED4[5];    					  									/*0xDC-0xEC*/
		__attribute__ ((aligned (4))) volatile unsigned int RX_OUT0_UPFIFO_STATU;    					  					/*0xF0*/
		__attribute__ ((aligned (4))) volatile unsigned int TX_IN0_DNFIFO_STATUS;    					  					/*0xF4*/
		__attribute__ ((aligned (4))) volatile unsigned int TX_IN1_DNFIFO_STATUS;    					  					/*0xF8*/
} COBRA_I2S_TypeDef;

/*Register*/
/*CFG_TXRX_COMMON_MODE*/
#define CFG_TXRX_COMMON_MODE_EN	    0x1UL
#define CFG_TXRX_COMMON_MODE_SYN	0x2UL

/*CFG_TX_CTRL*/
#define TX_ENABLE_POS			0UL
#define TX_ENABLE_MASK			0x1UL<<TX_ENABLE_POS
#define TX_CLOSE			0
#define TX_ENABLE				1

#define TX_MASTER_POS				1UL
#define TX_MASTER_MASK				0x1UL<<TX_MASTER_POS
#define TX_MODULE_SLAVE					0
#define TX_MODULE_MASTER				1

#define TX_WHO_TO_I2S_POS				2UL
#define TX_WHO_TO_I2S_MASK					0x1UL<<TX_WHO_TO_I2S_POS
#define CPU_WITE_DNFIFO0_TO_I2S						0
#define INTERNAL_PORT_WRITE_DNFIFO1_TO_I2S				1

#define TX_IN0_DNFIFO_ENABLE_POS																										3UL
#define TX_IN0_DNFIFO_ENABLE_MASK																										0x1UL<<TX_IN0_DNFIFO_ENABLE_POS

#define TX_DNFIFO_CLOSE																					0
#define TX_DNFIFO_ENABLE																				1

#define TX_IN1_DNFIFO_ENABLE_POS																										4UL
#define TX_IN1_DNFIFO_ENABLE_MASK																										0x1UL<<TX_IN1_DNFIFO_ENABLE_POS

#define TX_WS_INIT_VLD_POS																													5UL
#define TX_WS_INIT_VLD_MASK																													0x1UL<<TX_WS_INIT_VLD_POS
#define WS_INIT_GENERAL																					 0
#define WS_INIT_SPECIAL																					 1

#define TX_WS_INIT_POS																															6UL
#define TX_WS_INIT_MASK																															0x1<<TX_WS_INIT_POS
#define INITIAL_WS_0																						 0
#define INITIAL_WS_1																						 1

#define TX_BIT_DELAY_VLD_POS																												7UL
#define TX_BIT_DELAY_VLD_MASK																												0x1UL<<TX_BIT_DELAY_VLD_POS
#define BIT_DELAY_GENERAL																				 0
#define BIT_DELAY_SPECIAL																				 1

#define TX_BIT_DELAY_POS																														8UL
#define TX_BIT_DELAY_MASK																														0x1UL<<TX_BIT_DELAY_POS
#define NOT_NEED_DELAY_ONE_CYCLE																 0
#define NEED_DELAY_ONE_CYCLE																		 1

#define TX_LEFT_ALIGNED_VLD_POS																											9UL
#define TX_LEFT_ALIGNED_VLD_MASK																										0x1UL<<TX_LEFT_ALIGNED_VLD_POS
#define LEFT_ALIGNED_GENERAL																		 0
#define LEFT_ALIGNED_SPECIAL																		 1

#define TX_LEFT_ALIGNED_POS																													10UL
#define TX_LEFT_ALIGNED_MASK																												0x1UL<<TX_LEFT_ALIGNED_POS
#define RIGHT_ALIGNED																						 0
#define LEFT_ALIGNED																						 1


#define TX_SLOT_NUM_VLD_POS																													12UL
#define TX_SLOT_NUM_VLD_MASK																												0x1UL<<TX_SLOT_NUM_VLD_POS
#define USE_DEFAULT_SLOT_NUM																		 0
#define USE_SLOT_NUM_THAT_CPU_CONFIG														 1


/*CFG_RX_CTRL*/
#define RX_ENABLE_POS																																0UL
#define RX_ENABLE_MASK																															0x1UL<<RX_ENABLE_POS
#define RX_CLOSE																								 0
#define RX_ENABLE																								 1

#define RX_MASTER_POS																																1UL
#define RX_MASTER_MASK																															0x1UL<<RX_MASTER_POS
#define RX_MODULE_SLAVE																					 0
#define RX_MODULE_MASTER																				 1

//#define RX_I2S_TO_WHO_POS																														2UL
//#define RX_I2S_TO_WHO_MASK																													0x1UL<<RX_I2S_TO_WHO_POS
//#define RX_DATA_TO_CPU																					 0
//#define RX_DATA_TO_INTERNAL_PORT															   1

#define RX_OUT0_FIFO_ENA_POS																												3UL
#define RX_UT0_FIFO_ENA_MASK																												0x1UL<<RX_OUT0_FIFO_ENA_POS
#define UPFIFO0_CLOSE																						 0
#define UPFIFO0_ENABLE																				   1

#define RX_WS_INIT_VLD_POS																													4UL
#define RX_WS_INIT_VLD_MASK																													0x1UL<<RX_WS_INIT_VLD_POS

#define RX_WS_INIT_POS																															5UL
#define RX_WS_INIT_MASK																															0x1UL<<RX_WS_INIT_POS

#define RX_BIT_DELAY_VLD_POS																												6UL
#define RX_BIT_DELAY_VLD_MASK																												0x1UL<<RX_BIT_DELAY_VLD_POS

#define RX_BIT_DELAY_POS																														7UL
#define RX_BIT_DELAY_MASK																														0x1UL<<RX_BIT_DELAY_POS

#define RX_NEG_CLK_POS																															8UL
#define RX_NEG_CLK_MASK																															0x1UL<<RX_NEG_CLK_POS
#define LOCALRX_CLK																							 0
#define NONE_LOCALRX_CLK																				 1

#define RX_SLOT_NUM_VLD_POS																													10UL
#define RX_SLOT_NUM_VLD_MASK																												0x1UL<<RX_SLOT_NUM_VLD_POS

/*DMA_CTRL_SEL*/
#define DMA_TX_ENABLE_POS																														0UL
#define DMA_TX_ENABLE_MASK																													0x1UL<<DMA_TX_ENABLE_POS
#define DMA_DISABLE																								0
#define DMA_ENABLE																								1

#define DMA_RX_ENABLE_POS																														4UL
#define DMA_RX_ENABLE_MASK																													0x1UL<<DMA_RX_ENABLE_POS

#define DMA_TX_STOP_BY_FIFO_OVERRUN_POS																							1UL
#define DMA_TX_STOP_BY_FIFO_OVERRUN_MASK																						0x1UL<<DMA_TX_STOP_BY_FIFO_OVERRUN_POS
#define DMA_STILL_REQ_WHEN_OVERFLOW_OR_UNDERFLOW									0
#define DMA_NO_REQ_WHEN_OVERFLOW_OR_UNDERFLOW											1

#define DMA_RX_STOP_BY_FIFO_OVERRUN_POS																							5UL
#define DMA_RX_STOP_BY_FIFO_OVERRUN_MASK																						0x1UL<<DMA_RX_STOP_BY_FIFO_OVERRUN_POS

#define DMA_TX_ERROR_POS																														2UL
#define DMA_TX_ERROR_MASK																														0x1UL<<DMA_TX_ERROR_POS
#define DMA_NO_ERROR																							0
#define DMA_ERROR																									1

#define DMA_RX_ERROR_POS																														6UL
#define DMA_RX_ERROR_MASK																														0x1UL<<DMA_RX_ERROR_POS

#define DMA_TX_REQ_BY_ALMOSTFULL_POS																								3UL
#define DMA_TX_REQ_BY_ALMOSTFULL_MASK																								0x1UL<<DMA_TX_REQ_BY_ALMOSTFULL_POS

#define DMA_RX_REQ_BY_ALMOSTFULL_POS																								7UL
#define DMA_RX_REQ_BY_ALMOSTFULL_MASK																								0x1UL<<DMA_RX_REQ_BY_ALMOSTFULL_POS

#define IN0_ALMOSTFULL_THRESHOLD_POS																								8UL
#define	IN0_ALMOSTFULL_THRESHOLD_MASK																								0xFFUL<<OUT0_ALMOSTFULL_THRESHOLD_POS

#define OUT0_ALMOSTFULL_THRESHOLD_POS																								16UL
#define	OUT0_ALMOSTFULL_THRESHOLD_MASK																							0xFF<<OUT0_ALMOSTFULL_THRESHOLD_POS

/*CFG_INTERRUPT_MASK*/
#define OUT0_UPFIFO_FULL_INTERRUPT_MASK_POS																					0UL
#define OUT0_UPFIFO_FULL_INTERRUPT_MASK_MASK																				0x1UL<<OUT0_UPFIFO_FULL_INTERRUPT_MASK_POS

#define OUT0_UPFIFO_UNDERFLOW_INTERRUPT_MASK_POS																		1UL
#define OUT0_UPFIFO_UNDERFLOW_INTERRUPT_MASK_MASK																		0x1UL<<OUT0_UPFIFO_UNDERFLOW_INTERRUPT_MASK_POS

#define OUT0_UPFIFO_OVERFLOW_INTERRUPT_MASK_POS																			2UL
#define OUT0_UPFIFO_OVERFLOW_INTERRUPT_MASK_MASK																		0x1UL<<OUT0_UPFIFO_OVERFLOW_INTERRUPT_MASK_POS

#define OUT0_UPFIFO_EMPTY_INTERRUPT_MASK_POS																				3UL
#define OUT0_UPFIFO_EMPTY_INTERRUPT_MASK_MASK																				0x1UL<<OUT0_UPFIFO_EMPTY_INTERRUPT_MASK_POS

#define OUT0_UPFIFO_ALMOSTFULL_INTERRUPT_MASK_POS																		4UL
#define OUT0_UPFIFO_ALMOSTFULL_INTERRUPT_MASK_MASK																	0x1UL<<OUT0_UPFIFO_ALMOSTFULL_INTERRUPT_MASK_POS

#define IN0_DNFIFO_FULL_INTERRUPT_MASK_POS																					5UL
#define IN0_DNFIFO_FULL_INTERRUPT_MASK_MASK																					0x1UL<<IN0_DNFIFO_FULL_INTERRUPT_MASK_POS

#define IN0_DNFIFO_UNDERFLOW_INTERRUPT_MASK_POS																			6UL
#define IN0_DNFIFO_UNDERFLOW_INTERRUPT_MASK_MASK																		0x1UL<<IN0_DNFIFO_UNDERFLOW_INTERRUPT_MASK_POS

#define IN0_DNFIFO_OVERFLOW_INTERRUPT_MASK_POS																			7UL
#define IN0_DNFIFO_OVERFLOW_INTERRUPT_MASK_MASK																			0x1UL<<IN0_DNFIFO_OVERFLOW_INTERRUPT_MASK_POS

#define IN0_DNFIFO_EMPTY_INTERRUPT_MASK_POS																					8UL
#define IN0_DNFIFO_EMPTY_INTERRUPT_MASK_MASK																				0x1UL<<IN0_DNFIFO_EMPTY_INTERRUPT_MASK_POS

#define IN0_DNFIFO_ALMOSTFULL_INTERRUPT_MASK_POS																		9UL
#define IN0_DNFIFO_ALMOSTFULL_INTERRUPT_MASK_MASK																		0x1UL<<IN0_DNFIFO_ALMOSTFULL_INTERRUPT_MASK_POS

#define IN1_DNFIFO_FULL_INTERRUPT_MASK_POS																					10UL
#define IN1_DNFIFO_FULL_INTERRUPT_MASK_MASK																					0x1UL<<IN1_DNFIFO_FULL_INTERRUPT_MASK_POS

#define IN1_DNFIFO_UNDERFLOW_INTERRUPT_MASK_POS																			11UL
#define IN1_DNFIFO_UNDERFLOW_INTERRUPT_MASK_MASK																		0x1UL<<IN1_DNFIFO_UNDERFLOW_INTERRUPT_MASK_POS

#define IN1_DNFIFO_OVERFLOW_INTERRUPT_MASK_POS																			12UL
#define IN1_DNFIFO_OVERFLOW_INTERRUPT_MASK_MASK																			0x1UL<<IN1_DNFIFO_OVERFLOW_INTERRUPT_MASK_POS

#define IN1_DNFIFO_EMPTY_INTERRUPT_MASK_POS																					13UL
#define IN1_DNFIFO_EMPTY_INTERRUPT_MASK_MASK																				0x1UL<<IN1_DNFIFO_EMPTY_INTERRUPT_MASK_POS

#define IN1_DNFIFO_ALMOSTFULL_INTERRUPT_MASK_POS																		14UL
#define IN1_DNFIFO_ALMOSTFULL_INTERRUPT_MASK_MASK																		0x1UL<<IN1_DNFIFO_ALMOSTFULL_INTERRUPT_MASK_POS

#define OUT0_UPFIFO_INTERRUPT_MASK_POS																							15UL
#define OUT0_UPFIFO_INTERRUPT_MASK_MASK																							0x1UL<<OUT0_UPFIFO_INTERRUPT_MASK_POS

#define IN0_DNFIFO_INTERRUPT_MASK_POS																								16UL
#define IN0_DNFIFO_INTERRUPT_MASK_MASK																							0x1UL<<IN0_DNFIFO_INTERRUPT_MASK_POS

#define IN1_DNFIFO_INTERRUPT_MASK_POS																								17UL
#define IN1_DNFIFO_INTERRUPT_MASK_MASK																							0x1UL<<IN1_DNFIFO_INTERRUPT_MASK_POS

/*WS_SWAP*/
#define TX_WS_TO_RX_POS																															0UL
#define TX_WS_TO_RX_MASK																														0x1UL<<TX_WS_TO_RX_POS

#define RX_WS_TO_TX_POS																															1UL
#define RX_WS_TO_TX_MASK																														0x1UL<<RX_WS_TO_TX_POS

/*RX_OUT0_UPFIFO_STATU*/
#define OUT0_UPFIFO_OVERFLOW_POS																										0UL
#define OUT0_UPFIFO_OVERFLOW_MASK																										0x1UL<<OUT0_UPFIFO_OVERFLOW_POS

#define OUT0_UPFIFO_UNDERFLOW_POS																										1UL
#define OUT0_UPFIFO_UNDERFLOW_MASK																									0x1UL<<OUT0_UPFIFO_UNDERFLOW_POS

#define OUT0_UPFIFO_FULL_POS																												2UL
#define OUT0_UPFIFO_FULL_MASK																												0x1UL<<OUT0_UPFIFO_FULL_POS

#define OUT0_UPFIFO_ALMOSTFULL_POS																									3UL
#define OUT0_UPFIFO_ALMOSTFULL_MASK																									0x1UL<<OUT0_UPFIFO_ALMOSTFULL_POS

#define OUT0_UPFIFO_EMPTY_POS																												4UL
#define OUT0_UPFIFO_EMPTY_MASK																											0x1UL<<OUT0_UPFIFO_EMPTY_POS

#define OUT0_UPFIFO_FILLED_NUM_POS																									8UL
#define OUT0_UPFIFO_FILLED_NUM_MASK																									0xFFUL<<OUT0_UPFIFO_EMPTY_POS

/*TX_IN_DNFIFO_STATUS*/
#define IN_UPFIFO_OVERFLOW_POS																											0UL
#define IN_UPFIFO_OVERFLOW_MASK																											0x1UL<<IN_UPFIFO_OVERFLOW_POS

#define IN_UPFIFO_UNDERFLOW_POS																											1UL
#define IN_UPFIFO_UNDERFLOW_MASK																										0x1UL<<IN_UPFIFO_UNDERFLOW_POS

#define IN_UPFIFO_FULL_POS																													2UL
#define IN_UPFIFO_FULL_MASK																													0x1UL<<IN_UPFIFO_FULL_POS

#define IN_UPFIFO_ALMOSTFULL_POS																										3UL
#define IN_UPFIFO_ALMOSTFULL_MASK																										0x1UL<<IN_UPFIFO_ALMOSTFULL_POS

#define IN_UPFIFO_EMPTY_POS																													4UL
#define IN_UPFIFO_EMPTY_MASK																												0x1UL<<IN_UPFIFO_EMPTY_POS

#define IN_UPFIFO_FILLED_NUM_POS																										8UL
#define IN_UPFIFO_FILLED_NUM_MASK																										0xFFFFUL<<IN_UPFIFO_FILLED_NUM_POS

#define I2S_TX																	0x1
#define I2S_RX																	0x0

typedef enum
{
	I2S0,
	I2S1,
}i2s_id;

typedef enum
{
	I2S_MODE,
	LEFT_JUSTED_MODE,
	RIGHT_JUSTED_MODE,
	PCM_STERO_MODE,
	PCM_MONO_MODE,
	PCM_SHORT_FRAME_MODE,
	PCM_LONG_FRAME

}i2s_work_mode;

typedef enum
{
	SAMPLE_16BIT,
	SAMPLE_32BIT,
	SAMPLE_64BIT,
	SAMPLE_128BIT
}sample_point;

typedef enum
{
	SAMPLE_WORD_8BIT,
	SAMPLE_WORD_16BIT,
	SAMPLE_WORD_24BIT,
	SAMPLE_WORD_32BIT,
	SAMPLE_WORD_64BIT
}sample_word_width;

typedef enum i2s_tx_mode {
		I2S_SLAVE,
		I2S_MASTER
}I2S_TX_MODE;

typedef enum i2s_who_to_i2s {
	CPU_WRITE_DNFIFO0,
	INTERNAL_WRITE_DNFIFO1
}I2S_WHO_TO_I2S;


typedef enum i2s_i2s_to_who{
	RX_DATA_TO_CPU,
	RX_DATA_TO_INTERNAL
}I2S_I2S_TO_WHO;

typedef enum {
		I2S_PLAY,
		I2S_RECORD,	
}I2S_SOUND_MODE;

/*VM8978 MODE*/
#define	VM8978_MSB_MODE						0UL
#define VM8978_LSB_MODE						1UL
#define	VM8978_I2S_MODE						2UL
#define VM8978_PCM_MODE						3UL

/*VM8978 LEN*/
#define	VM8978_LEN_16							0UL
#define VM8978_LEN_20							1UL
#define	VM8978_LEN_24							2UL
#define VM8978_LEN_32							3UL


#define STATE_OUT0_UPFIFO_OVERFLOW			0x01
#define STATE_OUT0_UPFIFO_UNDERFLOW			0x02
#define STATE_OUT0_UPFIFO_FULL				0x04
#define STATE_OUT0_UPFIFO_ALMOSTFULL		0x08
#define STATE_OUT0_UPFIFO_EMPTY				0x10


#define STATE_IN0_UPFIFO_OVERFLOW		0x01
#define STATE_IN0_UPFIFO_UNDERFLOW		0x02
#define STATE_IN0_UPFIFO_FULL			0x04
#define STATE_IN0_UPFIFO_ALMOSTFULL		0x08
#define STATE_IN0_UPFIFO_EMPTY			0x10


#define STATE_IN1_UPFIFO_OVERFLOW		0x01
#define STATE_IN1_UPFIFO_UNDERFLOW		0x02
#define STATE_IN1_UPFIFO_FULL			0x04
#define STATE_IN1_UPFIFO_ALMOSTFULL		0x08
#define STATE_IN1_UPFIFO_EMPTY			0x10


#define COBRA_I2S0_BASE				(0x400CE000UL)
#define COBRA_I2S1_BASE				(0x400CF000UL)

struct i2s_env_tag {
		i2s_id    index;
		double    sample_rate;   //khz
		uint32_t	clk_in_khz;
		uint8_t		bit_num;
		uint8_t   m_s_div;
		
		uint16_t  				sample_ponit;
		I2S_SOUND_MODE 		sound_mode;
		bool							i2s_is_master;

};

typedef enum {
	CLK_48MHZ,
	CLK_PLL	
}CLOCK_SOURCE;


#ifdef COBRA_I2S_NEW
#define CLK_FROM_PLL                        1
#define CLK_FROM_XTAL                       0

extern volatile COBRA_I2S_TypeDef *  cobra_i2s0;
extern volatile COBRA_I2S_TypeDef *  cobra_i2s1;
#endif

void cobra_i2s_init(void);

void app_cobra_cfg_txrx_common_mode_en(i2s_id index, bool en);

void cobra_i2s_rx_init(i2s_id index, i2s_work_mode mode,sample_word_width width, sample_point point,bool dma_en);

void cobra_i2s_tx_init(i2s_id index, i2s_work_mode mode, sample_word_width width, sample_point point, bool dma_en);

void * cobra_i2s_get_m3_i2s_read_address(i2s_id i2s_index);
void * cobra_i2s_get_m3_i2s_write_address(i2s_id i2s_index);

void cobra_i2s_begin_work(i2s_id index);

void cobra_i2s_invert_lrclk(i2s_id index, uint8_t inv);

void cobra_i2s_send_data(i2s_id index, uint32_t data);

uint32_t cobra_get_tx_in_dnfifo_status(i2s_id index, uint8_t infifo_type);

void cobra_i2s_src_tx_enable(i2s_id index, bool isMaster);

void cobra_i2s_tx_disable(i2s_id index, bool isMaster);

void cobra_i2s_tx_enable(i2s_id index, bool isMaster);

void cobra_i2s_set_tx_bitwidth(i2s_id index, sample_word_width width);

void cobra_i2s_set_tx_sampling_point(i2s_id index, sample_point point);

void cobra_i2s_rx_disable(i2s_id index , bool isMaster);

void cobra_i2s_rx_enable(i2s_id index, bool isMaster);

void cobra_i2s_set_rx_bitwidth(i2s_id index, sample_word_width width);

void cobra_i2s_set_rx_sampling_point(i2s_id index, sample_point point);

void cobra_i2s_set_rx_mode(i2s_id index, i2s_work_mode mode);

void app_cobra_i2s_set_clock_mode(struct i2s_env_tag  * i2s_env);

void * cobra_i2s_get_m3_i2s_write_address(i2s_id i2s_index);

void app_cobra_i2s_tx_dma_enable(i2s_id index, unsigned int dma_en);

void app_cobra_i2s_clk_config(i2s_id index, CLOCK_SOURCE clk_source, double sample_clk_kHz, sample_word_width s_width, sample_point point);

#ifdef COBRA_I2S_NEW
void cobra_i2s_set_tx_mode(i2s_id index, i2s_work_mode mode);
void i2s_clk_mode_set(uint8_t i2s_type,double sample_clk_kHz,uint32_t clk_in_Khz,uint8_t bit_num,uint8_t m_s_Div,uint16_t sample_ponit,uint8_t i2s_iSTx,bool isMaster);
void i2s_pll_mode_set(char i2s_type,uint8_t m_s_Div,uint8_t i2s_iSTx);
void i2s_clk_config(char i2s_type,uint8_t clocksource,double sample_clk_kHz,uint8_t width,uint8_t sampling_point);
#endif

#endif   //COBRA_I2S_EN



#endif

