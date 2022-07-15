/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */
 /*
   Confidential and copyright 2014-2021 Radiawave Ltd.
	
	 Author:Liujian
	 
	 2020/11/20
 
 */

#include "cobra_multi_timer.h"
#include "cobra4_systick.h"
#include "rdw_mcu.h"

#if (COBRA_MULTI_TIMER_ENABLE == 1)
//timer handle list head.
static struct cobra_timer_Type* cobra_timer_head_handle = NULL;

//Timer ticks
static uint32_t soft_timer_ticks = 0;

/**
  * @brief  Initializes the timer struct handle.
  * @param  handle: the timer handle strcut.
  * @param  timeout_cb: timeout callback.
  * @param  repeat: repeat interval time.
  * @retval None
  */
void cobra_softtimer_init(struct cobra_timer_Type* handle, void(*timeout_cb)(), uint32_t timeout, uint32_t repeat)
{
	// memset(handle, sizeof(struct Timer), 0);
	handle->timeout_cb = timeout_cb;
	handle->timeout = soft_timer_ticks + timeout;
	handle->repeat = repeat;
}

/**
  * @brief  Start the timer work, add the handle into work list.
  * @param  btn: target handle strcut.
  * @retval 0: succeed. -1: already exist.
  */
int cobra_softtimer_start(struct cobra_timer_Type* handle)
{
	struct cobra_timer_Type* target = cobra_timer_head_handle;
	while(target) 
	{
		if(target == handle) return -1;	//already exist.
		  target = target->next;
	}
	handle->next = cobra_timer_head_handle;
	cobra_timer_head_handle = handle;
	return 0;
}

/**
  * @brief  Stop the timer work, remove the handle off work list.
  * @param  handle: target handle strcut.
  * @retval None
  */
void cobra_softtimer_stop(struct cobra_timer_Type* handle)
{
	struct cobra_timer_Type** curr;
	for(curr = &cobra_timer_head_handle; *curr; ) 
	{
		struct cobra_timer_Type* entry = *curr;
		if (entry == handle) 
		{
			*curr = entry->next;
		} 
		else
			curr = &entry->next;
	}
}

/**
  * @brief  main loop.
  * @param  None.
  * @retval None
  */
void cobra_softtimer_loop()
{
	struct cobra_timer_Type* target;

	for(target=cobra_timer_head_handle; target; target=target->next) 
	{
		 if(soft_timer_ticks >= target->timeout) 
		 {
			  if(target->repeat == 0) 
			  {
				     cobra_softtimer_stop(target);
			  }  
			  else 
			  {
				     target->timeout = soft_timer_ticks + target->repeat;
			  }
			  target->timeout_cb();
		  }
	}
}

/**
  * @brief  background ticks, timer repeat invoking interval 1ms.
  * @param  None.
  * @retval None.
  */
void cobra_softtimer_ticks()
{
	  soft_timer_ticks++;
}

void app_cobra_link_soft_timer_init(void)
{
   // Initialize the Timer Registers

   cobra4_systick_init(KERNEL_SYSTICK_HZ, cobra_softtimer_ticks);
}
#endif

