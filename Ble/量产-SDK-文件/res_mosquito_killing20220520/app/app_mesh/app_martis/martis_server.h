/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	Author:Liujian
	File Name	:	  martis_server.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/7/14
	Description :   
	    微断这边的设备
	Changed Log	:
	Liujian  2021/7/14		- Creation
*****************************************************************************/

#ifndef __MARTIS_SERVER_H__
#define __MARTIS_SERVER_H__

#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions

#include "martis_common.h"

#define MM_MICRO_BREAKER_REPLAY_MS  (6000)

uint16_t m_martis_breaker_register_server(uint8_t elmt_idx, bool main);

#endif

