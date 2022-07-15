/******************************************************************************
	File Name	:	voice_play_task.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/10

	Description :   

	Changed Log	:
		Liujian  2020/03/10			- Creation

*****************************************************************************/

#ifndef __VOICE_PLAY_TASK_H__
#define __VOICE_PLAY_TASK_H__
#include "rwip_config.h"  
#include "cobra_i2c.h"
#include <stdio.h>
#include "ke_task.h"      // kernel task
#include "ke_event.h"     // kernel event
#include "ke_timer.h"     // kernel timer defines



void app_voice_play_task_init(void);
#endif
