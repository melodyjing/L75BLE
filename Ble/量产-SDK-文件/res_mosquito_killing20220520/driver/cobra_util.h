/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_util.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/9/3

	Description :   
    ���һЩ��������,����SDK �Ŀ���Ӧ��
1. ��������������Ĵ���

	Changed Log	:

		Liujian  2020/9/3 		- Creation

*****************************************************************************/
#ifndef __COBRA_UTIL_H__
#define __COBRA_UTIL_H__
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API


void cobra_white_list_add(struct gap_bdaddr *p_bd_addr);

void cobra_white_list_get_size(void);

uint32_t cobra_app_whitelist_add(void);
#endif
