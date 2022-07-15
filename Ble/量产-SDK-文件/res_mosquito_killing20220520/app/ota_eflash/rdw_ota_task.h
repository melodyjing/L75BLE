// 参考 Dialog 的代码
//liujian
//2018/4/17
//ver1.0

//2020/6/2 
//liujian Ver2.0

#ifndef RDW_OTA_TASK_H__
#define RDW_OTA_TASK_H__



/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h" // Task definitions
#include "rdw_mcu.h"



enum  ota_msg_id
{
    OTA_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_OTA),//!< 
    /// Enable confirmation
    OTA_ENABLE_RSP,                               //!< 

	OTA_PATCH_MEM_INFO_UPDATE_REQ,

	OTA_STATUS_UPDATE_REQ,
	
	OTA_PATCH_MEM_DEV_IND,
	
	OTA_GPIO_MAP_IND,
	
	OTA_PATCH_DATA_IND,

	OTA_PATCH_LEN_IND,
	


};


struct ota_enable_req
{
    /// Connection index
    uint8_t conidx;
    /// Notification configuration (Bond Data to restore: @see )
    uint8_t  ntf_ind_cfg;
};

/// Parameters of the @ref OTA_ENABLE_RSP message
struct ota_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status of enable request
    uint8_t status;
};

/// Parameters of the @ref OTA_PATCH_MEM_DEV_IND message
struct ota_patch_mem_dev_ind
{
    /// Connection handle
    uint16_t conhdl;

    /// Char Code - Indicate whether patch data written successfully
    uint8_t char_code;

    uint32_t mem_dev;
};

/// Parameters of the @ref OTA_GPIO_MAP_IND message
struct ota_gpio_map_ind
{
    /// Connection handle
    uint16_t conhdl;

    /// Char Code - Indicate whether patch data written successfully
    uint8_t char_code;

    uint32_t gpio_map;
};


/// Parameters of the @ref OTA_PATCH_LEN_IND message
struct ota_patch_len_ind
{
    /// Connection handle
    uint16_t conhdl;

    /// Char Code - Indicate whether patch data written successfully
    uint8_t char_code;

    uint16_t len;
};

/// Parameters of the @ref OTA_PATCH_DATA_IND message
struct ota_patch_data_ind
{
    /// Connection handle
    uint16_t conhdl;

    /// Char Code - Indicate whether patch data written successfully
    uint8_t char_code;

    uint8_t len;

    uint8_t pd[__ARRAY_EMPTY];
};

/// Parameters of the @ref OTA_PATCH_MEM_INFO_UPDATE_REQ message
struct ota_patch_mem_info_update_req
{
    /// Memory info: 16MSbits show number of patches, 16LSbits overall mem len
    uint32_t mem_info;
};

/// Parameters of the @ref OTA_STATUS_UPDATE_REQ message

struct ota_status_update_req
{
    /// Connection handle
    uint16_t conhdl;

    /// SUOTAR Status
    uint8_t status;
};

struct ota_db_cfg
{
  
    uint8_t features;
 

};

enum  ota_ntf_ind_cfg
{

    OTA_PATCH_STATUS_VAL_IND    = (1 << 0),
    
};
	 




void app_ota_add_otas(void);

void ota_task_init(struct ke_task_desc *task_desc);















#endif 
