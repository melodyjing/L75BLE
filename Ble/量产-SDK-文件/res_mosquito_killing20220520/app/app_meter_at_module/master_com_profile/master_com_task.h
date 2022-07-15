/******************************************************************************
	File Name	:	device1_client_uart_task.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/11/28

	Description :   

	Changed Log	:
		Liujian  2019/11/28			- Creation

*****************************************************************************/
#ifndef __DEVICE1_CLIENT_UART_TASK_H__
#define __DEVICE1_CLIENT_UART_TASK_H__

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))

#include "rdw_mcu.h" // Task definitions
#include "prf_types.h"
#include "ke_task.h"
#include "rwip_task.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "prf.h"

enum device1_client_uart_msg_id
{
    /// Start the uart profile - at connection
    DEVICE1_CLIENT_UART_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_DEVICE1_COLLECTOR_UART),
    ///Confirm that cfg connection has finished with discovery results, or that normal cnx started
    DEVICE1_CLIENT_ENABLE_RSP,

    ///Generic message for configuring the 2 characteristics that can be handled
	  ///APP 要求改变配置要求,使能或停止 NTF
    UART_CFG_INDNTF_REQ,

		//发送数据到 server
		DEVICE1_UART_CLIENT_TX_DATA_REQ, 
	
    /// UART receive value send to APP
    UART_CLIENT_RX_DATA_IND,          //client receive data from server, and info APP
		
		UART_CLIENT_CMP_EVT,
};


/// Characteristics
enum
{
    /// UART Tx charactor
    DEVICE1_UART_TX_CHAR,
	
    /// UART Rx charactor
    DEVICE1_UART_RX_CHAR,


    DEVICE1_UART_CHAR_MAX,
};


/// Characteristic descriptors
enum
{
    /// TX client config
    DEVICE1_UART_DESC_TX_CLI_CFG,
    /// RX client config
 //   DEVICE1_UART_DESC_RX_CLI_CFG,
    DEVICE1_UART_DESC_MAX,
    DEVICE1_UART_DESC_MASK = 0x10,
};


/// Operation codes
enum uart_op_codes
{
    /// Database Creation Procedure
    UART_RESERVED_OP_CODE = 0,

    /// Configure CCC Operation Code
    UART_CFG_INDNTF_CMD_OP_CODE = 1,

    /// send data Operation Code
    UART_TX_DATA_CMD_OP_CODE = 2,
};


///Structure containing the characteristics handles, value handles and descriptors
struct uart_bps_content
{
    /// service info
    struct prf_svc svc;

    /// characteristic info:

    struct prf_char_inf chars[DEVICE1_UART_CHAR_MAX];

    /// Descriptor handles:
    ///  - TX client cfg
    ///  - RX client cfg
    struct prf_char_desc_inf descs[DEVICE1_UART_DESC_MAX];
};


/// Parameters of the @ref DEVICE1_CLIENT_UART_ENABLE_REQ message
struct device1_client_uart_enable_req
{
    ///Connection type
    uint8_t con_type;
    ///Existing handle values bps
    struct uart_bps_content bps;
	
//		UINT8  connection_index;
};

/// Parameters of the @ref DEVICE1_CLIENT_ENABLE_RSP message
struct device1_client_uart_enable_rsp
{
    ///status
    uint8_t status;
    ///Existing handle values bps
    struct uart_bps_content bps;
};


///Parameters of the @ref UART_CFG_INDNTF_REQ message
struct  uart_client_cfg_indntf_req
{
    ///Own code for differentiating between blood pressure and
    ///intermediate cuff pressure measurements
    uint8_t char_code;
    ///Stop/notify/indicate value to configure into the peer characteristic
    uint16_t cfg_val;
	
//		UINT16 connection_index;
};

///Parameters of the @ref UART_CFG_INDNTF_RSP message
struct uart_client_cfg_indntf_rsp
{
    ///Status
    uint8_t  status;
};

///Parameters of the @ref UART_RD_TX_DATA_REQ message
struct uart_client_rd_char_req
{
    ///Characteristic value code
    uint8_t  char_code;
};


///Parameters of the @ref UART_RX_DATA_IND message
struct uart_client_rx_data_ind
{
    uint8_t  length;
		uint8_t  value[255];
};

///Parameters of the @ref UART_CLIENT_TX_DATA_REQ message
struct uart_client_tx_data_req
{
    uint8_t  length;
		uint8_t  value[255];
	
//		unsigned int connection_index;
};

///Parameters of the @ref UART_CLIENT_TX_DATA_RSP message
struct uart_client_tx_data_rsp
{
    ///Status
    uint8_t  status;
};


///Parameters of the @ref BLPC_CMP_EVT message
struct uart_client_cmp_evt
{
    /// Operation
    uint8_t operation;
    /// Status
    uint8_t status;
};

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
    uint16_t last_uuid_req;
    /// Last characteristic code used in a read or a write request
    uint16_t last_char_code;
    /// Counter used to check service uniqueness
    uint8_t nb_svc;
    /// Current operation code
    uint8_t operation;
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



void device1_connector_uart_task_init(struct ke_task_desc *task_desc);

#endif

#endif
