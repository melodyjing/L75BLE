/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_sleep.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/3/4

	Description :   


	Changed Log	:

		Liujian  2021/3/4			- Creation

*****************************************************************************/

#ifndef __COBRA_SLEEP_H__
#define __COBRA_SLEEP_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "ke_mem.h"
#include "ke_msg.h"
#include "rwble_hl.h"
#include "hci.h"
#include "co_bt.h"


void cobra_sleep_init(void);
void cobra_sleep_enter(void);
void cobra_sleep_exit(void);


#endif
