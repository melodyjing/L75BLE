/**@file            ble_wechat_task.c
* @brief            
* @details          
* @author      			Liujian
* @date             2020-11-26
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.

                   使用微信服务 的任务
*/
#ifndef   __BLE_WECHAT_TASK_H__
#define   __BLE_WECHAT_TASK_H__
#include "rwip_task.h" // Task definitions
#include "rdw_mcu.h"

enum wechat_msg_id
{
    /// 
    WECHAT_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_WECHAT), //!< WECHAT_ENABLE_REQ
    /// Enable confirmation
    WECHAT_ENABLE_RSP, //


    WECHAT_DATA_SEND_REQ,
	
	  WECHAT_DATA_SEND_RSP,
	
    WECHAT_RX_DATA_IND,

    /// Inform APP that Indication Configuration has been changed - use to update bond data
    WECHAT_CFG_INDNTF_IND, //!< WECHAT_CFG_INDNTF_IND
};

/// Parameters of the @ref WECHAT_ENABLE_REQ message
struct wechat_enable_req
{
    /// Connection index
    uint8_t conidx;
    /// Notification configuration (Bond Data to restore: @see enum )
    uint8_t ntf_ind_cfg;
};

struct wechat_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status of enable request
    uint8_t status;
};

struct wechat_rx_data
{
    uint8_t conidx;
    uint8_t rx_data[BLE_WECHAT_MAX_DATA_LEN];
    uint8_t length;
};

struct wechat_data_send_req
{
    uint8_t length;
    uint8_t tx_data[BLE_WECHAT_MAX_DATA_LEN];
};

struct wechat_data_send_rsp
{
    /// status
    uint8_t status;
};


struct wechat_cfg_indntf_ind
{
    /// connection index
    uint8_t conidx;
    /// Notification Configuration 
    uint8_t ntf_ind_cfg;
};

void wechat_task_init(struct ke_task_desc *task_desc);

#endif
