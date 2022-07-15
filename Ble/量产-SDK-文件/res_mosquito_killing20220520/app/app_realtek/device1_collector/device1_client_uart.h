/******************************************************************************
	File Name	:	device1_client_uart.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/28

	Description :   

	Changed Log	:
		Liujian  2019/11/28			- Creation

*****************************************************************************/
#ifndef __DEVICE1_CLIENT_UART_H__
#define __DEVICE1_CLIENT_UART_H__

#include "rwip_config.h"

#if (BLE_DEVICE1_UART_CLIENT_ENABLE)
#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "device1_client_uart_task.h"
#include "def_self.h"
///Maximum number of UART Collector task instances
#define UART_COUNT_IDX_MAX    BLE_CONNECTION_MAX



/// Possible states of the UART task
enum
{
    /// Free state
    DEVICE1_UART_FREE,
    /// IDLE state
    DEVICE1_UART_IDLE,
    /// Discovering Blood Pressure SVC and CHars
    DEVICE1_UART_DISCOVERING,
    /// Busy state
    DEVICE1_UART_BUSY,

    /// Number of defined states.
    DEVICE1_UART_STATE_MAX
};


struct device1_uart_cnx_env
{
    /// Last requested UUID(to keep track of the two services and char)
    UINT16 last_uuid_req;
    /// Last characteristic code used in a read or a write request
    UINT16 last_char_code;
    /// Counter used to check service uniqueness
    UINT8 nb_svc;
    /// Current operation code
    UINT8 operation;
    ///HTS characteristics
    struct uart_bps_content bps;
};


/// Uart Profile Collector environment variable
struct device1_uart_client_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct device1_uart_cnx_env* env[UART_COUNT_IDX_MAX];
    /// State of different task instances
    ke_state_t state[UART_COUNT_IDX_MAX];
};




/**
 ****************************************************************************************
 * @brief Send an UART_CLIENT_CMP_EVT message to a requester.
 *
 * @param[in] p_uart_env    UART Client environment variable
 * @param[in] conidx        Connection index
 * @param[in] operation     Code of the completed operation
 * @param[in] status        Status of the request
 ****************************************************************************************
 */
void device1_uart_client_send_cmp_evt(struct device1_uart_client_env_tag *p_uart_env, UINT8 conidx, UINT8 operation, UINT8 status);

void device1_uart_client_enable_rsp_send(struct device1_uart_client_env_tag *device_uart_env, UINT8 conidx, UINT8 status);


CONST struct prf_task_cbs* device1_uart_collector_prf_itf_get(void);

#endif

#endif
