/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns_scene.h
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/7

	Description :   


	Changed Log	:

		Liujian  2021/2/7	- Creation

*****************************************************************************/

#ifndef __MM_TSCNS_SCENE_H__
#define __MM_TSCNS_SCENE_H__

#include "mm_tscns.h"
#include "mm_api_int.h"       // Mesh Model Application Programming Interface Internal Definitions
#include "mesh_tb_timer.h"    // Mesh Timer Manager Definitions


#define MM_SCENE_REPLAY_MS             (600)

#define MM_MSG_SCENE_GET		        		0x4182
#define MM_MSG_SCENE_RECALL		          0x4282
#define MM_MSG_SCENE_RECALL_NOACK		    0x4382
#define MM_MSG_SCENE_STATUS		    		  0x5E
#define MM_MSG_SCENE_REG_GET		        0x4482
#define MM_MSG_SCENE_REG_STATUS		      0x4582
#define MM_MSG_SCENE_STORE		        	0x4682
#define MM_MSG_SCENE_STORE_NOACK		    0x4782
#define MM_MSG_SCENE_DEL		        		0x9E82
#define MM_MSG_SCENE_DEL_NOACK		    	0x9F82


uint16_t mm_tscns_scene_register(uint8_t elmt_idx, m_lid_t *p_mdl_lid);

void mm_tscns_scene_cb_grp_event(m_lid_t mdl_lid, uint8_t event, uint8_t info);


enum{
	SCENE_ID_INVALID 	= 0,
};

enum{
	SCENE_ST_SUCCESS 	= 0,
	SCENE_ST_REG_FULL	= 1,
	SCENE_ST_NOT_FOUND	= 2,
	SCENE_ST_MAX,
};
#if (DRIVE_MESH_CWHSL_LIGHT_FUNTION == 1)	
#define   MAX_SCENE_COUNT  16
#endif

#define SCENE_CNT_MAX			(16)

typedef struct
{
	   uint16_t id;
	   int16_t lightness_s16;

	   int16_t temp_s16;
	   int16_t delta_uv_s16;

	   int16_t hue_s16;
	   int16_t sat_s16;

     int16_t x_s16;
     int16_t y_s16;

}scene_data_t;
#if (DRIVE_MESH_CWHSL_LIGHT_FUNTION == 1)	
typedef struct {
	 
	 uint8_t    statues_code;
	 
	 uint16_t   current_scene_index;
	
	 uint16_t   target_scene_index;
	
	 uint8_t    remaining_time;
		
}scene_status_t;

typedef struct {
	 
	 uint8_t    statues_code;
	 
	 uint16_t   current_scene;       //µ±Ç°³¡¾°µÄ index
	
	 uint16_t   scene[MAX_SCENE_COUNT];
		
}scene_reg_status_t;

typedef struct mm_tscns_scene_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
    /// Timer for sending of publications
    mesh_tb_timer_t tmr_publi;
    /// Publication period in milliseconds
    uint32_t publi_period_ms;

    /// Environment for replay protection mechanism
    mm_tb_replay_mdl_env_t replay_env;

 
	  scene_reg_status_t  reg_scene_status;
	
    /// Source address of set message that has triggered last or current transition
    uint16_t status_dst_addr;
    /// Application key local index to be used for transmission of Schdule Status

    m_lid_t status_app_key_lid;
	
    /// Relaying of sent Time Status authorized
    bool status_relay;
	
}mm_tscns_scene_env_t;

/// Structure for Scene Setup Server model environment
typedef struct mm_tscns_scenes_env
{
    /// Basic model environment - Must be first element in the structure - DO NOT MOVE
    mm_tb_state_mdl_env_t env;
	
    /// Pointer to environment of associated Scene Server model
    mm_tscns_scene_env_t *p_env_scene;
	
} mm_tscns_scenes_env_t;
#endif
#endif