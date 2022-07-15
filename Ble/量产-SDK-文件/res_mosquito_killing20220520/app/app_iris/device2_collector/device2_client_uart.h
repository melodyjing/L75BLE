/******************************************************************************
	File Name	:	device1_client_uart.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/5

	Description :   
			shanhai created
	Changed Log	:
		Liujian  2019/12/5			- Creation

*****************************************************************************/
#ifndef __DEVICE2_CLIENT_UART_H__
#define __DEVICE2_CLIENT_UART_H__

#include "rwip_config.h"

#if (BLE_DEVICE2_UART_CLIENT_ENABLE)
#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "device2_client_uart_task.h"

///Maximum number of UART Collector task instances
#define UART_COUNT_IDX_MAX    BLE_CONNECTION_MAX



/// Possible states of the UART task
enum
{
    /// Free state
    DEVICE2_UART_FREE,
    /// IDLE state
    DEVICE2_UART_IDLE,
    /// Discovering Blood Pressure SVC and CHars
    DEVICE2_UART_DISCOVERING,
    /// Busy state
    DEVICE2_UART_BUSY,

    /// Number of defined states.
    DEVICE2_UART_STATE_MAX
};


struct device2_uart_cnx_env
{
    /// Last requested UUID(to keep track of the two services and char)
    uint16_t last_uuid_req;
    /// Last characteristic code used in a read or a write request
    uint16_t last_char_code;
    /// Counter used to check service uniqueness
    uint8_t nb_svc;
    /// Current operation code
    uint8_t operation;
    ///HTS characteristics
    struct device2_uart_bps_content bps;
};


/// Uart Profile Collector environment variable
struct device2_uart_client_env_tag
{
    /// profile environment
    prf_env_t prf_env;
    /// Environment variable pointer for each connections
    struct device2_uart_cnx_env* env[UART_COUNT_IDX_MAX];
    /// State of different task instances
    ke_state_t state[UART_COUNT_IDX_MAX];
};







void device2_uart_client_enable_rsp_send(struct device2_uart_client_env_tag *device_uart_env, uint8_t conidx, uint8_t status);


const struct prf_task_cbs* device2_uart_collector_prf_itf_get(void);

#endif

#endif
