/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns_time.h
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/1

	Description :   


	Changed Log	:

		Liujian  2021/2/1		- Creation

*****************************************************************************/

#ifndef __MM_TSCNS_TIME_H__
#define __MM_TSCNS_TIME_H__

#include "mm_tscns.h"
#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions


/// Validity of information provided to the Replay Manager
#define MM_TIME_REPLAY_MS  6000

#define  MM_MSG_TIME_GET                 0x3782    //Time Get
#define  MM_MSG_TIME_SET                 0x5C
#define  MM_MSG_TIME_STATUS              0x5D
#define  MM_MSG_TIME_ROLE_GET            0x3882
#define  MM_MSG_TIME_ROLS_SET            0x3982
#define  MM_MSG_TIME_ROLS_STATUS         0x3A82
#define  MM_MSG_ZONE_GET                 0x3B82
#define  MM_MSG_ZONE_SET                 0x3C82
#define  MM_MSG_ZONE_STATUS              0x3D82
#define  MM_MSG_TAI_UTC_DELTA_GET        0x3E82
#define  MM_MSG_TAI_UTC_DELTA_SET        0x3F82
#define  MM_MSG_TAI_UTC_DELTA_STATUS     0x4082

enum mm_tscns_time_set_pos
{
    /// TAI Seconds
    MM_TSCNS_TIME_TAI_SECONDS_POS = 0,
    /// Subseconds
    MM_TSCNS_TIME_SUBSECONDS_POS = 5,
    /// Uncertain
    MM_TSCNS_TIME_UNCERTAINTY_POS = 6,
    /// TID
//    MM_TSCNS_TIME_UNCERTAINTY_POS = 6,
//    /// Transition Time
//    MM_LIGHT_HSL_SET_TRANS_TIME_POS = 7,
//    /// Delay
//    MM_LIGHT_HSL_SET_DELAY_POS = 8
};



uint16_t mm_tscns_time_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);

void mm_tscns_time_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info);





#endif
