/******************************************************************************
	File Name	:	device1_client_uart.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/28

	Description :   

	Changed Log	:
		Liujian  2019/11/28			- Creation

*****************************************************************************/
#ifndef __MASTER_COM_H__
#define __MASTER_COM_H__

#include "rwip_config.h"

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))

#include "master_com_task.h"
#include "ke_task.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "prf.h"

///Maximum number of UART Collector task instances
//#define UART_COUNT_IDX_MAX    BLE_CONNECTION_MAX


///// Possible states of the UART task
//enum
//{
//    /// Free state
//    DEVICE1_UART_FREE,
//    /// IDLE state
//    DEVICE1_UART_IDLE,
//    /// Discovering Blood Pressure SVC and CHars
//    DEVICE1_UART_DISCOVERING,
//    /// Busy state
//    DEVICE1_UART_BUSY,

//    /// Number of defined states.
//    DEVICE1_UART_STATE_MAX
//};


//struct device1_uart_cnx_env
//{
//    /// Last requested UUID(to keep track of the two services and char)
//    uint16_t last_uuid_req;
//    /// Last characteristic code used in a read or a write request
//    uint16_t last_char_code;
//    /// Counter used to check service uniqueness
//    uint8_t nb_svc;
//    /// Current operation code
//    uint8_t operation;
//    ///HTS characteristics
//    struct uart_bps_content bps;
//};



///// Uart Profile Collector environment variable
//struct device1_uart_client_env_tag
//{
//    /// profile environment
//    prf_env_t prf_env;
//    /// Environment variable pointer for each connections
//    struct device1_uart_cnx_env* env[UART_COUNT_IDX_MAX];
//    /// State of different task instances
//    ke_state_t state[UART_COUNT_IDX_MAX];
//};




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
void device1_uart_client_send_cmp_evt(struct device1_uart_client_env_tag *p_uart_env, uint8_t conidx, uint8_t operation, uint8_t status);

void device1_uart_client_enable_rsp_send(struct device1_uart_client_env_tag *device_uart_env, uint8_t conidx, uint8_t status);


const struct prf_task_cbs* master_uart_collector_prf_itf_get(void);

#endif

#endif
