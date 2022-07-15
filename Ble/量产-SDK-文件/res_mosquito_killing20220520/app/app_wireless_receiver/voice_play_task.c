/******************************************************************************
	File Name	:	voice_play_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/10

	Description :   

	Changed Log	:
		Liujian  2020/03/10			- Creation

*****************************************************************************/

#include "voice_play_task.h"
#include "app_voice.h"
#if BLE_APP_RECEIVE_VOICE

static void void_dma_callback(void)
{
#if (I2S_PLAY_DMA_WAY == USE_PINGPONG_DMA_PLAY_VOICE)
		ke_event_clear(KE_EVENT_I2S_PLAY);
		app_voice_play_dma_task();
#endif	
}

void app_voice_play_task_init(void)
{
#if (I2S_PLAY_DMA_WAY == USE_PINGPONG_DMA_PLAY_VOICE)
		// Create VOICE kernel event
		ke_event_callback_set(KE_EVENT_I2S_PLAY, &void_dma_callback);
#endif
	
}


#endif
