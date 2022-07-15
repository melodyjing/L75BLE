/*
liujian
2018/6/12
version 1.0


*/

#ifndef __RDW_VOICE_H__
#define __RDW_VOICE_H__


#include "rwip_config.h"     // SW configuration

#if (RDW_VOICE_PROFILE)   
	
#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"         // Kernel Task Definition

#include "prf.h"
#include "prf_types.h"
#include "prf_utils.h"

#include "gap.h"


#define VOICE_HANDLE(idx) (voice_att_hdl_get(voice_env, (idx)))

#define VOICE_IDX(hdl)    (voice_att_idx_get(voice_env, (hdl)))


#define  VOICE_PROFILE_MASK               (0x007F)

#define  VOICE_IDX_MAX    (1)

///Attributes database elements
enum voice_att_db_list
{
    VOICE_IDX_SVC,

	//bluetooth to phone
    VOICE_START_IDX_TX_CHAR,
    VOICE_START_IDX_TX_VAL,
    VOICE_START_IDX_TX_VAL_IND_CFG,
	
	//phone send to bluetooth
	VOICE_AUDIO_IDX_TX_CHAR,
	VOICE_AUDIO_IDX_TX_VAL,  
	VOICE_AUDIO_IDX_TX_VAL_IND_CFG,
		
    VOICE_IDX_NB,
	
};

enum voice_state
{
    /// Idle state
    VOICE_IDLE,
    /// Busy state
    VOICE_BUSY,

    /// Number of defined states.
    VOICE_STATE_MAX
};

/// ongoing operation information
struct voice_op
{
     /// Operation
     uint8_t op;
     /// Cursor on connection
     uint8_t cursor;
     /// Handle of the attribute to indicate/notify
     uint16_t handle;
     /// Task that request the operation that should receive completed message response
     uint16_t dest_id;
     /// Packed notification/indication data size
     uint8_t length;
     /// used to know on which device interval update has been requested, and to prevent
     /// indication to be triggered on this connection index
     uint8_t conidx;
     /// Packed notification/indication data
     uint8_t data[__ARRAY_EMPTY];
};




struct  voice_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// On-going operation
    struct voice_op * operation;
    /// Service Start Handle
    uint16_t shdl;
    /// Database configuration
    uint16_t features;

    /// Notification and indication configuration of peer devices.
    uint8_t ntf_ind_cfg[BLE_CONNECTION_MAX];
    /// State of different task instances
    ke_state_t state[VOICE_STATE_MAX];
};



//一些其它文件需要用到的函数,在这里Public 出来

const struct prf_task_cbs*  voice_prf_itf_get(void);

void voice_exe_operation(void);

uint8_t voice_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value);

uint16_t voice_att_hdl_get(struct voice_env_tag* voice_env, uint8_t att_idx);

uint8_t  voice_att_idx_get(struct voice_env_tag* voice_env, uint16_t handle);






#endif


#endif 
