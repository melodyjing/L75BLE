/******************************************************************************
	File Name	:	app_voice.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/10

	Description :   

	Changed Log	:
		Liujian  2020/03/10			- Creation

*****************************************************************************/

#include "app_voice.h"
#if BLE_APP_RECEIVE_VOICE

#define MAX_PLAY_SIZE  1024*8

static int16_t decode_bufferOut[2*ADPCM_DECODE_SAMPLE];

static char   i2s_play_dma_channel;   //播放声音用到 的 DMA 通道

static unsigned char app_play_buffer[MAX_PLAY_SIZE];  //BUffer, 相当于解码后的 PCM  数据的缓存

static struct cobra_ringbuffer     play_ring_buffer;


#if (I2S_PLAY_DMA_WAY == USE_BASIC_DMA_PLAY_VOICE)

static unsigned short basic_dma_play_buffer[1024];

static unsigned int voice_play_timer_index;

void app_voice_play_timer_callback(unsigned int param);

#endif

#if (I2S_PLAY_DMA_WAY == USE_PINGPONG_DMA_PLAY_VOICE)

static I2S_DMA_SIZE_TYPE     play_buffer_left[PLAY_I2S_DMA_SIZE];

static I2S_DMA_SIZE_TYPE     play_buffer_right[PLAY_I2S_DMA_SIZE];

static I2S_DMA_SIZE_TYPE     zero_buffer[PLAY_I2S_DMA_SIZE];
#endif

///function

void app_audio_i2s_play_init_buffer(void)
{
		if(cobra_ring_buffer_init(&play_ring_buffer, app_play_buffer, MAX_PLAY_SIZE))
		{
				printf("app_audio_i2s_play_init_buffer ok \n");
		}
		else
		{
				printf("app_audio_i2s_play_init_buffer error \n");
		}
	
}

//得到可以播放的语音数据的长度
unsigned int app_audio_get_audio_data_length(void)
{
		return cobra_ring_buffer_len(&play_ring_buffer);
	
}

//得到可以用于解码的buffer 的空间长度
unsigned int app_audio_get_empty_buffer_length(void)
{
		return cobra_ring_buffer_len_available(&play_ring_buffer);
}


void app_audio_put_audio_data_to_buffer(unsigned char * buffer, unsigned int length)
{
		cobra_ring_buffer_append_multiple(&play_ring_buffer, buffer, length);
}

//////////////////////////////////////////////////////////////////////////////////////

#if (I2S_PLAY_DMA_WAY == USE_PINGPONG_DMA_PLAY_VOICE)

void i2s_play_dma_callback(void)
{
		ke_event_set(KE_EVENT_I2S_PLAY);
		cobra_udma_clear_interrupt(CHANNEL15);

}
#endif

#if (I2S_PLAY_DMA_WAY == USE_PINGPONG_DMA_PLAY_VOICE)	
//这个我真的还不知道原因
//为什么要这样做 ????????????????
static void patch_dma_function(void)
{
		char i;
		for(i = 0; i < 32; i++)
		{
				cobra_udma_channel_attribute_enable(i, UDMA_ATTR_REQMASK);
			///	cobra_udma_disable_channel(i);
				cobra_udma_enable_channel(i);
				cobra_udma_clear_interrupt(i);
		}
	
}    
#endif


void app_audio_drv_dma_init(void)
{
		I2S_DMA_SIZE_TYPE * p_buffer; 

		app_audio_i2s_play_init_buffer();
	
		printf("app_audio_drv_dma_init..................... \n");
	
#if (I2S_PLAY_DMA_WAY == USE_PINGPONG_DMA_PLAY_VOICE)	
		patch_dma_function();
#endif
	
		i2s_play_dma_channel = app_rdw_udma_get_channel(CHANNEL15, CH15_I2S0_TX);
	
		cobra_udma_channel_attribute_disable(i2s_play_dma_channel,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK);


	  
	
    cobra_udma_set_channel_control(i2s_play_dma_channel | UDMA_PRI_SELECT,
                              UDMA_SIZE_16 | UDMA_SRC_INC_16 | UDMA_DST_INC_NONE |
                              UDMA_ARB_1);

    cobra_udma_set_channel_control(i2s_play_dma_channel | UDMA_ALT_SELECT,
                              UDMA_SIZE_16 | UDMA_SRC_INC_16 | UDMA_DST_INC_NONE |
                              UDMA_ARB_1);


#if (I2S_PLAY_DMA_WAY == USE_PINGPONG_DMA_PLAY_VOICE)	

	  cobra_udma_set_channel_transfer(i2s_play_dma_channel | UDMA_PRI_SELECT,
																	 UDMA_MODE_PINGPONG,
																	 play_buffer_left,
																	 (void *)cobra_i2s_get_m3_i2s_write_address(I2S0), PLAY_I2S_DMA_SIZE);
																	 
		cobra_udma_set_channel_transfer(i2s_play_dma_channel | UDMA_ALT_SELECT,
																	 UDMA_MODE_PINGPONG,
																	 play_buffer_right,
																	 (void *)cobra_i2s_get_m3_i2s_write_address(I2S0), PLAY_I2S_DMA_SIZE);
	

		app_rdw_udma_register_callback(i2s_play_dma_channel, i2s_play_dma_callback);
		
		cobra_udma_enable_channel(i2s_play_dma_channel);
													 															
		cobra_udma_enable_interrupt(CHANNEL15);	
		
		

		
#endif

		
		NVIC_ClearPendingIRQ(DMADONE_IRQn);
		
		NVIC_EnableIRQ(DMADONE_IRQn); 


		
}

void app_audio_drv_clock_init(void)
{
		struct i2s_env_tag i2s_env;
		i2s_env.i2s_is_master = true;
		i2s_env.sample_rate = 16;     //16k
		i2s_env.bit_num = 16;         //16bit, 8bit .....
		i2s_env.index = I2S0;
		i2s_env.clk_in_khz = 48000;    //16M
		i2s_env.m_s_div = 8;
		i2s_env.sound_mode = I2S_PLAY;
		i2s_env.sample_ponit = 32;
		app_cobra_i2s_set_clock_mode(&i2s_env);
}



/*
配置I2s 接口
DPIN0 - I2C0_SCL
DPIN1 - I2C0_SDA

DPIN14 - TX-BITIN
DPIN12 - TX-BITOUT
DPIN8  - I2S-LRCLK
DPIN5  - I2S-SCLK
DPIN27 - I2S-MCLK
*/
void app_audio_drv_pin_config(void)
{
	//如果用在cobra4 上面, 这里需要更改
	#if 0
		cobra_map_pinmodeset(PIN0_INDEX, PIN0_I2C0_SCL);
	#endif
		cobra_map_pinmodeset(PIN1_INDEX, PIN1_I2C0_SDA);
	
		cobra_pin_pullup_enable(PIN0_INDEX, 1);
		cobra_pin_pullup_enable(PIN1_INDEX, 1);
	
		cobra_map_pinmodeset(PIN14_INDEX, PIN14_I2SPCM0_RX_BITIN);
		cobra_map_pinmodeset(PIN12_INDEX, PIN12_I2SPCM0_TX_BITOUT);
	
		cobra_map_pinmodeset(PIN8_INDEX,  PIN8_I2S0PCM_TX_OE);
		cobra_map_pinmodeset(PIN5_INDEX,  PIN5_I2S0_TX_SCLK);
		cobra_map_pinmodeset(PIN27_INDEX, PIN27_I2S0_TX_MCLK);	
	
//		cobra_pin_drain_enable(PIN5_INDEX ,1);
//		cobra_pin_drain_enable(PIN8_INDEX ,1);
//		cobra_pin_drain_enable(PIN27_INDEX ,1);
//		cobra_pin_drain_enable(PIN12_INDEX ,1);
	
}





void app_audio_drv_init(void)
{
	
			unsigned int apb_i2c_clk;
	
			cobra_pcrm_i2c0_active_mode_clock_enable(1);   //I2C0 clock enable
			cobra_pcrm_i2c0_sleep_mode_clock_enable(1);
	
			app_audio_drv_pin_config();
	
			apb_i2c_clk = cobra_system_get_m3_apb1_clk();
		
			app_m3_i2c_init();
	
			cobra_i2c_set_bit_rate(M3_I2C0, apb_i2c_clk, 200000);    //i2c 200Khz
	
			cobra_i2c_maste_enable(M3_I2C0);
		
			cobra_pcrm_i2s0_active_mode_clock_enable(1);   //I2S0 clock enable
	
			cobra_pcrm_i2s0_sleep_mode_clock_enable(1);
#if 0
			cobra_wm8978_init();

			printf("wm8978_init.................................................. \n");
	
			app_audio_drv_clock_init();
#else
			cobra_wm8978_init_pll();
						
			app_cobra_i2s_clk_config(I2S0, CLK_PLL, 32, SAMPLE_WORD_16BIT, SAMPLE_16BIT);
			
	//		cobra_pcrm_i2s0_tx_sclk_phase(PHASE_180);

#endif

#if (I2S_PLAY_DMA_WAY == USE_PINGPONG_DMA_PLAY_VOICE)	

			app_audio_drv_dma_init();
			
			memset(zero_buffer, 0, 256*sizeof(I2S_DMA_SIZE_TYPE));
			
#endif

			cobra_i2s_tx_init(I2S0, I2S_MODE, SAMPLE_WORD_16BIT, SAMPLE_16BIT, DMA_ENABLE);
			cobra_i2s_tx_enable(I2S0, I2S_MASTER);
			
		
			cobra_wm8978_adda_cfg(DAC_EN, ADC_DISABLE);
	
			cobra_wm8978_input_cfg(MIC_DISABLE, LINEIN_DISABLE, AUX_DISABLE);
		
			cobra_wm8978_i2s_cfg(I2S_MODE, WL_16BITS, MONO_EN);     // 最后一位   , 0 单声道, 1 立体
			
			cobra_wm8978_hpvol_set(40, 40);
			
#if (I2S_PLAY_DMA_WAY == USE_BASIC_DMA_PLAY_VOICE)

			app_audio_drv_dma_init();
			
			voice_play_timer_index = cobra_soft_timer_create(app_voice_play_timer_callback);
			
			cobra_soft_timer_start(voice_play_timer_index, 1, 1);
#endif

}

#if (I2S_PLAY_DMA_WAY == USE_BASIC_DMA_PLAY_VOICE)

void app_voice_play_timer_callback(unsigned int param)
{
			uint32_t ui32Mode;
				
			ui32Mode = cobra_udma_get_channel_mode(i2s_play_dma_channel | UDMA_PRI_SELECT);
	
			if(ui32Mode == UDMA_MODE_STOP)
			{				
					if(app_audio_get_audio_data_length() >= 1024*sizeof(I2S_DMA_SIZE_TYPE))
					{
								cobra_ring_buffer_get_multiple(&play_ring_buffer, (unsigned char *)basic_dma_play_buffer, 1024*sizeof(I2S_DMA_SIZE_TYPE));					
								cobra_udma_set_channel_transfer(i2s_play_dma_channel | UDMA_PRI_SELECT,
																						 UDMA_MODE_BASIC,
																						 basic_dma_play_buffer,
																						(void *)cobra_i2s_get_m3_i2s_write_address(I2S0), 
																						 1024);																						
								cobra_udma_enable_channel(i2s_play_dma_channel);
					}													 
			}
	
}
#endif


#if (I2S_PLAY_DMA_WAY == USE_PINGPONG_DMA_PLAY_VOICE)

void app_voice_play_dma_task(void)
{

			uint32_t ui32Mode;
	
			I2S_DMA_SIZE_TYPE * p_buffer_alloc; 
			I2S_DMA_SIZE_TYPE * p_buffer_free; 
	
		 // printf("app_voice_play_dma_task \n");
	
			ui32Mode = cobra_udma_get_channel_mode(i2s_play_dma_channel | UDMA_PRI_SELECT);
	
	
			if(ui32Mode == UDMA_MODE_STOP)
			{
					if(app_audio_get_audio_data_length() >= PLAY_I2S_DMA_SIZE*sizeof(I2S_DMA_SIZE_TYPE))
					{
						cobra_ring_buffer_get_multiple(&play_ring_buffer, (unsigned char *)play_buffer_left, PLAY_I2S_DMA_SIZE*sizeof(I2S_DMA_SIZE_TYPE));
						cobra_udma_set_channel_transfer(i2s_play_dma_channel | UDMA_PRI_SELECT,
																	 UDMA_MODE_PINGPONG,
																	 play_buffer_left,
																	 (void *)cobra_i2s_get_m3_i2s_write_address(I2S0), PLAY_I2S_DMA_SIZE);
						
					}
					else
					{
							//printf("no Data 111...\n");
							//memset(play_buffer_left, 0, I2S_DMA_SIZE*sizeof(I2S_DMA_SIZE_TYPE));
							cobra_udma_set_channel_transfer(i2s_play_dma_channel | UDMA_PRI_SELECT,
																	 UDMA_MODE_PINGPONG,
																	 zero_buffer,
																	 (void *)cobra_i2s_get_m3_i2s_write_address(I2S0), PLAY_I2S_DMA_SIZE);
						
					}	
			}


			ui32Mode = cobra_udma_get_channel_mode(i2s_play_dma_channel | UDMA_ALT_SELECT);
		
			if(ui32Mode == UDMA_MODE_STOP)
			{  
						if(app_audio_get_audio_data_length() >= PLAY_I2S_DMA_SIZE*sizeof(I2S_DMA_SIZE_TYPE))
						{
								cobra_ring_buffer_get_multiple(&play_ring_buffer, (unsigned char *)play_buffer_right, PLAY_I2S_DMA_SIZE*sizeof(I2S_DMA_SIZE_TYPE));
								cobra_udma_set_channel_transfer(i2s_play_dma_channel | UDMA_ALT_SELECT,
																	 UDMA_MODE_PINGPONG,
																	 play_buffer_right,
																	 (void *)cobra_i2s_get_m3_i2s_write_address(I2S0), PLAY_I2S_DMA_SIZE);
						}
						else
						{
								//printf("no Data 222...\n");
								//memset(play_buffer_right, 0, I2S_DMA_SIZE*sizeof(I2S_DMA_SIZE_TYPE));
								cobra_udma_set_channel_transfer(i2s_play_dma_channel | UDMA_ALT_SELECT,
																	 UDMA_MODE_PINGPONG,
																	 zero_buffer,
																	 (void *)cobra_i2s_get_m3_i2s_write_address(I2S0), PLAY_I2S_DMA_SIZE);
							
						}
    }

		
}
#endif


void app_voice_adpcm_decode_sound(const unsigned char * pValue, unsigned int length)
{
			int ret ;
			
		  int16_t pv_rx = BUILD_UINT16(pValue[2], pValue[3]);
	
			int8_t  si_rx = pValue[1];
	
			Codec1_decodeBuff((int16_t *)decode_bufferOut, (uint8_t *)&pValue[4], \
													 ADPCM_DECODE_SAMPLE,
													 &si_rx, &pv_rx);
	
			if(app_audio_get_empty_buffer_length() >= 4*ADPCM_DECODE_SAMPLE)
			{
					app_audio_put_audio_data_to_buffer((unsigned char *)decode_bufferOut, ADPCM_DECODE_SAMPLE*4);
			}	
			else
			{
					printf("play two slow ...... \n");
			}
}


#endif
