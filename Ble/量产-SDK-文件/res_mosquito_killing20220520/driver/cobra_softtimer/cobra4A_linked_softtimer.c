/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4A_linked_softtimer.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/11

	Description :   
     链表式 软件定时器, 需要用到 linked Library

	Changed Log	:

		Liujian  2020/8/11 		- Creation

*****************************************************************************/
#include "cobra4A_linked_softtimer.h"
#include "cobra4A_link.h"
#include <cobra_config.h>
#include "cobra_system.h"
#include "cobra4_systick.h"
#include <stdlib.h>
#if ((COBRA_LINK_LIBRARY_ENABLE == 1) && (COBRA_LINKEN_SOFT_TIMER_ENABLE == 1))

LINKED_LIST   gSoftwareTimerList;
uint32_t      gActiveSoftwareTimers = 0;


static int32_t SoftwareTimerListCompareTo(const void *Data1, const void *Data2)
{
    return (int32_t)(!(Data1 == Data2));
}

static void StopHardwareTimer(void)
{
	 NVIC_DisableIRQ(SysTick_IRQn);	
}
static void StartHardwareTimer(void)
{
	  NVIC_EnableIRQ(SysTick_IRQn);	
}

static void SoftwareTimerTurnedOn(void)
{
    gActiveSoftwareTimers++;

    // check to see if this is the first timer added to the list
    // if so enable the hardware interrupt.
    // This prevents the hardware interrupt from constantly firing when no
    // timers are being used.
    if(gActiveSoftwareTimers == 1)
        StartHardwareTimer();
}

static void SoftwareTimerTurnedOff(void)
{
    gActiveSoftwareTimers--;

    if(gActiveSoftwareTimers == 0)
        StopHardwareTimer();
}

static void cobra_linked_softtimer_callback(void)
{
    if (LinkedListGetSize(&gSoftwareTimerList))
    {
        LinkedListIteratorReset(&gSoftwareTimerList);

        COBRA_SOFTWARE_TIMER *SoftwareTimer = (COBRA_SOFTWARE_TIMER*) LinkedListIteratorNext(&gSoftwareTimerList);

        while (SoftwareTimer != (COBRA_SOFTWARE_TIMER*) NULL)
        {
            if (SoftwareTimer->Active)
            {
                SoftwareTimer->Milliseconds++;

                // if they have a callback, see if the interval is up and call it
                if(SoftwareTimer->Callback != (void(*)(COBRA_SOFTWARE_TIMER*))NULL)
                {
                    if(SoftwareTimer->CallbackInterval == SoftwareTimer->Milliseconds)
                    {
                        SoftwareTimer->Callback(SoftwareTimer);

                        SoftwareTimer->Milliseconds = 0;
                    }
                }
            }

            // get the next timer in the list
            SoftwareTimer = (COBRA_SOFTWARE_TIMER*) LinkedListIteratorNext(&gSoftwareTimerList);
        }
    }
}

/*
初始化, 设置为 sysytick 1000hz
*/
Bool cobra_linked_softtimer_init_timer_lib(void)
{
	   if(!CreateLinkedList(&gSoftwareTimerList, SoftwareTimerListCompareTo, SoftwareTimerMemDealloc))
         return (Bool)FALSE;

     gActiveSoftwareTimers = 0;

	   // Initialize the Timer Registers
     cobra4_systick_init(KERNEL_SYSTICK_HZ, cobra_linked_softtimer_callback);
		 
	   return (Bool)TRUE;
}

COBRA_SOFTWARE_TIMER *cobra_linked_softtimer_create(COBRA_SOFTWARE_TIMER *SoftwareTimer, void(*SoftwareTimerCallback)(COBRA_SOFTWARE_TIMER *SoftwareTimer), uint32_t CallbackInterval)
{
    COBRA_SOFTWARE_TIMER *NewSoftwareTimer;

    if(CallbackInterval == 0 && SoftwareTimerCallback != (void(*)(COBRA_SOFTWARE_TIMER *SoftwareTimer))NULL)
        return (COBRA_SOFTWARE_TIMER*)NULL;

    EnterCritical();

    NewSoftwareTimer = SoftwareTimer;

    if(NewSoftwareTimer == (COBRA_SOFTWARE_TIMER*) NULL)
    {
        NewSoftwareTimer = (COBRA_SOFTWARE_TIMER*)SoftwareTimerMemAlloc(sizeof(COBRA_SOFTWARE_TIMER));

        if (NewSoftwareTimer == (COBRA_SOFTWARE_TIMER*) NULL)
        {
            ExitCritical();

            return (COBRA_SOFTWARE_TIMER*) NULL;
        }
    }
    else
    {
        if(LinkedListContains(&gSoftwareTimerList, (const void*) NewSoftwareTimer, FALSE, 0))
        {
            // the timer is already on the list!

            ExitCritical();

            return (COBRA_SOFTWARE_TIMER*) NULL;
        }
    }

    if(LinkedListAdd(&gSoftwareTimerList, (const void*)NewSoftwareTimer) == FALSE)
    {
        if(SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL)
            SoftwareTimerMemDealloc(NewSoftwareTimer); // this is ok even if they passed in a timer

        ExitCritical();

        return (COBRA_SOFTWARE_TIMER*)NULL;
    }

    NewSoftwareTimer->Milliseconds = 0;
    NewSoftwareTimer->Active = FALSE;
    NewSoftwareTimer->Callback = SoftwareTimerCallback;
    NewSoftwareTimer->CallbackInterval = CallbackInterval;

    ExitCritical();

    return (COBRA_SOFTWARE_TIMER*)NewSoftwareTimer;
}

Bool cobra_linked_softtimer_start(COBRA_SOFTWARE_TIMER *SoftwareTimer)
{
    if(SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL)
	     return (Bool)FALSE;

    if(SoftwareTimer->Active == TRUE)
	     return (Bool)FALSE;

    EnterCritical();

    SoftwareTimer->Active = TRUE;

    SoftwareTimerTurnedOn();

    ExitCritical();

    return (Bool)TRUE;
}

Bool cobra_linked_softtimer_stop(COBRA_SOFTWARE_TIMER *SoftwareTimer)
{
    if(SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL)
    	return (Bool)FALSE;

    if(SoftwareTimer->Active == FALSE)
    	return (Bool)FALSE;

    EnterCritical();

    SoftwareTimer->Active = FALSE;

    SoftwareTimerTurnedOff();

    ExitCritical();

    return (Bool)TRUE;
}

Bool cobra_linked_softtimer_restart(COBRA_SOFTWARE_TIMER *SoftwareTimer)
{
    if(SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL)
        return (Bool)FALSE;

    EnterCritical();

    // only add one if the timer isn't currently active
    if(SoftwareTimer->Active == FALSE)
        SoftwareTimerTurnedOn();

    SoftwareTimer->Active = TRUE;

    SoftwareTimer->Milliseconds = 0;

    ExitCritical();

    return (Bool)TRUE;
}

Bool cobra_linked_softtimer_reset(COBRA_SOFTWARE_TIMER *SoftwareTimer)
{
    if(SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL)
	     return (Bool)FALSE;

    EnterCritical();

    if(SoftwareTimer->Active == TRUE)
        SoftwareTimerTurnedOff();

    SoftwareTimer->Active = FALSE;

    SoftwareTimer->Milliseconds = 0;

    ExitCritical();

    return (Bool)TRUE;
}

Bool cobra_linked_softtimer_delete(COBRA_SOFTWARE_TIMER *SoftwareTimer)
{
    if(SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL)
	      return (Bool)FALSE;

    EnterCritical();

    uint32_t Index = LinkedListContains(&gSoftwareTimerList, (const void*)SoftwareTimer, FALSE, 0);

    if(Index == 0)
    {
	     ExitCritical();

	     return (Bool)FALSE;
    }

    LinkedListDeleteNode(&gSoftwareTimerList, Index);

    if(SoftwareTimer->Active == TRUE)
        SoftwareTimerTurnedOff();

    ExitCritical();

    return (Bool)TRUE;
}

Bool cobra_linked_softtimer_is_running(COBRA_SOFTWARE_TIMER *SoftwareTimer)
{
    Bool Running;

    if (SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL)
        return (Bool) FALSE;

    EnterCritical();

    Running = SoftwareTimer->Active;

    ExitCritical();

    return (Bool)Running;
}

Bool cobra_linked_softtimer_set_interval(COBRA_SOFTWARE_TIMER *SoftwareTimer, uint32_t NewInterval)
{
    if (SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL)
        return (Bool) FALSE;

    EnterCritical();

    SoftwareTimer->CallbackInterval = NewInterval;

    ExitCritical();

    return (Bool)TRUE;
}

uint32_t cobra_linked_softtimer_get_interval(COBRA_SOFTWARE_TIMER *SoftwareTimer)
{
    uint32_t Interval;

    if(SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL)
        return (uint32_t)0;

    EnterCritical();

    Interval = SoftwareTimer->CallbackInterval;

    ExitCritical();

    return (uint32_t)Interval;
}

Bool cobra_linked_softtimer_set_callback_and_interval(COBRA_SOFTWARE_TIMER *SoftwareTimer, void(*TimerCallback)(COBRA_SOFTWARE_TIMER *), uint32_t Interval)
{
    if (SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL || TimerCallback == (void(*)(COBRA_SOFTWARE_TIMER*))NULL || Interval == 0)
        return (Bool)FALSE;

    EnterCritical();

    SoftwareTimer->CallbackInterval = Interval;
    SoftwareTimer->Callback = TimerCallback;

    ExitCritical();

    return (Bool)TRUE;
}

Bool cobra_linked_softtimer_set_callback(COBRA_SOFTWARE_TIMER *SoftwareTimer, void(*SoftwareTimerCallback)(COBRA_SOFTWARE_TIMER *))
{
    if(SoftwareTimer == (COBRA_SOFTWARE_TIMER*)NULL || SoftwareTimerCallback == (void(*)(COBRA_SOFTWARE_TIMER *))NULL)
        return (Bool)FALSE;

    EnterCritical();

    SoftwareTimer->Callback = SoftwareTimerCallback;

    ExitCritical();

    return (Bool)TRUE;
}

uint32_t cobra_linked_softtimer_get_milliseconds(COBRA_SOFTWARE_TIMER *SoftwareTimer)
{
    uint32_t Milliseconds;

    if(SoftwareTimer == (COBRA_SOFTWARE_TIMER*) NULL)
        return (uint32_t)0;

    EnterCritical();

    // convert the seconds to milliseconds
    Milliseconds = SoftwareTimer->Milliseconds;

    ExitCritical();

    return (uint32_t)Milliseconds;
}


//void softtimer_test_callback(COBRA_SOFTWARE_TIMER *p_timer)
//{
//	  printf("1\n");
//}
//void app_cobra_linked_softtimer_test(void)
//{
//		COBRA_SOFTWARE_TIMER *softtimer = cobra_linked_softtimer_create(NULL, softtimer_test_callback, 10);
//	  cobra_linked_softtimer_start(softtimer);	
//}

#endif