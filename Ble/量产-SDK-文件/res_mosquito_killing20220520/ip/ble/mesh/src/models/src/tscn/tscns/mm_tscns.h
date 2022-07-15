/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns.h
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/1

	Description :   


	Changed Log	:

		Liujian  2021/2/1		- Creation

*****************************************************************************/

#ifndef __MM_TSCNS_H__
#define __MM_TSCNS_H__

#include "mesh_api.h"       // Mesh Model API Definitions
#include "mm_defines.h"     // Mesh Model Definitions
#include "mm_tb.h"          // Mesh Model Tool Boxes Definitions
#include "mm_route.h"       // Mesh Model Routing Module Definitions
#include "mesh_tb_buf.h"    // Mesh Buffer Manager Definitions




uint16_t mm_tscns_init(bool reset, void *p_env, const mm_cfg_t *p_cfg);

uint16_t mm_tscns_get_env_size(const mm_cfg_t *p_cfg);

uint16_t mm_tscns_register(uint8_t elmt_idx, uint8_t mdl_cfg_idx, bool main);

#endif
