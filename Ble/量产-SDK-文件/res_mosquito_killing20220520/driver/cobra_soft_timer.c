/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_soft_timer.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/5/5

	Description :   


	Changed Log	:

		Liujian  2019/5/5			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>
#include <string.h>
#include "rdw_mcu.h"
#include "cobra_soft_timer.h"
#include "cobra_pcrm.h"
#include "cobra_timer.h"

#if COBRA_SOFT_TIMER_EN

#if (COBRA_POWER_DOWN_ENABLE == 1)
static soft_timer_t cobra_timer_table[MAX_SOFT_TIMERS] __attribute__((section("retention_area"),zero_init));
#else
static soft_timer_t cobra_timer_table[MAX_SOFT_TIMERS];
#endif

void cobra_soft_timer_init(void)
{
    memset(&cobra_timer_table, 0, sizeof(cobra_timer_table));
}


/**
 * @fn int32_t cobra_soft_timer_create(uint32_t timeout, func_pnt_t FunctionPointer)
 * @brief Create a software timer.
 * @param FunctionPointer The function to call on timeout.
 * @return The timer id or the error code ERR_TIMER_NOT_AVAILABLE.
 */
int32_t cobra_soft_timer_create(timer_handler_t FunctionPointer)
{
    uint8_t idx;

    idx = 0;
    while ((idx < MAX_SOFT_TIMERS) && (cobra_timer_table[idx].timer_mode & TIMER_CREATE_MASK))
    {
        idx++;
    }
    if (idx == MAX_SOFT_TIMERS)
    {
        idx = -1;
    }
    else
    {
        /* Disable timer interrupt */
        NVIC_DisableIRQ(COBRA4_TIMER0_IRQn);
			
        /* Initialize timer */
        cobra_timer_table[idx].TimerHandler = FunctionPointer;
			
        cobra_timer_table[idx].timer_mode |= TIMER_CREATE_MASK;
			
        /* Enable timer interrupt */
        NVIC_EnableIRQ(COBRA4_TIMER0_IRQn);
				NVIC_SetPriority(COBRA4_TIMER0_IRQn, 1);
    }
    return idx;
}


/**
 * @fn void Timer_Destroy(uint8_t timerId)
 * @brief Destroy the timer passed as parameter
 * @param timerId The id of the timer to destroy.
 */
void cobra_soft_timer_destroy(uint32_t timerId)
{
    if (timerId < MAX_SOFT_TIMERS)
    {
        cobra_timer_table[timerId].timer_mode = 0;
    }
}

/**
 * @fn void cobra_soft_timer_start(uint32_t timerId)
 * @brief Start the timer.
 * @param timerId The id of the timer to start.
 * @param timeout The timeout in tens of milliseconds. Max timeout value is 16383 (14 bits available).
 */

void cobra_soft_timer_start(uint32_t timerId, uint32_t timeout, uint32_t period)
{
    if (timerId < MAX_SOFT_TIMERS)
    {
        /* Disable timer interrupt */
        NVIC_DisableIRQ(COBRA4_TIMER0_IRQn);
        /* Set the timer active flag */
        cobra_timer_table[timerId].timer_out_tick = timeout;
				cobra_timer_table[timerId].timer_mode |= TIMER_ACTIVER_MASK;
				if(period)
				{
						cobra_timer_table[timerId].timer_mode |= TIMER_REPEAT_MASK;
						cobra_timer_table[timerId].reload_timer_out_tick = timeout;
				}
				else
				{
						cobra_timer_table[timerId].timer_mode &= ~TIMER_REPEAT_MASK;
				}
        /* Enable timer interrupt */
        NVIC_EnableIRQ(COBRA4_TIMER0_IRQn);
    }
}

/**
 * @fn void cobra_soft_timer_stop(uint32_t timerId)
 * @brief Stop the timer.
 * @param timerId The id of the timer to stop.
 */
void cobra_soft_timer_stop(uint32_t timerId)
{
    if (timerId < MAX_SOFT_TIMERS)
    {
        /* Disable timer interrupt */
        NVIC_DisableIRQ(COBRA4_TIMER0_IRQn);
        /* Reset the timer active flag */
        cobra_timer_table[timerId].timer_mode &= ~TIMER_ACTIVER_MASK;
        /* Enable timer interrupt */
        NVIC_EnableIRQ(COBRA4_TIMER0_IRQn);
				NVIC_SetPriority(COBRA4_TIMER0_IRQn, 1);
    }
}


/**
 * @fn void cobra_soft_timer_update(void)
 * @brief Update active timers. Note that it's called in interrupt contest.
 */
void cobra_soft_timer_update(void)
{
    uint8_t idx;

    for (idx=0; idx<MAX_SOFT_TIMERS; idx++)
    {
        if (cobra_timer_table[idx].timer_mode & TIMER_ACTIVER_MASK)
        {
            cobra_timer_table[idx].timer_out_tick--;
            if (cobra_timer_table[idx].timer_out_tick  == 0)
            {
                /* Disable the timer */
								if(cobra_timer_table[idx].timer_mode & TIMER_REPEAT_MASK)
								{
										cobra_timer_table[idx].timer_out_tick = cobra_timer_table[idx].reload_timer_out_tick;
								}
								else
								{
										cobra_timer_table[idx].timer_mode &= ~TIMER_ACTIVER_MASK;
								}
                /* Call the timer handler */
                cobra_timer_table[idx].TimerHandler(idx);
            }
        }
    }
}


void app_cobra_soft_timer_init(void)
{
#if (!APP_SLEEP_MODE_ENABLE)	
		cobra_pcrm_timer0_active_mode_clock_enable(1);
		cobra_pcrm_timer0_sleep_mode_clock_enable(1);

#if (DIANBIAO_MODULE_ENABLE)	
	  app_cobra_timer0_set_callback(NULL);
#endif
	
		cobra_timer_set_frequency(APB2_TIMER0, COBRA_SOFT_TIMER_TICK);    //100hz  timer0   10ms
		cobra_timer_enable_set(APB2_TIMER0, 1);        //Enable timer0
		cobra_timer_interrupt_set(APB2_TIMER0, 1) ;    //Enale interrupt
		
		NVIC_SetPriority(COBRA4_TIMER0_IRQn, 1);
		NVIC_ClearPendingIRQ(COBRA4_TIMER0_IRQn);
		NVIC_EnableIRQ(COBRA4_TIMER0_IRQn);
#endif
	
}

void app_cobra_soft_timer_uninit(void)
{
			NVIC_DisableIRQ(COBRA4_TIMER0_IRQn);
			NVIC_ClearPendingIRQ(COBRA4_TIMER0_IRQn);
			
			cobra_timer_interrupt_set(APB2_TIMER0, 0) ; 
			cobra_timer_enable_set(APB2_TIMER0, 0); 
	
}



#endif














