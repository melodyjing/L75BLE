/**@file            ble_wechat_service.h
* @brief            
* @details          
* @author      			Liujian
* @date             2020-11-26
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.

*                 微信平台连接 的服务

**********************************************************************************/

#ifndef __BLE_WECHAT_SERVICE_H__
#define __BLE_WECHAT_SERVICE_H__

#include "rwip_task.h" // Task definitions
#include "gap.h"
#include "prf.h"
#include "prf_types.h"
#include "prf_utils.h"

#if (APP_MOSQUITO_UUID_DEBUG_211201_ENABLE == 1)
#define BLE_UUID_WECHAT_SERVICE							 			0xFFB0
#define BLE_UUID_WECHAT_WRITE_CHARACTERISTICS 		0xFFB1
#define BLE_UUID_WECHAT_INDICATE_CHARACTERISTICS 	0xFFB2
#define BLE_UUID_WECHAT_READ_CHARACTERISTICS 			0xFFB3
#else
#define BLE_UUID_WECHAT_SERVICE							 			0xFEE7
#define BLE_UUID_WECHAT_WRITE_CHARACTERISTICS 		0xFEC7
#define BLE_UUID_WECHAT_INDICATE_CHARACTERISTICS 	0xFEC8
#define BLE_UUID_WECHAT_READ_CHARACTERISTICS 			0xFEC9
#endif

#define BLE_WECHAT_MAX_DATA_LEN                   256 //(ATT_DEFAULT_MTU - 3) 							/**<Set the Maximum Data length og WECHAT to the (GATT Maximum Transmission Unit -3) */


#define WECHAT_IDX_MAX (1)

#define WECHAT_IND_NTF_CFG_MAX_LEN  2

#define WECHAT_PROFILE_MASK  0x00FF

#define WECHAT_IS_FEATURE_SUPPORTED(feat, bit_mask) (((feat & bit_mask) == bit_mask))

#define WECHAT_HANDLE(idx) (wechat_att_hdl_get(wechat_env, (idx)))

#define WECHAT_IDX(hdl) (wechat_att_idx_get(wechat_env, (hdl)))

enum wechat_att_db_list
{
    WECHAT_IDX_SVC,
	  WECHAT_IDX_IND_CHAR,
	  WECHAT_IDX_IND_VAL,
	  WECHAT_IDX_IND_CFG,
	
    WECHAT_IDX_READ_FEATURE_CHAR,
	  WECHAT_IDX_READ_FEATURE_VAL,
	
    WECHAT_IDX_WRITE_FEATURE_CHAR,
	  WECHAT_IDX_WRITE_FEATURE_VAL,
	
    WECHAT_IDX_NB,
};

/// Parameters of the wechat service database
struct wechat_db_cfg
{
    /// wechat feature
    uint8_t features;
};

enum wechat_ntf_ind_cfg
{
    /// Stable data indication enabled
    WECHAT_CFG_DATA_IND = (1 << 0),

};


/// Possible states of the wechat task
enum uart_state
{
    /// Idle state
    WECHAT_IDLE,
    /// Busy state
    WECHAT_BUSY,

    /// Number of defined states.
    WECHAT_STATE_MAX
};

struct wechat_op
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


struct wechat_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// On-going operation
    struct wechat_op *operation;
    /// Service Start Handle
    uint16_t shdl;
    /// Database configuration
    uint16_t features;

    /// Notification and indication configuration of peer devices.
    uint8_t ntf_ind_cfg[BLE_CONNECTION_MAX];
    /// State of different task instances
    ke_state_t state[WECHAT_IDX_MAX];
};

void wechat_exe_operation(void);

const struct prf_task_cbs *wechat_prf_itf_get(void);

uint8_t wechat_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value);

uint16_t wechat_att_hdl_get(struct wechat_env_tag *wechat_env, uint8_t att_idx);

uint8_t wechat_att_idx_get(struct wechat_env_tag *wechat_env, uint16_t handle);

#endif
