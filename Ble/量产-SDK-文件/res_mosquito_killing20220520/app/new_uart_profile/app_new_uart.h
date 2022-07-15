/**@file            app_new_uart.h
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

#ifndef APP_NEW_UART_H_
#define APP_NEW_UART_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h" // SW configuration

#if (BLE_APP_UART == 1)

#include <stdint.h> // Standard Integer Definition
#include "ke_task.h"
#include "uart_new_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

extern struct app_env_tag app_env;

/// health thermometer application environment structure
struct app_uart_env_tag
{
    /// Measurement interval
    uint16_t uart_meas_intv;

    /// Measurement interval timer enable
    bool timer_enable;

    uint8_t rx_data[MAX_UART_SIZE];

    uint8_t receive_data[MAX_RX_UART];
    uint8_t receive_length;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Health Thermomter Application environment
extern struct app_uart_env_tag app_uart_env;

/// Table of message handlers
extern const struct app_subtask_handlers app_uart_handlers;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * Health Thermometer Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Health Thermometer Application
 ****************************************************************************************
 */
void app_uart_init(void);

/**
 ****************************************************************************************
 * @brief Add a Health Thermometer instance in the DB
 ****************************************************************************************
 */
void app_uart_add_uarts(void);

/**
 ****************************************************************************************
 * @brief Stop the interval measurement timer if used
 ****************************************************************************************
 */
void app_uart_stop_timer(void);

/**
 ****************************************************************************************
 * @brief Enable the health thermometer profile
 ****************************************************************************************
 */
void app_uart_enable_prf(uint8_t);

#if (UART_MODEL_TIMEOUT_ENABLE == 1)
void uart_send_to_phone(uint8_t *buffer, uint16_t length);
#endif

#endif //(BLE_APP_HT)

/// @} APP

#endif // APP_HT_H_
