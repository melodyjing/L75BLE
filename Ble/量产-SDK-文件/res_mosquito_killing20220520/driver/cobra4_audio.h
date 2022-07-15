/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.	
	Author:Liujian
	cobra Drivers
	File Name	:	cobra4_audio.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/2/22
	Description :   
	Changed Log	:
		Liujian  2020/2/22			- Creation
*****************************************************************************/
#ifndef __COBRA4_AUDIO_H__
#define __COBRA4_AUDIO_H__

#include <cobra_config.h>
#include "rwip_config.h"
#include "ke_event.h"
#include "rdw_mcu.h"
#include "co_list.h"
#include "ke_mem.h"         // Used to allocate memory environment
#include "co_list.h"        // List of AES execution to perform
#include "co_math.h"        // For rand functions

#if COBRA4_AUDIO_EN

typedef struct cobra4_audio_type
{
			__attribute__ ((aligned (4))) volatile uint32_t reserver1[10];
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_ctrl;
	  
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_mclk;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_pdm;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_adc_data_intf;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_dma_upfifo_mask;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_interrupt_sticky;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir_group;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_upfifo_almost_threshold ;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_dma_upfifo_ctrl;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_cic;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_bypass;
	
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_out_bitwidth;
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_dc_offset;
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_who_to_out;
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_zero_crossing_timeout_value;
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_volume_slide_ctrl;
			
			__attribute__ ((aligned (4))) volatile uint32_t reserver2[4];
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_hp_iir_num;   //30
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_hp_iir_den;   //31
			
			__attribute__ ((aligned (4))) volatile uint32_t reserver3;
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_clk_speed;     //33
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_high_speed_fir;      //34
			
			__attribute__ ((aligned (4))) volatile uint32_t reserver4[15];          
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir0_num;       //50
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir0_den;       //51
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir1_num;       //52
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir1_den;       //53
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir2_num;       //54
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir2_den;       //55
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir3_num;       //56
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir3_den;       //57
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir4_num;       //58
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir4_den;       //59
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir5_num;       //60
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir5_den;       //61
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir6_num;       //62
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir6_den;       //63
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir7_num;       //64
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir7_den;       //65
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir8_num;       //66
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir8_den;       //67
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir9_num;       //68
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir9_den;       //69
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir0_g0;      //70
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir1_g0;  
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir2_g0;  
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir3_g0;  
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir4_g0;  
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir5_g0;  
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir6_g0;  
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir7_g0;  
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir8_g0;
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_iir9_g0;  
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_manual_gain_coarse;      //80
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_manual_gain_fine;   //81
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_cfg_manual_gain_mode;   //82

			__attribute__ ((aligned (4))) volatile uint32_t reserver5[13];          

			__attribute__ ((aligned (4))) volatile uint32_t audio_upfifo_rdat0;    //96
			__attribute__ ((aligned (4))) volatile uint32_t audio_upfifo_rdat1;
			__attribute__ ((aligned (4))) volatile uint32_t audio_upfifo_rdat2;
			__attribute__ ((aligned (4))) volatile uint32_t audio_upfifo_rdat3;
			__attribute__ ((aligned (4))) volatile uint32_t audio_upfifo_rdat4;
			__attribute__ ((aligned (4))) volatile uint32_t audio_upfifo_rdat5;
			__attribute__ ((aligned (4))) volatile uint32_t audio_upfifo_rdat6;
			__attribute__ ((aligned (4))) volatile uint32_t audio_upfifo_rdat7;
			
			__attribute__ ((aligned (4))) volatile uint32_t reserver6[4];          
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_int_status;    //108
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_volume_slide_status;  //109
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_upfifo_status;   //110
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_audioadc_left_channel_fifo_status;  //111
			
			__attribute__ ((aligned (4))) volatile uint32_t audio_audioadc_right_channel_fifo_status;  //112
			
			
} COBRA4_AUDIO_TypeDef;

typedef enum {
		MCLK_FROM_XTAL,
		MCLK_FROM_PLL
}COBRA4_SRC_CLK;


typedef enum {
		ADC_AUDIO_SWAP_TRUE,
		ADC_AUDIO_SWAP_FALSE
}COBRA4_ADC_SWAP;

typedef enum {
		PDM_INTERFACE,
		ADC_INTERFACE
}COBRA4_SRC_TYPE;

typedef enum {
		IIR_GROUP_MODE0,
		IIR_GROUP_MODE1,
		IIR_GROUP_MODE2,
		IIR_GROUP_MODE3,
		IIR_GROUP_MODE4,
		IIR_GROUP_MODE5,
		IIR_GROUP_MODE6,
		IIR_GROUP_MODE7,
		IIR_GROUP_MODE8
}COBRA4_IIR_GROUP_MODE;

typedef enum {
		I2S_OUT_32BITS,
		I2S_OUT_24BITS,
		I2S_OUT_20BITS,
		I2S_OUT_16BITS,
		I2S_OUT_12BITS,
		I2S_OUT_8BITS,
		I2S_OUT_WDBITS,
}COBRA4_I2S_OUT_BITS;

typedef enum {
		DMA_OUT_32BITS,
		DMA_OUT_24BITS,
		DMA_OUT_20BITS,
		DMA_OUT_16BITS,
		DMA_OUT_12BITS,
		DMA_OUT_8BITS,
		DMA_OUT_WDBITS,
}COBRA4_DMA_OUT_BITS;

typedef enum {
		SRC_OUT,     //0
		ADC_INTF_IN,   //1, 2
		SIGNED_ADC_IN,  //3
		ADC_INTF_OUT,   //4
		SRC_IN,         //5 
		CIC_IN,         //6
		CIC_OUT,        //7
		HBF0_IN,				//8
		HBF0_OUT,				//9
		HBF1_IN,				//10
		HBF1_OUT,				//11
		HBF2_IN,				//12
		HBF2_OUT,				//13
		HP_IIR_IN,          //14
		HP_IIR_OUT,					//15
		IIR_GROUP_IN,				//16
		IIR_GROUP_OUT,			//17
		HIGH_SPEED_FIR_IN,   //18
		HIGH_SPEED_FIR_OUT,  //19
		VOLUME_HAND_IN,			 //20
		VOLUME_HAND_OUT,     //21
		
		VOLUME_SLIDE_IN,			//22
		VOLUME_SLIDE_OUT,			//23
		
		AGC_IN,							//24
		ENVELOPE_OUT,				//25
		DETECTOR_OUT,				//26
		NOISE,							//27
	
		AGC_OUT,						//28	
}COBRA4_WHO_TO_SFT_DMA;


typedef enum {
		I2S_SRC_OUT,			//0
		I2S_HBF1_OUT,	    
	  I2S_HBF2_OUT,	
	  I2S_HP_IIR_OUT,	
	  I2S_HP_IIR_IN,	
	  I2S_AGC_OUT	
	
}COBRA4_WHO_TO_I2S;


typedef enum {
		SPEED_CLK_1FS,		//0
		SPEED_CLK_2FS,	    
	  SPEED_CLK_4FS,	
	  SPEED_CLK_8FS,	
	  SPEED_CLK_MCLK
	
}COBRA4_SPEED_CLK;

typedef enum {
		AMPLIFY_DATA,
		SHRINK_DATA,
}AMPLIFY_OR_SHRINK;

typedef enum {
		ZERO_DB_MODE,
		FINE_GAIN_MODE,
		COARSE_GAIN_MODE,
		COARSE_PLUS_FINE_MODE,
}MANUAL_GAIN_MODE;

#define  AUDIO_INTERRUPT												0x0080
#define  AUDIO_STICKY_INT												0x0040
#define  AUDIO_UPFIFO_ALMOSTFULL_STICKY_INT			0x0020
#define  AUDIO_UPFIFO_ALMOSTEMPTY_STICKY_INT		0x0010
#define  AUDIO_UPFIFO_EMPTY_STICKY_INT					0x0008
#define  AUDIO_UPFIFO_OVERFLOW_STICKY_INT				0x0004
#define  AUDIO_UPFIFO_UNDERFLOW_STICKY_INT			0x0002
#define  AUDIO_UPFIFO_FULL_STICKY_INT						0x0001


#define AUDIO_V_S_TF_DECLINE_DONE_R					0x08
#define AUDIO_V_S_TF_CLIME_DONE_R						0x04
#define AUDIO_V_S_TF_DECLINE_DONE_L					0x02
#define AUDIO_V_S_TF_CLIME_DONE_L						0x01


#define AUDIO_UPFIFO_ALMOSTEMPTY					0x20
#define AUDIO_UPFIFO_EMPTY								0x10
#define AUDIO_UPFIFO_ALMOSTFULL						0x08
#define AUDIO_UPFIFO_FULL 								0x04
#define AUDIO_UPFIFO_UNDERFLOW						0x02
#define AUDIO_UPFIFO_OVERFLOW							0x01

#define AUDIO_audioadc_fifo_almostempty_l   0x20

#define  COBRA4_AUDIO_BASE  (0x400CC000UL)


#define PDM_OSR_128 128

void cobra4_audio_init(void);

struct audio_env_tag{
		COBRA4_SRC_CLK  clock_source;
		double					sample_1fs_clk_kHz;
		uint8_t					out_bitwidth;
		uint16_t				osr;
//		uint8_t					dma_en;
		double					pclk_khz;
		uint8_t					bypass_hbnum;
		uint8_t					i2s_enable;
	
		uint8_t					agc_enable;
};

void app_audio_pdm_init(struct audio_env_tag * p_param);
void app_cobra_audio_enable(void);
void app_cobra_audio_disable(void);

unsigned int * cobra4_audio_read_upfifo_address(void);

#endif
#endif
