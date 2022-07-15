/******************************************************************************
	File Name	:	app_pdm_record.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/3/14

	Description :   

	Changed Log	:
		Liujian  2020/3/14		- Creation

*****************************************************************************/

#include "app_pdm_record.h"

#if((USE_PDM_SAMPLE) && BLE_APP_RDW_VOICE)

static struct adpcm_stream   adpcm_env;

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


void pdm_dma_callback(void)
{
		ke_event_set(KE_EVENT_VOICE);
		cobra_udma_clear_interrupt(CHANNEL2);
//	  printf("pdm_dma_callback ..... \n");
}

void app_voice_init_pdm(void)
{
	struct audio_env_tag audio_env;
	
	cobra_pcrm_pdm0_active_mode_clock_enable(1);
	
	cobra_map_pinmodeset(PIN31_INDEX, PIN31_PDM_LCLK);
	cobra_map_pinmodeset(PIN29_INDEX, PIN29_PDM0_PDM);

	cobra_map_pinmodeset(PIN28_INDEX, PIN28_PDM_RCLK);
	cobra_map_pinmodeset(PIN30_INDEX, PIN30_PDM1_PDM);
	
// if(clocksource == 0)
//    {
//        src_pdm_init(0,15.625,12,PDM_OSR_128,1,48000,0,1);
//    }else
//    {
//        src_pdm_init(1,16,12,PDM_OSR_128,1,24576,1,1);
//    }
//		void src_pdm_init(uint8_t clocksource,double _1fs_clk_kHz,
	// uint8_t out_bitwidth,uint16_t osr,uint8_t dma_en,double pclk_kHz,uint8_t bypass_hbnum,uint8_t to_i2s)
#if 1
	audio_env.agc_enable = 0;
	audio_env.clock_source = MCLK_FROM_PLL;
	audio_env.bypass_hbnum = 1;
//	audio_env.dma_en = 1;
	audio_env.i2s_enable = 0;
	audio_env.sample_1fs_clk_kHz = 16;
	audio_env.out_bitwidth = 12;
	audio_env.osr = PDM_OSR_128;
	audio_env.pclk_khz = 24576;
	app_audio_pdm_init(&audio_env);
#else	
	audio_env.agc_enable = 0;
	audio_env.clock_source = MCLK_FROM_XTAL;
	audio_env.bypass_hbnum = 0;
//	audio_env.dma_en = 1;
	audio_env.i2s_enable = 0;
	audio_env.sample_1fs_clk_kHz = 15.625;
	audio_env.out_bitwidth = 12;
	audio_env.osr = PDM_OSR_128;
	audio_env.pclk_khz = 48000;
	app_audio_pdm_init(&audio_env);
	
	app_cobra_audio_disable();
#endif

	
//	app_cobra_pdm_init(PDM0, 1, 128, 24000);
	
//	rdw_pdm_init(1,PDM_OSR_128,16000);
	
//	pdm_to_i2s_init(pcm_clk,16000,PDM_OSR_128);
}


static char     pdm_dma_channel;

void app_pdm_record_init_dma(void)
{
		I2S_DMA_SIZE_TYPE * p_buffer; 

		app_voice_init_buffer();
	
	//	rdw_pdm_disable();
		app_cobra_audio_disable();
//		app_rdw_udma_register_callback(pdm_dma_channel, NULL);
		
		pdm_dma_channel = app_rdw_udma_get_channel(CHANNEL2, CH2_PDM0);
		
		cobra_udma_channel_attribute_disable(pdm_dma_channel,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK);


    cobra_udma_set_channel_control(pdm_dma_channel | UDMA_PRI_SELECT,
                              UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 |
                              UDMA_ARB_1);

    cobra_udma_set_channel_control(pdm_dma_channel | UDMA_ALT_SELECT,
                              UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 |
                              UDMA_ARB_1);


		p_buffer = i2s_buf_rx_alloc();
		if(p_buffer != NULL)
		{
				cobra_udma_set_channel_transfer(pdm_dma_channel | UDMA_PRI_SELECT,
																	 UDMA_MODE_PINGPONG,
																	 (void *)cobra4_audio_read_upfifo_address(),
																	 p_buffer, I2S_DMA_SIZE);
		}
  
		p_buffer = i2s_buf_rx_alloc();
		if(p_buffer != NULL)
		{
				cobra_udma_set_channel_transfer(pdm_dma_channel | UDMA_ALT_SELECT,
																	 UDMA_MODE_PINGPONG,
																	 (void *)cobra4_audio_read_upfifo_address(),
																	 p_buffer, I2S_DMA_SIZE);
		}


    cobra_udma_enable_channel(pdm_dma_channel);
													 
		app_rdw_udma_register_callback(pdm_dma_channel, pdm_dma_callback);
															 
		app_voice_init_pdm();
															 
		cobra_udma_enable_interrupt(CHANNEL2);	
		
		NVIC_ClearPendingIRQ(DMADONE_IRQn);
		
		NVIC_EnableIRQ(DMADONE_IRQn); 
		

}

void app_pdm_record_init(void)
{
			app_cobra_audio_enable();
			app_rdw_udma_register_callback(pdm_dma_channel, pdm_dma_callback);
			cobra_udma_enable_interrupt(CHANNEL2);	
	
}

void app_pdm_record_uninit(void)
{
		app_rdw_udma_register_callback(pdm_dma_channel, NULL);
		app_cobra_audio_disable();
		cobra_udma_disable_interrupt(CHANNEL2);
	
}

//voice 事件处理函数
void voice_dma_task(void)
{
	
//		  uint32_t ui32Status;
			uint32_t ui32Mode;
	
//			uint32_t i ;
	
			uint8_t si_local =  adpcm_env.si;
      uint16_t pv_local = adpcm_env.pv;

	

			I2S_DMA_SIZE_TYPE * p_buffer_alloc; 
			I2S_DMA_SIZE_TYPE * p_buffer_free; 
	


	  ui32Mode = cobra_udma_get_channel_mode(pdm_dma_channel | UDMA_PRI_SELECT);

		
		
//		cobra_pdm_debug();
		
    if(ui32Mode == UDMA_MODE_STOP)
    {
			

			  p_buffer_free = cobra_udma_get_channel_dst_buffer(pdm_dma_channel | UDMA_PRI_SELECT, I2S_DMA_SIZE);
			
			
				p_buffer_alloc = i2s_buf_rx_alloc();

				if(p_buffer_alloc != NULL)
				{
					


						cobra_udma_set_channel_transfer(pdm_dma_channel | UDMA_PRI_SELECT,
																			 UDMA_MODE_PINGPONG,
																			 (void *)cobra4_audio_read_upfifo_address(),
																			 p_buffer_alloc, I2S_DMA_SIZE);
				}
				else
				{
						printf("No buffer 1\n");
				}
					
#if (TEST_LOCAL_SOUND)	
								
//				cobra_uart_write(M3_UART0, 0xAA);
//				cobra_uart_write(M3_UART0, 0x55);
//				app_cobra_urat_print_hex(M3_UART0, (int16_t *)p_buffer_free, adpcm_env.frame_length*2);	
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


		

		ui32Mode = cobra_udma_get_channel_mode(pdm_dma_channel | UDMA_ALT_SELECT);


		
    if(ui32Mode == UDMA_MODE_STOP)
    {



				p_buffer_free = cobra_udma_get_channel_dst_buffer(pdm_dma_channel | UDMA_ALT_SELECT, I2S_DMA_SIZE);

		
  				p_buffer_alloc = i2s_buf_rx_alloc();
				
				if(p_buffer_alloc != NULL)
				{
					

																			 

				cobra_udma_set_channel_transfer(pdm_dma_channel | UDMA_ALT_SELECT,
																			 UDMA_MODE_PINGPONG,
																			  (void *)cobra4_audio_read_upfifo_address(),
																			 p_buffer_alloc, I2S_DMA_SIZE);
			
	//	  cobra_udma_enable_channel(pdm_dma_channel);
																		 
					//	uDMAChannelEnable(i2s0_dma_channel);
				} 
				else
				{
						printf("No buffer 2\n");
				}

#if (TEST_LOCAL_SOUND)			
			
//				cobra_uart_write(M3_UART0, 0xAA);
//				cobra_uart_write(M3_UART0, 0x55);
//				app_cobra_urat_print_hex(M3_UART0, (int16_t *)p_buffer_free, adpcm_env.frame_length*2);
				
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
