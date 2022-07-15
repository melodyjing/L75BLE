/******************************************************************************
	File Name	:	app_i2s_record.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/3/14

	Description :   

	Changed Log	:
		Liujian  2020/3/14		- Creation

*****************************************************************************/

#include "app_i2s_record.h"
#include "cobra_system.h"

#if((USE_WM8976_SAMPLE) && (BLE_APP_RDW_VOICE))

static struct adpcm_stream   adpcm_env;

static char  i2s0_dma_channel;

static uint8_t seqNum;

#if (TEST_LOCAL_SOUND)	
//Header 0xAA 0x55
//4 sequnumber + si + vi
static uint8_t adpcm_encode_buffer[((I2S_DMA_SIZE>>1)+4+2)];   //压缩后ADPCM 放在这里的buffer
#else
//如果不是本地播放,发送到Ｄｏｎｇｌｅ　去播放，定义如下
//没有 0xAA 0x55
static uint8_t adpcm_encode_buffer[((I2S_DMA_SIZE>>1)+4)];  

#endif


void app_adpcm_init(void)
{
			adpcm_env.pv = 0;
			adpcm_env.si = 0;
			adpcm_env.frame_length = I2S_DMA_SIZE;
	
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

void app_i2s_record_pin_config(void)
{
		cobra_map_pinmodeset(PIN0_INDEX, PIN0_I2C0_SCL);
		cobra_map_pinmodeset(PIN1_INDEX, PIN1_I2C0_SDA);
	
		cobra_pin_pullup_enable(PIN0_INDEX, 1);
		cobra_pin_pullup_enable(PIN1_INDEX, 1);
	
		cobra_map_pinmodeset(PIN14_INDEX, PIN14_I2SPCM0_RX_BITIN);
		cobra_map_pinmodeset(PIN12_INDEX, PIN12_I2SPCM0_TX_BITOUT);
	
		cobra_map_pinmodeset(PIN8_INDEX,  PIN8_I2S0PCM_TX_OE);
		cobra_map_pinmodeset(PIN5_INDEX,  PIN5_I2S0_TX_SCLK);
		cobra_map_pinmodeset(PIN27_INDEX, PIN27_I2S0_TX_MCLK);	
	
//		cobra_map_pinmodeset(PIN28_INDEX, PIN28_I2S0_RX_MCLK);
//		cobra_map_pinmodeset(PIN7_INDEX,  PIN7_I2S0_RX_SCLK);
//		cobra_map_pinmodeset(PIN9_INDEX,  PIN9_I2SPCM0_RX_OE);
//	
//		cobra_pin_drain_enable(PIN5_INDEX ,1);
}


void app_i2s_record_clock_init(void)
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
	
		i2s_env.i2s_is_master = true;
		i2s_env.sample_rate = 16;     //16k
		i2s_env.bit_num = 16;         //16bit, 8bit .....
		i2s_env.index = I2S0;
		i2s_env.clk_in_khz = 48000;    //16M
		i2s_env.m_s_div = 8;
		i2s_env.sound_mode = I2S_RECORD;
		i2s_env.sample_ponit = 32;
		app_cobra_i2s_set_clock_mode(&i2s_env);
	
	
}

static void app_i2s_tx_rx_init(i2s_id index, sample_word_width width, sample_point point)
{
			
			app_cobra_cfg_txrx_common_mode_en(index, 1);
	
			cobra_i2s_rx_init(index, I2S_MODE, width, point, DMA_ENABLE);
			cobra_i2s_rx_enable(index, I2S_MASTER);
	
			cobra_i2s_tx_init(index, I2S_MODE, width, point, DMA_ENABLE);
			cobra_i2s_tx_enable(index, I2S_MASTER);
	
			cobra_i2s_invert_lrclk(index, 1);
	
			cobra_i2s_begin_work(index);
	
		
}



//I2S DMA 中断回调函数, 只是把事件标志设立起来
void i2s_set_dma_callback(void)
{
			ke_event_set(KE_EVENT_VOICE);
	
			cobra_udma_clear_interrupt(i2s0_dma_channel);
	
}

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

void app_i2s_record_dma_init(void)
{
			I2S_DMA_SIZE_TYPE * p_buffer; 
	
			i2s0_dma_channel = app_rdw_udma_get_channel(CHANNEL14, CH14_I2S0_RX);
	
			patch_dma_function();
			
      cobra_udma_channel_attribute_disable(i2s0_dma_channel,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK);

	
      cobra_udma_set_channel_control(i2s0_dma_channel | UDMA_PRI_SELECT,
                              UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 |
                              UDMA_ARB_1);

      cobra_udma_set_channel_control(i2s0_dma_channel | UDMA_ALT_SELECT,
                              UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 |
                              UDMA_ARB_1);

	
			
			p_buffer = i2s_buf_rx_alloc();
			if(p_buffer != NULL)
			{
					cobra_udma_set_channel_transfer(i2s0_dma_channel | UDMA_PRI_SELECT,
																		 UDMA_MODE_PINGPONG,
																		 (void *)cobra_i2s_get_m3_i2s_read_address(I2S0),
																		 p_buffer, I2S_DMA_SIZE);
				
			}
			else
			{
					printf("No buffer \n");
			}
			
		
			p_buffer = i2s_buf_rx_alloc();
			if(p_buffer != NULL)
			{
					cobra_udma_set_channel_transfer(i2s0_dma_channel | UDMA_ALT_SELECT,
																		 UDMA_MODE_PINGPONG,
																		 (void *)cobra_i2s_get_m3_i2s_read_address(I2S0),
																		 p_buffer, I2S_DMA_SIZE);
			}
			else
			{
					printf("No buffer \n");
			}
			
//			cobra_udma_channel_attribute_disable(i2s0_dma_channel,
//                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
//                                    UDMA_ATTR_HIGH_PRIORITY |
//                                    UDMA_ATTR_REQMASK);
			
			cobra_udma_enable_channel(i2s0_dma_channel);
													 
			app_rdw_udma_register_callback(i2s0_dma_channel, i2s_set_dma_callback);

			cobra_udma_enable_interrupt(i2s0_dma_channel);		
		
			NVIC_ClearPendingIRQ(DMADONE_IRQn);
			
			NVIC_EnableIRQ(DMADONE_IRQn); 
			
	
}

void  app_i2s_record_init(void)
{				
			unsigned int apb_i2c_clk;
	
			
	
			printf("app_i2s_record_init ...... \n");
	
			cobra_pcrm_i2s0_soft_reset();
	
			app_voice_init_buffer();
	

			cobra_pcrm_i2c0_active_mode_clock_enable(1);   //I2C0 clock enable
			cobra_pcrm_i2c0_sleep_mode_clock_enable(1);
	
			app_i2s_record_pin_config();
	
			apb_i2c_clk = cobra_system_get_m3_apb1_clk();
		
			app_m3_i2c_init();
	
			cobra_i2c_set_bit_rate(M3_I2C0, apb_i2c_clk, 200000);    //i2c 200Khz
	
			cobra_i2c_maste_enable(M3_I2C0);
	
			cobra_pcrm_i2s0_active_mode_clock_enable(1);   //I2S0 clock enable
			cobra_pcrm_i2s0_sleep_mode_clock_enable(1);
	
#if 0
			cobra_wm8978_init();
			
			app_i2s_record_clock_init();  //设置 record I2S  的时钟相关的内容
#else

			cobra_wm8978_init_pll();
			
			app_cobra_i2s_clk_config(I2S0, CLK_PLL, 32, SAMPLE_WORD_16BIT, SAMPLE_16BIT);
#endif

			app_i2s_record_dma_init();

			app_i2s_tx_rx_init(I2S0, SAMPLE_WORD_16BIT, SAMPLE_16BIT);
			
//			cobra_pcrm_i2s0_tx_sclk_phase(PHASE_180);			
//			cobra_pcrm_i2s0_rx_sclk_phase(PHASE_180);
		

			cobra_wm8978_adda_cfg(DAC_DISABLE, ADC_EN);
	
			cobra_wm8978_input_cfg(MIC_DISABLE, LINEIN_EN, AUX_DISABLE);
		
			cobra_wm8978_i2s_cfg(FMT_I2S, WL_16BITS, STERO_EN);     // 最后一位   , 0 单声道, 1 立体	
				
			cobra_wm8978_mic_gain(0);
			
			cobra_wm8978_linein_gain(L2_2BOOSTVOL_N9DB);
			
	//		cobra_wm8978_MIC_mute(0);
}


 //voice 事件处理函数
void voice_dma_task(void)
{
	
		  uint32_t ui32Status;
			uint32_t ui32Mode;
	
			uint32_t i ;
	
			uint8_t si_local = adpcm_env.si;
      uint16_t pv_local = adpcm_env.pv;

	

			I2S_DMA_SIZE_TYPE * p_buffer_alloc; 
			I2S_DMA_SIZE_TYPE * p_buffer_free; 
	

     ui32Mode = cobra_udma_get_channel_mode(i2s0_dma_channel | UDMA_PRI_SELECT);


		
    if(ui32Mode == UDMA_MODE_STOP)
    {
			

				p_buffer_free = cobra_udma_get_channel_dst_buffer(i2s0_dma_channel | UDMA_PRI_SELECT, I2S_DMA_SIZE);
		
				p_buffer_alloc = i2s_buf_rx_alloc();

				if(p_buffer_alloc != NULL)
				{
					

						cobra_udma_set_channel_transfer(i2s0_dma_channel | UDMA_PRI_SELECT,
																			 UDMA_MODE_PINGPONG,
																			 (void *)cobra_i2s_get_m3_i2s_read_address(I2S0),
																			 p_buffer_alloc, I2S_DMA_SIZE);
				}
				else
				{
						printf("No buffer 1\n");
				}
					
#if (TEST_LOCAL_SOUND)	
								
				cobra_uart_write(M3_UART0, 0xAA);
				cobra_uart_write(M3_UART0, 0x55);
				app_cobra_urat_print_hex(M3_UART0, (int16_t *)p_buffer_free, adpcm_env.frame_length*2);	
				Codec1_encodeBuff(&adpcm_encode_buffer[6], (int16_t *)p_buffer_free, adpcm_env.frame_length, &adpcm_env.si, &adpcm_env.pv);
				
				adpcm_encode_buffer[0] = 0xAA;
				adpcm_encode_buffer[1] = 0x55;
				adpcm_encode_buffer[2] = seqNum++;
         // Send previous PV and SI
        adpcm_encode_buffer[3] = si_local;
				adpcm_encode_buffer[4] = LO_UINT16(pv_local);
				adpcm_encode_buffer[5] = HI_UINT16(pv_local);
		 		app_cobra_urat_print_hex(M3_UART0, adpcm_encode_buffer, ((I2S_DMA_SIZE>>1)+6));
				i2s_buf_rx_free(p_buffer_free);
#else
				Codec1_encodeBuff(&adpcm_encode_buffer[4], (int16_t *)p_buffer_free, adpcm_env.frame_length, &adpcm_env.si, &adpcm_env.pv);
		
				adpcm_encode_buffer[0] = seqNum++;
         // Send previous PV and SI
        adpcm_encode_buffer[1] = si_local;
				adpcm_encode_buffer[2] = LO_UINT16(pv_local);
				adpcm_encode_buffer[3] = HI_UINT16(pv_local);
				//通过蓝牙发送过去
				i2s_buf_rx_free(p_buffer_free);
				
				struct voice_data_req * req = KE_MSG_ALLOC(VOICE_AUDIO_DATA_REQ,
                                                    prf_get_task_from_id(TASK_ID_VOICE),
                                                    TASK_APP,
                                                    voice_data_req);
				req->length = ((I2S_DMA_SIZE>>1)+4);
				
				memcpy(req->audio_data, adpcm_encode_buffer, req->length); 
				ke_msg_send(req);
				
				
#endif				

				
		
			

    }


    ui32Mode = cobra_udma_get_channel_mode(i2s0_dma_channel | UDMA_ALT_SELECT);

		
    if(ui32Mode == UDMA_MODE_STOP)
    {

    
  				p_buffer_free = cobra_udma_get_channel_dst_buffer(i2s0_dma_channel | UDMA_ALT_SELECT, I2S_DMA_SIZE);
	
  				p_buffer_alloc = i2s_buf_rx_alloc();
				
				if(p_buffer_alloc != NULL)
				{
					

						cobra_udma_set_channel_transfer(i2s0_dma_channel | UDMA_ALT_SELECT,
																			 UDMA_MODE_PINGPONG,
																			 (void *)cobra_i2s_get_m3_i2s_read_address(I2S0),
																			 p_buffer_alloc, I2S_DMA_SIZE);

																			 
																		 
					//	uDMAChannelEnable(i2s0_dma_channel);
				} 
				else
				{
						printf("No buffer 2\n");
				}

#if (TEST_LOCAL_SOUND)			
			
				cobra_uart_write(M3_UART0, 0xAA);
				cobra_uart_write(M3_UART0, 0x55);
				app_cobra_urat_print_hex(M3_UART0, (int16_t *)p_buffer_free, adpcm_env.frame_length*2);
				
				Codec1_encodeBuff(&adpcm_encode_buffer[6], (int16_t *)p_buffer_free, adpcm_env.frame_length, &adpcm_env.si, &adpcm_env.pv);
				
				adpcm_encode_buffer[0] = 0xAA;
				adpcm_encode_buffer[1] = 0x55;
				adpcm_encode_buffer[2] = seqNum++;
         // Send previous PV and SI
        adpcm_encode_buffer[3] = si_local;
				adpcm_encode_buffer[4] = LO_UINT16(pv_local);
				adpcm_encode_buffer[5] = HI_UINT16(pv_local);	
		  	app_cobra_urat_print_hex(M3_UART0, adpcm_encode_buffer, ((I2S_DMA_SIZE>>1)+6));
				i2s_buf_rx_free(p_buffer_free);
#else
				Codec1_encodeBuff(&adpcm_encode_buffer[4], (int16_t *)p_buffer_free, adpcm_env.frame_length, &adpcm_env.si, &adpcm_env.pv);
				adpcm_encode_buffer[0] = seqNum++;
         // Send previous PV and SI
        adpcm_encode_buffer[1] = si_local;
				adpcm_encode_buffer[2] = LO_UINT16(pv_local);
				adpcm_encode_buffer[3] = HI_UINT16(pv_local);	
				//通过蓝牙发送过去
				
				i2s_buf_rx_free(p_buffer_free);
				
				struct voice_data_req * req = KE_MSG_ALLOC(VOICE_AUDIO_DATA_REQ,
                                                    prf_get_task_from_id(TASK_ID_VOICE),
                                                    TASK_APP,
                                                    voice_data_req);
				req->length = ((I2S_DMA_SIZE>>1)+4);
				
				memcpy(req->audio_data, adpcm_encode_buffer, req->length); 
				ke_msg_send(req);

#endif				
				
		
    }
}




#endif

