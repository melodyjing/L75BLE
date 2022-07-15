/******************************************************************************
	File Name	:	app_voice.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/10

	Description :   

	Changed Log	:
		Liujian  2020/03/10			- Creation

*****************************************************************************/
#ifndef __APP_VOICE_H__
#define __APP_VOICE_H__

#include "cobra_wm8978.h"
#include "cobra_i2c.h"
#include "cobra_i2s.h"
#include "cobra_system.h"
#include "rdw_dma.h"
#include <stdint.h>
#include "adpcm_code.h"
#include "cobra_ringbuffer.h"

#if BLE_APP_RECEIVE_VOICE

#define USE_BASIC_DMA_PLAY_VOICE				1   		//使用 BASIC DMA 来播放声音
#define USE_PINGPONG_DMA_PLAY_VOICE			2       //使用 pingpong DMA 来播放声音

#define I2S_PLAY_DMA_WAY						USE_PINGPONG_DMA_PLAY_VOICE

#define ADPCM_DECODE_SAMPLE     196

//这里只是为了方便改动而定义的宏
#define I2S_DMA_SIZE_TYPE		int16_t

#define I2S_BUFFER_NUMBER			6
#define PLAY_I2S_DMA_SIZE         	512  			//ADPCM_DECODE_SAMPLE    //512


struct  i2s_buf_elt
{
		struct co_list_hdr hdr;
	
		I2S_DMA_SIZE_TYPE * buf_ptr;
};


#define BUILD_UINT16(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))


void app_voice_adpcm_decode_sound(const unsigned char * param, unsigned int length);

int app_voice_send_data_to_wm8978(int16_t* sample_value, unsigned int length);

void app_audio_drv_init(void);

void app_voice_play_dma_task(void);

#endif

#endif
