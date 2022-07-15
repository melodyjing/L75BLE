/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra4A_systick.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/10

	Description :   
       ����֮ǰ�Ķ�ʱ��, �����ĸ���, ������ǵڶ����汾

	Changed Log	:

		Liujian  2020/8/10 		- Creation

*****************************************************************************/
#ifndef __COBRA4A_SYSTICK_H__
#define __COBRA4A_SYSTICK_H__

typedef void (*systick_callback)(void);

void cobra4_systick_init(unsigned int timer_frequency, systick_callback callback);
void cobra4_systick_uninit(void);

#endif


