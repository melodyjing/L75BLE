/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	led_button_task.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/6/28

	Description :   Led and button service task implementation
	Changed Log	:

		Liujian  2020/6/28		- Creation
*****************************************************************************/

#ifndef __LED_BUTTON_TASK_H__
#define __LED_BUTTON_TASK_H__
#if defined(CFG_PRF_LED_BUTTON)

#include "prf_types.h"
#include "rdw_mcu.h"
#include "rwip_task.h"

/// Messages for Led Button Profile 
/*@TRACE*/
enum led_button_msg_id
{
    /// Start the Led Button profile - at connection
    LED_BUTTON_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_LED_BUTTON), //!< LED_BUTTON_ENABLE_REQ
    /// Enable confirmation
    LED_BUTTON_ENABLE_RSP,                               //!< LED_BUTTON_ENABLE_RSP

   /// Send Button value from APP
    LED_BUTTON_BUTTON_SEND_REQ,                            //!< LED_BUTTON_BUTTON_SEND_REQ
    /// Send temperature response
    LED_BUTTON_BUTTON_SEND_RSP,                            //!< LED_BUTTON_BUTTON_SEND_RSP
	
	
    /// Inform APP of new led value requested by a peer device
    LED_BUTTON_LED_REQ_IND,                    //!< LED_BUTTON_LED_REQ_IND
    /// APP Confirm message of new led value requested by a peer device
    /// If accepted, it triggers indication on led value attribute
    LED_BUTTON_LED_CHG_CFM,                        //!< LED_BUTTON_LED_CHG_CFM

    /// Inform APP that Indication Configuration has been changed - use to update bond data
    LED_BUTTON_CFG_INDNTF_IND,                           //!< LED_BUTTON_CFG_INDNTF_IND
};


/// Notification and indication configuration
enum led_button_ntf_ind_cfg
{
    /// Button indication enabled
    LED_BUTTON_CFG_BUTTON_NTF    = (1 << 0),
    /// LED value indication
    LED_BUTTON_CFG_LED_IND      = (1 << 1),
};

/// Parameters of the @ref HTPT_CFG_INDNTF_IND message
struct led_button_cfg_indntf_ind
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration (@see enum htpt_ntf_ind_cfg)
    uint8_t  ntf_ind_cfg;
};



/// Database Feature Configuration Flags
enum led_button_features
{
    /// Indicate if Button Type Char. is supported
    LED_BUTTON_TYPE_CHAR_SUP        = 0x01,


    /// All Features supported
    LED_BUTTON_ALL_FEAT_SUP              = 0x01,
};


/// Parameters of the Led and Button service database
struct led_button_db_cfg
{
    /// LED Button Feature (@see enum led_button_features)
    uint8_t features;
};

/// Parameters of the @ref LED_BUTTON_ENABLE_REQ message
struct led_button_enable_req
{
    /// Connection index
    uint8_t conidx;
    /// Notification configuration (Bond Data to restore: @see enum led_button_ntf_ind_cfg)
    uint8_t  ntf_ind_cfg;
	
	  uint8_t  led_value;
};

/// Parameters of the @ref LED_BUTTON_ENABLE_RSP message
struct led_button_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status of enable request
    uint8_t status;
};



/// Parameters of the @ref LED_BUTTON_LED_REQ_IND message
struct led_button_led_req_ind
{
    /// Connection index
    uint8_t conidx;
    /// new led value
    uint8_t led_value;
};

/// Parameters of the @ref LED_BUTTON_BUTTON_SEND_REQ message
struct led_button_button_send_req
{
    /// Button Value
    uint8_t key_value;
};



#endif
#endif



