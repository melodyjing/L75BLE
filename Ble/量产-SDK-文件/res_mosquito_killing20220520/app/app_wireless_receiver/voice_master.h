/******************************************************************************
	File Name	:	voice_master.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/07

	Description :   连接声音模块的 host profile

	Changed Log	:
		Liujian  2020/03/07		- Creation

*****************************************************************************/
#ifndef __VOICE_MASTER_H__
#define __VOICE_MASTER_H__

#include "rwip_config.h"
#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "rwip_config.h"
#include "prf.h"
#include "gap.h"

#include "ke_mem.h"
#include "co_utils.h"
#include "rdw_mcu.h"

#include "voice_master_task.h"


#define VOICE_COUNT_IDX_MAX  1

#if BLE_APP_RECEIVE_VOICE


/// Possible states of the UART task
enum
{
    /// Free state
    VOICE_MASTER_FREE,
    /// IDLE state
    VOICE_MASTER_IDLE,
    /// Discovering Blood Pressure SVC and CHars
    VOICE_MASTER_DISCOVERING,
    /// Busy state
    VOICE_MASTER_BUSY,

    /// Number of defined states.
    VOICE_MASTER_STATE_MAX
};



//struct voice_master_cnx_env
//{
//    /// Last requested UUID(to keep track of the two services and char)
//    uint16_t last_uuid_req;
//    /// Last characteristic code used in a read or a write request
//    uint16_t last_char_code;
//    /// Counter used to check service uniqueness
//    uint8_t nb_svc;
//    /// Current operation code
//    uint8_t operation;

//		struct voice_master_bsp_content bps;
//};


struct voice_master_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct voice_master_cnx_env* env[VOICE_COUNT_IDX_MAX];
    /// State of different task instances
    ke_state_t state[VOICE_COUNT_IDX_MAX];
};



const struct prf_task_cbs* voice_master_prf_itf_get(void);

void voice_master_enable_rsp_send(struct voice_master_env_tag *voice_master_env, uint8_t conidx, uint8_t status);

#endif
#endif

