/******************************************************************************
	File Name	:	ota_master_task.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/26

	Description :   
	    用于发现 OTA 设备, 这个是 Master, 用来连接 OTA  设备

	Changed Log	:
		Liujian  2020/8/26		- Creation

*****************************************************************************/
#ifndef __OTA_MASTER_TASK_H__
#define __OTA_MASTER_TASK_H__

#include "rdw_mcu.h" // Task definitions
#include "prf_types.h"
#include "ke_task.h"
#include "rwip_task.h"
#include "ota_master.h"
#include "co_utils.h"
#include "gap.h"
#include "att.h"
#include "gattc_task.h"
#include "gapc_task.h"
#include "ke_mem.h"
#include "ke_task.h"
#include <stdio.h>
#include <string.h>
//#include "cobra_prf.h"
#if (APP_OTA_MATSER_ENABLE == 1)

enum ota_master_msg_id
{
    /// Start the uart profile - at connection
    OTA_MASTER_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_OTA_MASTER),
    ///Confirm that cfg connection has finished with discovery results, or that normal cnx started
    OTA_MASTER_ENABLE_RSP,
	
	  /// Read the value of an attribute in the peer device database
    OTA_MASTER_READ_CMD,                                 
     /// Write the value of an attribute in the peer device database
    OTA_MASTER_WRITE_CMD, 
	
    /// Complete Event Information
	  OTA_MASTER_CMP_EVT,
	
};

/// Operation Codes
enum  ota_master_op_codes
{
    /// Reserved operation code
     OTA_RESERVED_OP_CODE  = 0x00,

    /// Discovery Procedure
     OTA_ENABLE_OP_CODE,
    /// Read attribute value Procedure
     OTA_READ_OP_CODE,
    /// Write attribute value Procedure
     OTA_WRITE_OP_CODE,
};



/// Characteristics
enum
{
    OTA_MEM_DEV_CHAR,    //SUOTA_MEM_DEV_UUID
	
    OTA_GPIO_MAP_CHAR,    //SUOTA_GPIO_MAP_UUID

    OTA_MEM_INFO_CHAR,   //SUOTA_MEM_INFO_UUID
	
	  OTA_PATCH_LEN_CHAR,   //SUOTA_PATCH_LEN_UUID
	
	  OTA_PATCH_DATA_CHAR,   //SUOTA_PATCH_DATA_UUID
	
	  OTA_SERV_STATUS_CHAR,   //SUOTA_SERV_STATUS_UUID
	
	  OTA_VERSION_CHAR,                 //SUOTA_VERSION_UUID
	
	  OTA_PATCH_DATA_CHAR_SIZE_CHAR,   //SUOTA_PATCH_DATA_CHAR_SIZE_UUID
	
	  OTA_MTU_CHAR,                     //SUOTA_MTU_UUID
	
    OTA_MASTER_CHAR_MAX,
};


/// Characteristic descriptors
enum
{
    /// ota serv status config
    OTA_MASTER_DESC_SERV_STATUS_CLI_CFG,
    OTA_MASTER_DESC_MAX,
	
    OTA_MASTER_DESC_MASK = 0x10,
};


///Structure containing the characteristics handles, value handles and descriptors
struct  ota_master_bsp_content
{
    /// service info
    struct prf_svc svc;
    /// characteristic info:
    struct prf_char_inf chars[OTA_MASTER_CHAR_MAX];     //保留有多少个character
    /// Descriptor handles:
    struct prf_char_desc_inf descs[OTA_MASTER_CHAR_MAX];    //多少个描述符
};


struct ota_master_cnx_env
{
    /// Last requested UUID(to keep track of the two services and char)
    uint16_t last_uuid_req;
    /// Last characteristic code used in a read or a write request
    uint16_t last_char_code;
    /// Counter used to check service uniqueness
    uint8_t nb_svc;
    /// Current operation code
    uint8_t operation;

		struct ota_master_bsp_content bps;
};


/// Parameters of the @ref OTA_MASTER_ENABLE_REQ message
struct ota_master_enable_req
{
    ///Connection type
    uint8_t con_type;
    ///Existing handle values bps
    struct ota_master_bsp_content bps;
	
		uint8_t  connection_index;
};


/// Parameters of the @ref OTA_MASTER_ENABLE_RSP message
struct ota_master_enable_rsp
{
    ///status
    uint8_t status;
    ///Existing handle values bps
    struct ota_master_bsp_content bps;
};


/// Parameters of the @ref OTA_MASTER_READ_CMD message
struct ota_master_read_cmd
{
    /// Read code
    uint8_t read_code;
};

/// Parameters of the @ref OTA_MASTER_WRITE_CMD message
struct ota_master_write_cmd
{
    /// Write code
    uint8_t write_code;
	
	  uint8_t data_length;
    /// Value
    union write_value_tag
    {
			  uint32_t   ota_mem_dev_info;
        /// patch len
        uint16_t   ota_patch_len;
        /// patch data
        uint16_t   ota_patch_data[OTA_PD_CHAR_SIZE];			
        /// patch_status Setting characteristic client characteristic configuration value
        uint16_t   patch_status_setting_ntf_cfg;
    } value;
};



/// Internal codes for reading/writing a OTA characteristic with one single request
enum
{
    /// Read MEM DEV 
    OTA_RD_MEM_DEV              = OTA_MEM_DEV_CHAR,
    /// Read OTA GPIO setting
    OTA_RD_GPIO_MAP            =  OTA_GPIO_MAP_CHAR,
    /// Read OTA memeory information
    OTA_RD_MEM_INFO            =  OTA_MEM_INFO_CHAR,
	
	  OTA_WD_PATCH_LEN           =  OTA_PATCH_LEN_CHAR,
	
	  OTA_WD_PATCH_DATA          =  OTA_PATCH_DATA_CHAR,
	
	  OTA_WD_MEM_DEV             =  OTA_MEM_DEV_CHAR,

    /// Read/Write SERV_STATUS Status Client Characteristic Configuration Descriptor
    OTA_RD_WR_SERV_STATUS_CFG       = (OTA_MASTER_DESC_SERV_STATUS_CLI_CFG | OTA_MASTER_DESC_MASK),

};


void ota_master_task_init(struct ke_task_desc *task_desc);


#endif

#endif
