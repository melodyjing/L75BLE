/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  tuya_ble_services_task.h
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/5/18

	Description :   
	      ÒÆÖ²µ½Í¿Ñ» SDK 

	Changed Log	:

		Liujian  2021/5/18			- Creation
*****************************************************************************/

#ifndef __TUYA_BLE_SERVICES_TASK_H__
#define __TUYA_BLE_SERVICES_TASK_H__

#include "rwip_task.h" // Task definitions
#include "rdw_mcu.h"
#include "ke_task.h"


#define MAX_TUYA_BLE_DATA_SIZE   240
#define MAX_RX_TUYA_BLE_SIZE     240

enum tuya_ble_msg_id
{
    /// 
    TUYA_BLE_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_TUYA_BLE), //!< HTPT_ENABLE_REQ
    /// Enable confirmation
    TUYA_BLE_ENABLE_RSP, //!< 

    /// Send data value from APP
    TUYA_BLE_DATA_SEND_REQ, //!< 
    /// Send data response
    TUYA_BLE_DATA_SEND_RSP, //!< 

    /// Indicate Measurement Interval
    TUYA_BLE_MEAS_INTV_UPD_REQ, //!< 
    /// Send Measurement Interval response
    TUYA_BLE_MEAS_INTV_UPD_RSP, //!< 

    TUYA_BLE_RX_DATA_IND,

    /// Inform APP that Indication Configuration has been changed - use to update bond data
    TUYA_BLE_CFG_INDNTF_IND, //!< 
};


enum tuya_ble_services_features
{
    /// All Features supported
    TUYA_BLE_ALL_FEAT_SUP = 0x1F,
};

/// Notification and indication configuration
enum tuya_ble_ntf_ind_cfg
{
    /// Stable measurement interval indication enabled
    TUYA_BLE_CFG_STABLE_MEAS_IND = (1 << 0),
    /// Intermediate measurement notification enabled
    TUYA_BLE_CFG_INTERM_MEAS_NTF = (1 << 1),
    /// Measurement interval indication
    TUYA_BLE_CFG_MEAS_INTV_IND = (1 << 2),

};


struct tuya_ble_services_db_cfg
{
    uint8_t features;

//    uint16_t meas_intv;
};


struct tuya_ble_data_send_req
{
    uint8_t length;
    uint8_t tx_data[MAX_TUYA_BLE_DATA_SIZE];
};

struct tuya_ble_send_rsp
{
    /// Status
    uint8_t status;
};

struct tuya_ble_meas_intv_upd_rsp
{
    /// status
    uint8_t status;
};

struct tuya_ble_cfg_indntf_ind
{
    /// connection index
    uint8_t conidx;
    /// Notification Configuration (@see enum htpt_ntf_ind_cfg)
    uint8_t ntf_ind_cfg;
};

struct tuya_ble_enable_req
{
    /// Connection index
    uint8_t conidx;
    /// Notification configuration 
    uint8_t ntf_ind_cfg;
};

struct tuya_ble_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status of enable request
    uint8_t status;
};

struct tuya_ble_rx_data
{
    uint8_t conidx;
    uint8_t rx_data[MAX_RX_TUYA_BLE_SIZE];
    uint8_t length;
};


void tuya_ble_task_init(struct ke_task_desc *task_desc);

#endif
