/* 
 liujian add
 2018/7/3
 version 1.0 
 
 建立一个 任务来处理 I2S 接收到的数据

*/

#if BLE_APP_RDW_VOICE
#include "rwip_config.h"  
#include "cobra_i2c.h"
#include "voice_task.h"
#include <stdio.h>
#include "ke_task.h"      // kernel task
#include "ke_event.h"     // kernel event
#include "ke_timer.h"     // kernel timer defines


static void i2s_dma_callback(void);

extern void voice_dma_task(void);


static void void_dma_callback(void)
{

    ke_event_clear(KE_EVENT_VOICE);
		voice_dma_task();
	
}

void rdw_voice_task_init(void)
{
	// Create VOICE kernel event
    ke_event_callback_set(KE_EVENT_VOICE, &void_dma_callback);
	
	
}

#endif



