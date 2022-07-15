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

#if (BLE_DEVICE1_UART_CLIENT_ENABLE)
#include "rdw_mcu.h" // Task definitions
#include "prf_types.h"
#include "ke_task.h"
#include "rwip_task.h"

#include "def_self.h"
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
    UINT8 con_type;
    ///Existing handle values bps
    struct uart_bps_content bps;
	
//		UINT8  connection_index;
};

/// Parameters of the @ref DEVICE1_CLIENT_ENABLE_RSP message
struct device1_client_uart_enable_rsp
{
    ///status
    UINT8 status;
    ///Existing handle values bps
    struct uart_bps_content bps;
};


///Parameters of the @ref UART_CFG_INDNTF_REQ message
struct  uart_client_cfg_indntf_req
{
    ///Own code for differentiating between blood pressure and
    ///intermediate cuff pressure measurements
    UINT8 char_code;
    ///Stop/notify/indicate value to configure into the peer characteristic
    UINT16 cfg_val;
	
//		UINT16 connection_index;
};

///Parameters of the @ref UART_CFG_INDNTF_RSP message
struct uart_client_cfg_indntf_rsp
{
    ///Status
    UINT8  status;
};

///Parameters of the @ref UART_RD_TX_DATA_REQ message
struct uart_client_rd_char_req
{
    ///Characteristic value code
    UINT8  char_code;
};


///Parameters of the @ref UART_RX_DATA_IND message
struct uart_client_rx_data_ind
{
    UINT8  length;
		UINT8  value[255];
};

///Parameters of the @ref UART_CLIENT_TX_DATA_REQ message
struct uart_client_tx_data_req
{
    UINT8  length;
		UINT8  value[255];
	
//		unsigned int connection_index;
};

///Parameters of the @ref UART_CLIENT_TX_DATA_RSP message
struct uart_client_tx_data_rsp
{
    ///Status
    UINT8  status;
};


///Parameters of the @ref BLPC_CMP_EVT message
struct uart_client_cmp_evt
{
    /// Operation
    UINT8 operation;
    /// Status
    UINT8 status;
};


void device1_connector_uart_task_init(struct ke_task_desc *task_desc);

#endif

#endif
