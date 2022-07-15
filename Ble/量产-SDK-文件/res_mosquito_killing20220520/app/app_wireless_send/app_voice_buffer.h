/******************************************************************************
	File Name	:	app_voice_buffer.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/3/14

	Description :   
				Buffer 管理
	Changed Log	:
		Liujian  2020/3/14		- Creation

*****************************************************************************/

#ifndef __APP_VOICE_BUFFER_H__
#define __APP_VOICE_BUFFER_H__
#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"
#include "rdw_voice_task.h"
#include <string.h>
#include "app_rdw_voice.h"                  
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "rdw_voice_task.h"             
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions

#include "co_list.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define USE_PDM_SAMPLE			1

#define USE_WM8976_SAMPLE		0

//定义在本地通过串口来测试录音数据
#define TEST_LOCAL_SOUND		0


#define I2S_DMA_SIZE         	392                        //196*2

#define I2S_BUFFER_NUMBER			6

//这里只是为了方便改动而定义的宏
#define I2S_DMA_SIZE_TYPE		int16_t

struct  i2s_buf_elt
{
		struct co_list_hdr hdr;
	
		I2S_DMA_SIZE_TYPE * buf_ptr;
};

struct adpcm_stream
{
		int8_t 		si;   
		int16_t 	pv;
		int16_t	  frame_length;
		
};


void app_voice_init_buffer(void);

I2S_DMA_SIZE_TYPE * i2s_buf_rx_alloc(void);

void i2s_buf_rx_free(I2S_DMA_SIZE_TYPE * buf);

#endif
