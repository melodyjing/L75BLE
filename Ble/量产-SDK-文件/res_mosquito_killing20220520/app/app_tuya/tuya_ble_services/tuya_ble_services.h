/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  tuya_ble_services.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/5/6

	Description :   
	      ÒÆÖ²µ½Í¿Ñ» SDK 

	Changed Log	:

		Liujian  2021/5/6			- Creation
*****************************************************************************/
#ifndef __TUYA_BLE_SERVICES_H__
#define __TUYA_BLE_SERVICES_H__

#include "rwip_config.h"
#include <stdint.h>
#include <stdbool.h>
#include "prf.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "gap.h"

#if(TUYA_BLE_SDK_ENABLE == 1)

#define TUYA_BLE_IS_FEATURE_SUPPORTED(feat, bit_mask) (((feat & bit_mask) == bit_mask))

#define TUYA_BLE_HANDLE(idx)  (tuya_ble_att_hdl_get(tuya_ble_env, (idx)))

#define TUYA_BLE_IDX(hdl)     (tuya_ble_att_idx_get(tuya_ble_env, (hdl)))

#define TUYA_BLE_IDX_MAX       (1)

#define TUYA_BLE_IND_NTF_CFG_MAX_LEN (2)

#define TUYA_BLE_PROFILE_MASK      (0x003F)

/// Possible states of the tuya task
enum tuya_ble_state
{
    /// Idle state
    TUYA_BLE_IDLE,
    /// Busy state
    TUYA_BLE_BUSY,

    /// Number of defined states.
    TUYA_BLE_STATE_MAX
};


///Attributes database elements
enum tuya_ble_services_att_db_list
{
    TUYA_BLE_IDX_SVC,

    TUYA_BLE_IDX_TX_CHAR,
    TUYA_BLE_IDX_TX_VAL,
    TUYA_BLE_IDX_TX_VAL_IND_CFG,


    TUYA_BLE_IDX_RX_CHAR,
    TUYA_BLE_IDX_RX_VAL,
    TUYA_BLE_IDX_RX_VAL_CFG,

    TUYA_BLE_IDX_NB,
	
};

/// ongoing operation information
struct tuya_ble_op
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


///Tuya BLE profile Environment Variable
struct tuya_ble_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// On-going operation
    struct tuya_ble_op *operation;
    /// Service Start Handle
    uint16_t shdl;
    /// Database configuration
    uint16_t features;

    /// Notification and indication configuration of peer devices.
    uint8_t ntf_ind_cfg[BLE_CONNECTION_MAX];
    /// State of different task instances
    ke_state_t state[TUYA_BLE_IDX_MAX];
};


uint8_t  tuya_ble_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value);

uint16_t tuya_ble_att_hdl_get(struct tuya_ble_env_tag *tuya_ble_env, uint8_t att_idx);

uint8_t tuya_ble_att_idx_get(struct tuya_ble_env_tag *tuya_ble_env, uint16_t handle);

void tuya_ble_exe_operation(void);

const struct prf_task_cbs *tuya_ble_prf_itf_get(void);

#endif

#endif
