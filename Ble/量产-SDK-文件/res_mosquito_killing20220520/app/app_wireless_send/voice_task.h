/*
liujian
2018/7/3
version 1.0
建立一个 VOICE 任务, 来处理 I2S 接收到的数据

*/

#ifndef __VOICE_TASK__
#define __VOICE_TASK__

#include "rdw_mcu.h"
#include "cobra_i2c.h"

//struct i2s_env_tag{
//	
//	uint32_t  dma_alt_or_master;
//	
//	
//};

void rdw_voice_task_init(void);
void i2s_task_set_dma_mode(uint32_t mode);


#endif 
