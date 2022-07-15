//liujian add 
//2019/5/14

#ifndef __APP_SYSTEM_TRIM_H__
#define __APP_SYSTEM_TRIM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if COBRA_TRIM_EN	
void cobra_app_trim_task_loop(void);

void app_trim_init(void);
#endif	
	
#ifdef __cplusplus
}
#endif

#endif 
