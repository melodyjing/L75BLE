/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4A_linked_softtimer_config.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/11

	Description :   
     软件定时器的配置

	Changed Log	:

		Liujian  2020/8/11		- Creation

*****************************************************************************/
#ifndef __COBRA4A_LINKED_SOFTTIMER_CONFIG_H__
#define __COBRA4A_LINKED_SOFTTIMER_CONFIG_H__

#include "cobra_mmu.h"

/**
	*This defines what method the Timers library will use to
	allocate memory and deallocated.  This allows the Timers
	library to be integrated into an OS.
*/

#define SoftwareTimerMemAlloc(Mem)					cobra4_malloc(Mem)

/**
	*The method used for freeing up memory.
*/
#define SoftwareTimerMemDealloc             cobra4_free

/**
	*Define USE_MALLOC as 1 to enable the stdlib.h file included
	with LinkedList.c.
*/
#define SOFTWARE_TIMER_USE_MALLOC					1

/**
	*If the user isn't using malloc then include the file that will
	be needed for accessing whatever method SoftwareTimerMemAlloc()
        and SoftwareTimerMemDealloc() implement, below.
*/
//#include "Your memory file here.h"

#define EnterCritical GLOBAL_INT_DISABLE
#define ExitCritical  GLOBAL_INT_RESTORE
#endif
