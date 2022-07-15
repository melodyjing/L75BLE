/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_lights_lc.h
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/19

	Description :   
        Light LC Mode

	Changed Log	:

		Liujian  2021/2/19	- Creation

*****************************************************************************/

#ifndef __MM_LIGHTS_LC_H__
#define __MM_LIGHTS_LC_H__

#include "mm_lights_int.h"

/// Validity of information provided to the Replay Manager
#define MM_LIGHT_LC_REPLAY_MS  6000


uint16_t mm_lights_lc_register(uint8_t elmt_idx, uint8_t mdl_cfg_idx, bool main);

#endif

