/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	Author:Liujian
	cobra Drivers
	File Name	:	cobra4_rdw_common.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/7/9
	Description :   
          ��һ���� cobra4_common  ���ڷ������ĺ����Ƶ�����ļ���
	Changed Log	:
		Liujian  2021/7/9			- Creation

*****************************************************************************/

#ifndef __COBRA4_RDW_COMMON_H__
#define __COBRA4_RDW_COMMON_H__

#include <stdint.h>

void app_common_uart_hci_mode_init(void);

const struct rwip_eif_api* rwip_eif_get(uint8_t type);

void app_m4_trim_init(void);

#endif

