/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  cobra_mmu.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/1/25

	Description :   
        内存管理模块, 把空闲的内存管理起来, 参考的代码为  rt-thread os

	Changed Log	:

		Liujian  2021/1/25			- Creation

*****************************************************************************/

#ifndef __COBRA_MMU_H__
#define __COBRA_MMU_H__
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "rdw_mcu.h"
#include <cobra_config.h>

#if (COBRA4_MMU_ENABLE == 1)
#define COBRA_MEM_STATS  1

void  cobra4_free(void *rmem);

void *cobra4_malloc(unsigned long size);

void *cobra4_realloc(void *rmem, unsigned long newsize);

void *cobra4_calloc(unsigned long count, unsigned long  size);

void cobra4_system_heap_init(void *begin_addr, void *end_addr);

#endif

void rdw_memory_information(int argc, char *argv[]);

#endif
