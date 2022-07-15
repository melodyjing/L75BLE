/**
 ****************************************************************************************
 *
 * @file app.h
 *
 * @brief Application entry point
 *
 * Copyright (C) RADIAWAVE  2017-2020
 *
 *
 ****************************************************************************************
 */

#ifndef APP_H_
#define APP_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_PRESENT)

#include <stdint.h>          // Standard Integer Definition
#include <co_bt.h>           // Common BT Definitions
#include "arch.h"            // Platform Definitions
#include "gapc.h"            // GAPC Definitions
#include "rdw_mcu.h"
#if (NVDS_SUPPORT)
#include "nvds.h"
#endif // (NVDS_SUPPORT)

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximal length of the Device Name value
#define APP_DEVICE_NAME_MAX_LEN      (18)

/*
 * MACROS
 ****************************************************************************************
 */

#define APP_HANDLERS(subtask)    {&subtask##_msg_handler_list[0], ARRAY_LEN(subtask##_msg_handler_list)}

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

#if (NVDS_SUPPORT)
/// List of Application NVDS TAG identifiers

// /// Application specific
//    PARAM_ID_APP_SPECIFIC_FIRST         = 0x2B,
//    PARAM_ID_APP_SPECIFIC_LAST          = 0x2C,

enum app_nvds_tag
{
    /// BD Address
    NVDS_TAG_BD_ADDRESS                 = 0x01,
    NVDS_LEN_BD_ADDRESS                 = 6,

    /// Device Name
    NVDS_TAG_DEVICE_NAME                = 0x02,
    NVDS_LEN_DEVICE_NAME                = 62,

    /// BLE Application Advertising data
    NVDS_TAG_APP_BLE_ADV_DATA           = 0x37,
    NVDS_LEN_APP_BLE_ADV_DATA           = 32,

    /// BLE Application Scan response data
    NVDS_TAG_APP_BLE_SCAN_RESP_DATA     = 0x38,
    NVDS_LEN_APP_BLE_SCAN_RESP_DATA     = 32,

    /// Mouse Sample Rate
    NVDS_TAG_MOUSE_SAMPLE_RATE          = 0x39,
    NVDS_LEN_MOUSE_SAMPLE_RATE          = 1,

    /// Peripheral Bonded
    NVDS_TAG_PERIPH_BONDED              = 0x3A,
    NVDS_LEN_PERIPH_BONDED              = 1,

    /// Mouse NTF Cfg
    NVDS_TAG_MOUSE_NTF_CFG              = 0x3B,
    NVDS_LEN_MOUSE_NTF_CFG              = 2,

    /// Mouse Timeout value
    NVDS_TAG_MOUSE_TIMEOUT              = 0x3C,
    NVDS_LEN_MOUSE_TIMEOUT              = 2,

    /// Peer Device BD Address
    NVDS_TAG_PEER_BD_ADDRESS            = 0x3D,
    NVDS_LEN_PEER_BD_ADDRESS            = 7,

    /// Mouse Energy Safe
    NVDS_TAG_MOUSE_ENERGY_SAFE          = 0x3E,
    NVDS_LEN_MOUSE_SAFE_ENERGY          = 2,

    /// EDIV (2bytes), RAND NB (8bytes),  LTK (16 bytes), Key Size (1 byte)
    NVDS_TAG_LTK                        = 0x3F,
    NVDS_LEN_LTK                        = 28,

    /// PAIRING
    NVDS_TAG_PAIRING                    = 0x40,
    NVDS_LEN_PAIRING                    = 54,

    /// Audio mode 0 task
    NVDS_TAG_AM0_FIRST                  = 0x41,
    NVDS_TAG_AM0_LAST                   = 0x51,

    /// Local device Identity resolving key
    NVDS_TAG_LOC_IRK                    = 0x52,
    NVDS_LEN_LOC_IRK                    = KEY_LEN,

    /// Peer device Resolving identity key (+identity address)
    NVDS_TAG_PEER_IRK                   = 0x53,
    NVDS_LEN_PEER_IRK                   = sizeof(struct gapc_irk),
};
#endif // (NVDS_SUPPORT)

/// Advertising state machine
enum app_adv_state
{
    /// Advertising activity does not exists
    APP_ADV_STATE_IDLE = 0,
    /// Creating advertising activity
    APP_ADV_STATE_CREATING,
    /// Setting advertising data
    APP_ADV_STATE_SETTING_ADV_DATA,
    /// Setting scan response data
    APP_ADV_STATE_SETTING_SCAN_RSP_DATA,

    /// Advertising activity created
    APP_ADV_STATE_CREATED,
    /// Starting advertising activity
    APP_ADV_STATE_STARTING,
    /// Advertising activity started
    APP_ADV_STATE_STARTED,
    /// Stopping advertising activity
    APP_ADV_STATE_STOPPING,

};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure containing information about the handlers for an application subtask
struct app_subtask_handlers
{
    /// Pointer to the message handler table
    const struct ke_msg_handler *p_msg_handler_tab;
    /// Number of messages handled
    uint16_t msg_cnt;
};

/// Application environment structure
struct app_env_tag
{
    /// Connection handle
    uint16_t conhdl;
    /// Connection Index
    uint8_t  conidx;

    /// Advertising activity index
    uint8_t adv_actv_idx;
    /// Current advertising state (@see enum app_adv_state)
    uint8_t adv_state;
    /// Next expected operation completed event
    uint8_t adv_op;

    /// Last initialized profile
    uint8_t next_svc;

    /// Bonding status
    bool bonded;

    /// Device Name length
    uint8_t dev_name_len;
    /// Device Name
    uint8_t dev_name[APP_DEVICE_NAME_MAX_LEN];

    /// Local device IRK
    uint8_t loc_irk[KEY_LEN];

    /// Secure Connections on current link
    bool sec_con_enabled;

    /// Counter used to generate IRK
    uint8_t rand_cnt;
		
		uint8_t adv_ok;            //是否在广播
};

struct app_host_uart_env_tag
{
	  
		uint8_t  receive_data[COBRA4_MTU];
	
		uint8_t  receive_index;    //要拷贝的数据的地址
	
		uint8_t  receive_begin;
	
		uint8_t  receive_end;
	
	  uint8_t  receive_length;
	
		uint8_t  connection_index;
	
		uint16_t  need_data_number;  //需要的 698 数据个数
	
};


/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */

/// Application environment
extern struct app_env_tag app_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the BLE demo application.
 ****************************************************************************************
 */
void appm_init(void);

/**
 ****************************************************************************************
 * @brief Add a required service in the database
 ****************************************************************************************
 */
bool appm_add_svc(void);


/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
void appm_adv_fsm_next(void);

/**
 ****************************************************************************************
 * @brief Send to request to update the connection parameters
 ****************************************************************************************
 */
//void appm_update_param(struct gapc_conn_param *conn_param);

void appm_update_param(struct gapc_conn_param *conn_param, ke_task_id_t app_task);
void host_appm_update_param(struct gapc_conn_param *conn_param, ke_task_id_t app_task);
/**
 ****************************************************************************************
 * @brief Send a disconnection request
 ****************************************************************************************
 */
void appm_disconnect(void);

/**
 ****************************************************************************************
 * @brief Retrieve device name
 *
 * @param[out] device name
 *
 * @return name length
 ****************************************************************************************
 */
uint8_t appm_get_dev_name(uint8_t* name);

/**
 ****************************************************************************************
 * @brief Start/stop advertising
 *
 * @param[in] start     True if advertising has to be started, else false
 ****************************************************************************************
 */
void appm_update_adv_state(bool start);

/**
 ****************************************************************************************
 * @brief delete advertising
 *
 * @param[in] none
 ****************************************************************************************
 */

void appm_delete_advertising(void);
/**
 ****************************************************************************************
 * @brief Return if the device is currently bonded
 ****************************************************************************************
 */
bool app_sec_get_bond_status(void);


void appm_send_gapm_reset_cmd(void);

void app_collection_2_ready(void);

unsigned int app_collection_2_is_ready(void);

void app_get_collection2_address(unsigned char *p_buffer);

void app_iris_collection2_address(struct bd_addr * p_collection_address);
//比较地址是否是采集器2 的地址
unsigned int app_device_compare_collection2_address(unsigned char * source_addr);

void app_collection_2_not_ready(void);

void appm_reset_adv_data(void);

void appm_ble_update_mtu(uint8_t idx);

#if (BLE_APP_MESH_IOS_ENABLE == 1)
//IOS 需要清掉 service 的 cache
void appm_svc_changed_command(void);
#endif
/// @} APP

#endif //(BLE_APP_PRESENT)

#endif // APP_H_
