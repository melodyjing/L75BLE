/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */
 
#ifndef _COBRA_MULTI_TIMER_H_
#define _COBRA_MULTI_TIMER_H_

#include "stdint.h"
#include "stddef.h"
#include <cobra_config.h>
#if (COBRA_MULTI_TIMER_ENABLE == 1)
typedef struct cobra_timer_Type 
{
    uint32_t timeout;
    uint32_t repeat;
    void (*timeout_cb)(void);
    struct cobra_timer_Type* next;
}cobra_timer;

#ifdef __cplusplus  
extern "C" {  
#endif  

void cobra_softtimer_init(struct cobra_timer_Type* handle, void(*timeout_cb)(), uint32_t timeout, uint32_t repeat);
int  cobra_softtimer_start(struct cobra_timer_Type* handle);
void cobra_softtimer_stop(struct cobra_timer_Type* handle);
void cobra_softtimer_ticks(void);
void cobra_softtimer_loop();
void app_cobra_link_soft_timer_init(void);
// void timer_again(struct Timer* handle);
// void timer_set_repeat(struct Timer* handle, uint32_t repeat);

#ifdef __cplusplus
} 
#endif

#endif

#endif
