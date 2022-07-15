/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	led_button.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/6/28

	Description :   Led and button service implementation
	Changed Log	:

		Liujian  2020/6/28		- Creation
*****************************************************************************/
#ifndef __LED_BUTTON_H__
#define __LED_BUTTON_H__
#if defined(CFG_PRF_LED_BUTTON)

#include "led_button_task.h"
#include "attm.h"
#include "prf.h"
#include "rdw_mcu.h"

#define LED_BUTTON_IDX_MAX		(1)


#define LED_BUTTON_IDX(hdl)    (led_button_att_idx_get(led_button_env, (hdl)))

#define LED_BUTTON_HANDLE(idx) (led_button_att_hdl_get(led_button_env, (idx)))


#define LED_BUTTON_IND_NTF_CFG_MAX_LEN          (2)

#define LED_BUTTON_LED_MAX_LEN            			(1)




/// ongoing operation information
struct led_button_op
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


///Led Button Profile Thermometer Environment Variable
struct led_button_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// On-going operation
    struct led_button_op *operation;
    /// Service Start Handle
    uint16_t shdl;
    /// Database configuration
    uint16_t features;

    /// Notification and indication configuration of peer devices.
    uint8_t ntf_ind_cfg[BLE_CONNECTION_MAX];
    /// State of different task instances
    ke_state_t state[LED_BUTTON_IDX_MAX];
	
	  uint8_t   led_value;
	
};

///Attributes database elements
enum led_burtton_db_list
{
    LED_BUTTON_IDX_SVC,
	
   //Button
    LED_BUTTON_IDX_BUTTON_CHAR,
    LED_BUTTON_IDX_BUTTON_VAL,
    LED_BUTTON_IDX_BUTTON_IND_CFG,
	
    //LED handler
    LED_BUTTON_IDX_LED_CHAR,
    LED_BUTTON_IDX_LED_VAL,
 //   LED_BUTTON_IDX_LED_IND_CFG,

    

    LED_BUTTON_IDX_NB,
};

/// Possible states of the LED Button task
enum led_button_state
{
    /// Idle state
    LED_BUTTON_IDLE,
    /// Busy state
    LED_BUTTON_BUSY,

    /// Number of defined states.
    LED_BUTTON_STATE_MAX
};


uint16_t led_button_att_hdl_get(struct led_button_env_tag *led_button_env, uint8_t att_idx);

uint8_t led_button_att_idx_get(struct led_button_env_tag *led_button_env, uint16_t handle);


uint8_t led_button_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value);

void led_button_task_init(struct ke_task_desc *task_desc);

void led_button_exe_operation(void);

const struct prf_task_cbs* led_button_prf_itf_get(void);
#endif
#endif
