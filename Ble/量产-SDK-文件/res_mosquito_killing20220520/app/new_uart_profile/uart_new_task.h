
/**@file            uart_new_task.h
* @brief            
* @details          
* @author      			
* @date             2018-04-03
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2019/04/03  <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef UART_NEW_TASK_H_
#define UART_NEW_TASK_H_

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
#if LOW_POWER_WAKE_UP_BY_UART
#define MAX_UART_SIZE   120
#define MAX_RX_UART     120
#else
#define MAX_UART_SIZE   512
#define MAX_RX_UART     512
#endif

/// Messages for Health Thermometer Profile Thermometer
/*@TRACE*/
enum uart_msg_id
{
    /// Start the Health Thermometer Profile Thermometer profile - at connection
    UART_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_UART), //!< HTPT_ENABLE_REQ
    /// Enable confirmation
    UART_ENABLE_RSP, //!< HTPT_ENABLE_RSP

    /// Send temperature value from APP
    UART_TEMP_SEND_REQ, //!< HTPT_TEMP_SEND_REQ
    /// Send temperature response
    UART_TEMP_SEND_RSP, //!< HTPT_TEMP_SEND_RSP

    /// Indicate Measurement Interval
    UART_MEAS_INTV_UPD_REQ, //!< HTPT_MEAS_INTV_UPD_REQ
    /// Send Measurement Interval response
    UART_MEAS_INTV_UPD_RSP, //!< HTPT_MEAS_INTV_UPD_RSP

    UART_RX_DATA_IND,

    /// Inform APP that Indication Configuration has been changed - use to update bond data
    UART_CFG_INDNTF_IND, //!< HTPT_CFG_INDNTF_IND
};

/// Database Feature Configuration Flags
enum uart_features
{
    /// All Features supported
    UART_ALL_FEAT_SUP = 0x1F,
};

/// Notification and indication configuration
enum uart_ntf_ind_cfg
{
    /// Stable measurement interval indication enabled
    UART_CFG_STABLE_MEAS_IND = (1 << 0),
    /// Intermediate measurement notification enabled
    UART_CFG_INTERM_MEAS_NTF = (1 << 1),
    /// Measurement interval indication
    UART_CFG_MEAS_INTV_IND = (1 << 2),

};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */
/// Parameters of the Health thermometer service database
struct uart_db_cfg
{
    /// Health thermometer Feature (@see enum htpt_features)
    uint8_t features;
    /// Temperature Type Value
    //    uint8_t temp_type;

    /// Measurement Interval Valid Range - Minimal Value
    //   uint16_t valid_range_min;
    /// Measurement Interval Valid Range - Maximal Value
    //   uint16_t valid_range_max;
    /// Measurement interval (latest known interval range)
    uint16_t meas_intv;
};

/// Parameters of the @ref UART_ENABLE_REQ message
struct uart_enable_req
{
    /// Connection index
    uint8_t conidx;
    /// Notification configuration (Bond Data to restore: @see enum htpt_ntf_ind_cfg)
    uint8_t ntf_ind_cfg;
};

/// Parameters of the @ref UART_ENABLE_RSP message
struct uart_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status of enable request
    uint8_t status;
};

/// Parameters of the @ref UART_TEMP_SEND_REQ message
struct uart_data_send_req
{
    uint16_t length;
    uint8_t  tx_data[MAX_UART_SIZE];
};

/// Parameters of the @ref UART_TEMP_SEND_RSP message
struct uart_send_rsp
{
    /// Status
    uint8_t status;
};

/// Parameters of the @ref HTPT_MEAS_INTV_UPD_REQ message
struct uart_meas_intv_upd_req
{
    /// Measurement Interval value
    uint16_t meas_intv;
};

/// Parameters of the @ref HTPT_MEAS_INTV_UPD_RSP message
struct uart_meas_intv_upd_rsp
{
    /// status
    uint8_t status;
};

/// Parameters of the @ref HTPT_CFG_INDNTF_IND message
struct uart_cfg_indntf_ind
{
    /// connection index
    uint8_t conidx;
    /// Notification Configuration (@see enum htpt_ntf_ind_cfg)
    uint8_t ntf_ind_cfg;
};

struct uart_rx_data
{
    uint8_t conidx;
    uint8_t rx_data[MAX_RX_UART];
    uint16_t length;
};

/// @} HTPTTASK
#endif // HTPT_TASK_H_
