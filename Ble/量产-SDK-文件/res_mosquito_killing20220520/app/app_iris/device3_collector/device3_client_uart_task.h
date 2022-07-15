/******************************************************************************
	File Name	:	device3_client_uart_task.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/6

	Description :   

	Changed Log	:
		Liujian  2019/12/6			- Creation

*****************************************************************************/
#ifndef __DEVICE3_CLIENT_UART_TASK_H__
#define __DEVICE3_CLIENT_UART_TASK_H__

#if (BLE_DEVICE3_UART_CLIENT_ENABLE)
#include "rdw_mcu.h" // Task definitions
#include "prf_types.h"
#include "ke_task.h"
#include "rwip_task.h"


enum device3_client_uart_msg_id
{
    /// Start the uart profile - at connection
    DEVICE3_CLIENT_UART_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_DEVICE3_COLLECTOR_UART),
    ///Confirm that cfg connection has finished with discovery results, or that normal cnx started
    DEVICE3_CLIENT_ENABLE_RSP,

    /// Generic message to read a UART characteristic value
    DEVICE3_UART_RD_TX_CHAR_REQ,
    ///Generic message for read responses for APP
    DEVICE3_UART_RD_TX_CHAR_RSP,

    ///Generic message for configuring the 2 characteristics that can be handled
	  ///APP 要求改变配置要求,使能或停止 NTF
    DEVICE3_UART_CFG_INDNTF_REQ,
    ///Generic message for write characteristic response status to APP
    DEVICE3_UART_CFG_INDNTF_RSP,

		//发送数据到 server
	DEVICE3_UART_CLIENT_TX_DATA_REQ, 
	
	DEVICE3_UART_CLIENT_TX_DATA_RSP, 
	
//    /// UART receive value send to APP
    DEVICE3_UART_CLIENT_RX_DATA_IND,          //client receive data from server, and info APP
};


/// Characteristics
enum
{
    /// UART Tx charactor
    DEVICE3_UART_TX_CHAR,
	
    /// UART Rx charactor
    DEVICE3_UART_RX_CHAR,


    DEVICE3_UART_CHAR_MAX,
};


/// Characteristic descriptors
enum
{
    /// TX client config
    DEVICE3_UART_DESC_TX_CLI_CFG,
    /// RX client config
 //   DEVICE1_UART_DESC_RX_CLI_CFG,
    DEVICE3_UART_DESC_MAX,
    DEVICE3_UART_DESC_MASK = 0x10,
};


///Structure containing the characteristics handles, value handles and descriptors
struct device3_uart_bps_content
{
    /// service info
    struct prf_svc svc;

    /// characteristic info:

    struct prf_char_inf chars[DEVICE3_UART_CHAR_MAX];

    /// Descriptor handles:
    ///  - TX client cfg
    ///  - RX client cfg
    struct prf_char_desc_inf descs[DEVICE3_UART_DESC_MAX];
};


/// Parameters of the @ref DEVICE1_CLIENT_UART_ENABLE_REQ message
struct device3_client_uart_enable_req
{
    ///Connection type
    uint8_t con_type;
    ///Existing handle values bps
    struct device3_uart_bps_content bps;
	
		uint8_t connection_index;
};

/// Parameters of the @ref DEVICE1_CLIENT_ENABLE_RSP message
struct device3_client_uart_enable_rsp
{
    ///status
    uint8_t status;
    ///Existing handle values bps
    struct device3_uart_bps_content bps;
};


///Parameters of the @ref UART_CFG_INDNTF_REQ message
struct  device3_uart_client_cfg_indntf_req
{
    ///Own code for differentiating between blood pressure and
    ///intermediate cuff pressure measurements
    uint8_t char_code;
    ///Stop/notify/indicate value to configure into the peer characteristic
    uint16_t cfg_val;
	
	  uint8_t  connection_index;
};

///Parameters of the @ref UART_CFG_INDNTF_RSP message
struct device3_uart_client_cfg_indntf_rsp
{
    ///Status
    uint8_t  status;
};

///Parameters of the @ref UART_RD_TX_DATA_REQ message
struct device3_uart_client_rd_char_req
{
    ///Characteristic value code
    uint8_t  char_code;
};


///Parameters of the @ref UART_RX_DATA_IND message
struct device3_uart_client_rx_data_ind
{
    uint8_t  length;
		uint8_t  value[255];
};

///Parameters of the @ref UART_CLIENT_TX_DATA_REQ message
struct device3_uart_client_tx_data_req
{
    uint8_t  length;
		uint8_t  value[255];
		unsigned int connection_index;
};

///Parameters of the @ref UART_CLIENT_TX_DATA_RSP message
struct device3_uart_client_tx_data_rsp
{
    ///Status
    uint8_t  status;
};




void device3_connector_uart_task_init(struct ke_task_desc *task_desc);

#endif

#endif
