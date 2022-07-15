/******************************************************************************
	File Name	:	voice_master_task.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/07

	Description :   连接声音模块的 host profile task

	Changed Log	:
		Liujian  2020/03/07		- Creation

*****************************************************************************/
#ifndef __VOICE_MASTER_TASK_H__
#define __VOICE_MASTER_TASK_H__
#include "rdw_mcu.h" // Task definitions
#include "prf_types.h"
#include "ke_task.h"
#include "rwip_task.h"
#include "voice_master.h"

#if BLE_APP_RECEIVE_VOICE
#include "co_utils.h"
#include "gap.h"
#include "att.h"
#include "gattc_task.h"
#include "gapc_task.h"
#include "ke_mem.h"
#include "ke_task.h"
#include <stdio.h>
#include <string.h>
#include "cobra_prf.h"

enum voice_master_msg_id
{
    /// Start the uart profile - at connection
    VOICE_MASTER_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_VOICE_MASTER),
    ///Confirm that cfg connection has finished with discovery results, or that normal cnx started
    VOICE_MASTER_ENABLE_RSP,

    /// Generic message to read a UART characteristic value
    VOICE_MASTER_START_CHAR_REQ,
    ///Generic message for read responses for APP
    VOICE_MASTER_START_CHAR_RSP,

    VOICE_MASTER_CFG_INDNTF_REQ,
    ///Generic message for write characteristic response status to APP
    VOICE_MASTER_CFG_INDNTF_RSP,


		VOICE_MASTER_DATA_CHAR_REQ, 
	
		VOICE_MASTER_DATA_CHAR_RSP, 
//	

    VOICE_MASTER_RX_DATA_IND,          //client receive data from server, and info APP
	
};



/// Characteristics
enum
{
    
    VOICE_MASTER_BEGIN_CHAR,
	

    VOICE_MASTER_DATA_CHAR,


    VOICE_MASTER_CHAR_MAX,
};

/// Characteristic descriptors
enum
{
    /// voice begin client config
    VOICE_MASTER_DESC_BEGIN_CLI_CFG,
    /// voice data client config
    VOICE_MASTER_DESC_DATA_CLI_CFG,
	
    VOICE_MASTER_DESC_MAX,
    VOICE_MASTER_DESC_MASK = 0x10,
};


///Structure containing the characteristics handles, value handles and descriptors
struct voice_master_bsp_content
{
    /// service info
    struct prf_svc svc;
    /// characteristic info:
    struct prf_char_inf chars[VOICE_MASTER_CHAR_MAX];     //保留有多少个character
    /// Descriptor handles:
    struct prf_char_desc_inf descs[VOICE_MASTER_DESC_MAX];    //多少个描述符
};


struct voice_master_cnx_env
{
    /// Last requested UUID(to keep track of the two services and char)
    uint16_t last_uuid_req;
    /// Last characteristic code used in a read or a write request
    uint16_t last_char_code;
    /// Counter used to check service uniqueness
    uint8_t nb_svc;
    /// Current operation code
    uint8_t operation;

		struct voice_master_bsp_content bps;
};

/// Parameters of the @ref VOICE_MASTER_ENABLE_REQ message
struct voice_master_enable_req
{
    ///Connection type
    uint8_t con_type;
    ///Existing handle values bps
    struct voice_master_bsp_content bps;
	
		uint8_t  connection_index;
};


/// Parameters of the @ref VOICE_MASTER_ENABLE_RSP message
struct voice_master_enable_rsp
{
    ///status
    uint8_t status;
    ///Existing handle values bps
    struct voice_master_bsp_content bps;
};


///Parameters of the @ref UART_CFG_INDNTF_REQ message
struct  voice_master_cfg_indntf_req
{
    ///Own code for differentiating between blood pressure and
    ///intermediate cuff pressure measurements
    uint8_t char_code;
    ///Stop/notify/indicate value to configure into the peer characteristic
    uint16_t cfg_val;
	
		uint16_t connection_index;
};

///Parameters of the @ref UART_CFG_INDNTF_RSP message
struct voice_master_cfg_indntf_rsp
{
    ///Status
    uint8_t  status;
};

///Parameters of the @ref VOICE_MASTER_RX_DATA_IND message
struct voice_master_rx_voice_data_ind
{
    uint8_t  length;
		uint8_t  value[255];
};


void voice_master_task_init(struct ke_task_desc *task_desc);

#endif
#endif

