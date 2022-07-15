/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns_schdule.h
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/2

	Description :   


	Changed Log	:

		Liujian  2021/2/2	- Creation

*****************************************************************************/

#ifndef __MM_TSCNS_SCHDULE_H__
#define __MM_TSCNS_SCHDULE_H__
#include "mm_tscns.h"
#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions


#define MM_SCHDULE_REPLAY_MS  (600)
//MM_MSG_TIME_GET
#define MM_MSG_SHCDULE_ACTION_GET		        	0x4882
#define MM_MSG_SHCDULE_ACTION_STATUS		      0x5F
#define MM_MSG_SHCDULE_GET		        		    0x4982
#define MM_MSG_SHCDULE_STATUS		        		  0x4A82
#define MM_MSG_SHCDULE_ACTION_SET		        	0x60
#define MM_MSG_SHCDULE_ACTION_SET_NOACK		    0x61


uint16_t mm_tscns_schdule_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);

void mm_tscns_schdule_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info);


enum{
    SCHD_YEAR_ANY = 0x64,
    SCHD_YEAR_MAX,
};

enum{
    SCHD_MONTH_ANY = 0,  // define by Examples of Table 5.14
};

enum{
    SCHD_DAY_ANY = 0,
    SCHD_DAY_MAX = 31,
};

enum{
    SCHD_WEEK_ANY = 0,  // define by Examples of Table 5.14 // SCHD_WEEK_ANY = BIT_MASK_LEN(7),    // just use as input parameter
};

enum{
    SCHD_HOUR_ANY       = 0x18,
    SCHD_HOUR_RANDOM    = 0x19,
    SCHD_HOUR_MAX,
};

enum{
    SCHD_MIN_ANY        = 0x3C,
    SCHD_MIN_EVERY15    = 0x3D, // 0, 15, 30, 45
    SCHD_MIN_EVERY20    = 0x3E, // 0, 20, 40, 
    SCHD_MIN_RANDOM     = 0x3F,
    SCHD_MIN_MAX,
};

enum{
    SCHD_SEC_ANY        = 0x3C,
    SCHD_SEC_EVERY15    = 0x3D, // 0, 15, 30, 45
    SCHD_SEC_EVERY20    = 0x3E, // 0, 20, 40, 
    SCHD_SEC_RANDOM     = 0x3F,
    SCHD_SEC_MAX,
};

enum{
	SCHD_ACTION_OFF     = 0,
	SCHD_ACTION_ON      = 1,
	SCHD_ACTION_SCENE   = 2,
	SCHD_ACTION_NONE    = 0x0F,
	SCHD_ACTION_MAX,
};

enum{
    RANDOM_REBUILD_HOUR = 0,
    RANDOM_REBUILD_MIN,
    RANDOM_REBUILD_SEC,
};

#define NO_NEAREST  (0xff)

typedef struct
{
	
	uint32_t TAI_local;
	
	uint16_t entry_bit;
	
}schd_nearest_t;

#endif
