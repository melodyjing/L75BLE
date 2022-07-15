// 参考 Dialog 的代码
//liujian
//2018/4/17
//ver1.0

#ifndef RDW_OTA_H__
#define RDW_OTA_H__


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_OTA_PROFILE)
#include <stdint.h>
#include <stdbool.h>
#include "prf.h"
#include "prf_types.h"
#include "prf_utils.h"

#include "gap.h"
#if (DEBUG_CFG)
#define  OTA_DBG      1
#endif
#define  OTA_PROFILE_MASK               (0x003F)

#define  OTA_IDX_MAX    (1)

/// Macro used to convert 16-bits UUID to byte array
#define ATT_UUID_16_ARR(uuid)                           {(uint8_t)(uuid), (uint8_t)(uuid >> 8)}
/// @} ATTMCFG

#if OTA_VERSION >= OTA_VERSION_1_3
#define SUOTA_PD_CHAR_SIZE      244
#else
#define SUOTA_PD_CHAR_SIZE      20
#endif


enum ota_state
{
    /// Idle state
    OTA_IDLE,
    /// Busy state
    OTA_BUSY,
    /// Number of defined states.
    OTA_STATE_MAX
};


/// SUOTA Handles offsets
enum
{
    SUOTA_IDX_SVC,

    SUOTA_IDX_PATCH_MEM_DEV_CHAR,
    SUOTA_IDX_PATCH_MEM_DEV_VAL,
//    SUOTA_IDX_PATCH_MEM_DEV_VAL_DESCR,

    SUOTA_IDX_GPIO_MAP_CHAR,
    SUOTA_IDX_GPIO_MAP_VAL,
//    SUOTA_IDX_GPIO_MAP_VAL_DESCR,

    SUOTA_IDX_PATCH_MEM_INFO_CHAR,
    SUOTA_IDX_PATCH_MEM_INFO_VAL,
//    SUOTA_IDX_PATCH_MEM_INFO_VAL_DESCR,

    SUOTA_IDX_PATCH_LEN_CHAR,
    SUOTA_IDX_PATCH_LEN_VAL,
//    SUOTA_IDX_PATCH_LEN_VAL_DESCR,

    SUOTA_IDX_PATCH_DATA_CHAR,
    SUOTA_IDX_PATCH_DATA_VAL,
//    SUOTA_IDX_PATCH_DATA_VAL_DESCR,

    SUOTA_IDX_PATCH_STATUS_CHAR,
    SUOTA_IDX_PATCH_STATUS_VAL,
    SUOTA_IDX_PATCH_STATUS_NTF_CFG,
//    SUOTA_IDX_PATCH_STATUS_VAL_DESCR,

#if OTA_VERSION >= OTA_VERSION_1_3
    SUOTA_IDX_VERSION_CHAR,
    SUOTA_IDX_VERSION_VAL,

    SUOTA_IDX_PATCH_DATA_CHAR_SIZE_CHAR,
    SUOTA_IDX_PATCH_DATA_CHAR_SIZE_VAL,

    SUOTA_IDX_MTU_CHAR,
    SUOTA_IDX_MTU_VAL,
#endif

    SUOTA_IDX_NB,
};

///Characteristics Code for Write Indications
enum
{
    OTA_ERR_CHAR,
    OTA_PATCH_MEM_DEV_CHAR,
    OTA_GPIO_MAP_CHAR,
    OTA_PATCH_LEN_CHAR,
    OTA_PATCH_DATA_CHAR,
    OTA_PATCH_STATUS_NTF_CFG,
};

/// SUOTA Status values
enum
{
    SUOTAR_RESERVED         = 0x00,     // Value zero must not be used !! Notifications are sent when status changes.
    SUOTAR_SRV_STARTED      = 0x01,     // Valid memory device has been configured by initiator. No sleep state while in this mode
    SUOTAR_CMP_OK           = 0x02,     // SUOTA process completed successfully.
    SUOTAR_SRV_EXIT         = 0x03,     // Forced exit of SUOTAR service.
    SUOTAR_CRC_ERR          = 0x04,     // Overall Patch Data CRC failed
    SUOTAR_PATCH_LEN_ERR    = 0x05,     // Received patch Length not equal to PATCH_LEN characteristic value
    SUOTAR_EXT_MEM_WRITE_ERR= 0x06,     // External Mem Error (Writing to external device failed)
    SUOTAR_INT_MEM_ERR      = 0x07,     // Internal Mem Error (not enough space for Patch)
    SUOTAR_INVAL_MEM_TYPE   = 0x08,     // Invalid memory device
    SUOTAR_APP_ERROR        = 0x09,     // Application error

    // SUOTAR application specific error codes
    SUOTAR_IMG_STARTED      = 0x10,     // SUOTA started for downloading image (SUOTA application)
    SUOTAR_INVAL_IMG_BANK   = 0x11,     // Invalid image bank
    SUOTAR_INVAL_IMG_HDR    = 0x12,     // Invalid image header
    SUOTAR_INVAL_IMG_SIZE   = 0x13,     // Invalid image size
    SUOTAR_INVAL_PRODUCT_HDR= 0x14,     // Invalid product header
    SUOTAR_SAME_IMG_ERR     = 0x15,     // Same Image Error
    SUOTAR_EXT_MEM_READ_ERR = 0x16,     // Failed to read from external memory device
};


/// ongoing operation information
struct ota_op
{
     /// Operation
     uint8_t op;
     /// Cursor on connection
     uint8_t cursor;
     /// Handle of the attribute to indicate/notify
     uint16_t handle;
     /// Task that request the operation that should receive completed message response
     uint16_t dest_id;
     /// Packed notification/indication data size
     uint8_t length;
     /// used to know on which device interval update has been requested, and to prevent
     /// indication to be triggered on this connection index
     uint8_t conidx;
     /// Packed notification/indication data
     uint8_t data[__ARRAY_EMPTY];
};




struct ota_env_tag
{
	 /// profile environment
    prf_env_t prf_env;
    /// On-going operation
    struct ota_op * operation;
    /// Service Start Handle
    uint16_t shdl;
    /// Database configuration
    uint16_t features;

    /// Notification and indication configuration of peer devices.
    uint8_t ntf_ind_cfg[BLE_CONNECTION_MAX];
    /// State of different task instances
    ke_state_t state[OTA_IDX_MAX];
	
//For OTA use
	uint32_t  mem_info; 
	uint8_t   patch_status_value;
	uint32_t  patch_mem_dev_value;
	uint32_t  gpio_map_value;
	uint32_t  PATCH_DATA_VAL;
	uint16_t  patch_len_value;
	
	uint32_t  write_number;
	
};


const struct prf_task_cbs* ota_prf_itf_get(void);

void  ota_exe_operation(void);

prf_env_t* prf_env_get_ota(uint16_t prf_id);



#endif 



#endif 
