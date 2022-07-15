/******************************************************************************
	File Name	:	device1_uart_task.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/27

	Description :   

	Changed Log	:
		Liujian  2019/11/27			- Creation

*****************************************************************************/


#ifndef __DEVICE1_UART_TASK_H__
#define __DEVICE1_UART_TASK_H__


#if (BLE_DEVICE1_UART_ENABLE)
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h" // Task definitions
#include "rdw_mcu.h"
//#include "htp_common.h"
/*
 * DEFINES
 ****************************************************************************************
 */
#define   MAX_UART_SIZE   247
#define   MAX_RX_UART     247 

/// Messages for Health Thermometer Profile Thermometer
/*@TRACE*/
enum  device1_uart_msg_id
{
    /// Start the Health Thermometer Profile Thermometer profile - at connection
    DEVICE1_UART_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_DEVICE1_UART),//!< HTPT_ENABLE_REQ
    /// Enable confirmation
    DEVICE1_UART_ENABLE_RSP,                               //!< HTPT_ENABLE_RSP

    /// Send temperature value from APP
    DEVICE1_UART_TEMP_SEND_REQ,                            //!< HTPT_TEMP_SEND_REQ
    /// Send temperature response
    DEVICE1_UART_TEMP_SEND_RSP,                            //!< HTPT_TEMP_SEND_RSP

	
    /// Indicate Measurement Interval
    DEVICE1_UART_MEAS_INTV_UPD_REQ,                        //!< HTPT_MEAS_INTV_UPD_REQ
    /// Send Measurement Interval response
    DEVICE1_UART_MEAS_INTV_UPD_RSP,                        //!< HTPT_MEAS_INTV_UPD_RSP
	
		DEVICE1_UART_RX_DATA_IND,
	
    /// Inform APP that Indication Configuration has been changed - use to update bond data
    DEVICE1_UART_CFG_INDNTF_IND,                           //!< HTPT_CFG_INDNTF_IND
};


/// Database Feature Configuration Flags
enum device1_uart_features
{
    /// All Features supported
    DEVICE1_UART_ALL_FEAT_SUP              = 0x1F,
};


/// Notification and indication configuration
enum  device1_uart_ntf_ind_cfg
{
     /// Stable measurement interval indication enabled
    DEVICE1_UART_CFG_STABLE_MEAS_IND    = (1 << 0),
    /// Intermediate measurement notification enabled
    DEVICE1_UART_CFG_INTERM_MEAS_NTF    = (1 << 1),
    /// Measurement interval indication
    DEVICE1_UART_CFG_MEAS_INTV_IND      = (1 << 2),

};


/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */
/// Parameters of the Health thermometer service database
struct device1_uart_db_cfg
{

    uint8_t features;

};

/// Parameters of the @ref UART_ENABLE_REQ message
struct device1_uart_enable_req
{
    /// Connection index
    uint8_t conidx;
    /// Notification configuration (Bond Data to restore: @see enum htpt_ntf_ind_cfg)
    uint8_t  ntf_ind_cfg;
};

/// Parameters of the @ref UART_ENABLE_RSP message
struct device1_uart_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status of enable request
    uint8_t status;
};

/// Parameters of the @ref UART_TEMP_SEND_REQ message
struct device1_uart_data_send_req
{
			uint8_t  length;
			uint8_t  tx_data[MAX_UART_SIZE];
			uint8_t  connection_idx;
};

/// Parameters of the @ref UART_TEMP_SEND_RSP message
struct device1_uart_send_rsp
{
	///Connection index
    uint8_t conidx;
    /// Status
    uint8_t status;
};


/// Parameters of the @ref HTPT_MEAS_INTV_UPD_REQ message
struct device1_uart_meas_intv_upd_req
{
    /// Measurement Interval value
    uint16_t meas_intv;
};

/// Parameters of the @ref HTPT_MEAS_INTV_UPD_RSP message
struct device1_uart_meas_intv_upd_rsp
{
    /// status
    uint8_t status;
};




/// Parameters of the @ref HTPT_CFG_INDNTF_IND message
struct device1_uart_cfg_indntf_ind
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration (@see enum htpt_ntf_ind_cfg)
    uint8_t  ntf_ind_cfg;
};

struct device1_uart_rx_data
{
		uint8_t  conidx;
		uint8_t  rx_data[MAX_RX_UART];
		uint8_t  length;
	
};

void device1_uart_task_init(struct ke_task_desc *task_desc);

#endif

#endif 
