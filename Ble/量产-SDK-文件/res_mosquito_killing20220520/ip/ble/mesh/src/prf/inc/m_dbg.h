/**
 ****************************************************************************************
 *
 * @file m_dbg.h
 *
 * @brief Header file for Mesh Debug Module
 *
 * Copyright (C) Radiawave 2017-2021
 *
 ****************************************************************************************
 */

#ifndef M_DBG_
#define M_DBG_

/**
 ****************************************************************************************
 * @defgroup M_DBG Mesh Debug Layer
 * @ingroup MESH
 * @brief  Mesh Debug Layer
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "m_config.h"       // Mesh Configuration

#if (BLE_MESH_DBG)

#include "m_defines.h"      // Mesh Profile Definitions
#include "mesh_api_msg.h"   // Mesh Message Application Programming Interface
#include "mal.h"            // Mesh Abstraction layer

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Debug Command code that can be used
enum m_dbg_cmd_code
{
    /// Get Mesh stack configuration
    M_DBG_GET_CONFIG = 0,
    
    /* ************************************************************************* */
    /* *                                MODEL IO                               * */
    /* ************************************************************************* */
    /// Set publication set of a model instance
    M_DBG_MIO_SET_PUBLI_PARAM,
    /// Get publication set of a model instance
    M_DBG_MIO_GET_PUBLI_PARAM,
    /// Add an address in subscription list of a model
    M_DBG_MIO_ADD_ADDR_SUBS,
    /// Delete an address from subscription list of a model
    M_DBG_MIO_DEL_ADDR_SUBS,
    /// Clear content of subscription list
    M_DBG_MIO_CLEAR_SUBS,
    /// Get content of subscription list of a model
    M_DBG_MIO_GET_SUBS,
    /// Look for an address in the subscription list and return if present or not
    M_DBG_MIO_IS_ADDR_SUBS,
    /// Set unicast address of primary element
    M_DBG_MIO_SET_PRIM_ADDR,
    /// Get unicast address of primary element
    M_DBG_MIO_GET_PRIM_ADDR,
    
    /* ************************************************************************* */
    /* *                              KEY MANAGER                              * */
    /* ************************************************************************* */
    /// Add a new device key
    M_DBG_KEY_DEV_ADD,
    /// Delete a device key
    M_DBG_KEY_DEV_DELETE,
    /// Get a device key
    M_DBG_KEY_DEV_GET,
    /// Add a new network key
    M_DBG_KEY_NET_ADD,
    /// Update an existing network key
    M_DBG_KEY_NET_UPDATE,
    /// Delete a network key
    M_DBG_KEY_NET_DELETE,
    /// Use new key for transmission
    M_DBG_KEY_USE_NEW,
    /// Remove old key for reception
    M_DBG_KEY_REVOKE_OLD,
    /// Get a network key
    M_DBG_KEY_NET_GET,
    /// Get network key local index
    M_DBG_KEY_NET_GET_LID,
    /// Add a new application key
    M_DBG_KEY_APP_ADD,
    /// Update an existing application key
    M_DBG_KEY_APP_UPDATE,
     /// Delete an application key
    M_DBG_KEY_APP_DELETE,
    /// Get an application key
    M_DBG_KEY_APP_GET,
    /// Get application key local index
    M_DBG_KEY_APP_GET_LID,
    /// Bind a model to an application key
    M_DBG_KEY_MODEL_BIND,
    /// Unbind a model from an application key
    M_DBG_KEY_MODEL_UNBIND,
    /// Set IV and SEQ values
    M_DBG_KEY_SET_IV_SEQ,
    /// Get IV and SEQ values
    M_DBG_KEY_GET_IV_SEQ,
    /// Limit number of keys that can be stored to 1
    M_DBG_KEY_LIMIT_KEY_NB,
    /// Indicate that a friendship has been established
    M_DBG_KEY_FRIEND_START,
    /// Indicate that a friendship has been ended
    M_DBG_KEY_FRIEND_END,
    /// Get friendship security credential
    M_DBG_KEY_FRIEND_GET,
    /// Dump content of NID -> Network Key Local Index table for friendship security credentials
    M_DBG_KEY_FRIEND_DUMP_NID,
    
    /* ************************************************************************* */
    /* *                                 DATA                                  * */
    /* ************************************************************************* */
    /// Send data
    M_DBG_DATA_SEND,
    /// Receive data
    M_DBG_DATA_RX,
    /// Data Sent result
    M_DBG_DATA_SENT,

    /* ************************************************************************* */
    /* *                                LAYERS                                 * */
    /* ************************************************************************* */
    /// Configure connection (API or callback functions) for a given layer
    M_DBG_LAY_CONFIG,
    /// Disable check of destination address at network layer level
    M_DBG_LAY_DIS_CHECK_DST,
    /// Request bearer operation (open/close)
    M_DBG_LAY_BEARER_OPERATION,
    /// Control the bearer for layer that use bearer
    M_DBG_LAY_BEARER_CTL,

    /* ************************************************************************* */
    /* *                                BEACONING                              * */
    /* ************************************************************************* */
    /// Request beacon layer to start transmission of unprovisioned device beacons
    M_DBG_BCN_START_UNPROV,
    /// Request beacon layer to start transmission of secure network beacons
    M_DBG_BCN_START_PROV,

    /* ************************************************************************* */
    /* *                              PROVISIONING                             * */
    /* ************************************************************************* */
    /// Start provisioning
    M_DBG_PROV_START,

    /* ************************************************************************* */
    /* *                                 STATE                                 * */
    /* ************************************************************************* */
    /// Set state
    M_DBG_STATE_SET,
    /// Set time of last IV update
    M_DBG_STATE_SET_IV_UPD_TIME,
    /// Get state
    M_DBG_STATE_GET,

    /* ************************************************************************* */
    /* *                                STORAGE                                * */
    /* ************************************************************************* */
    /// Get NVDS content
    M_DBG_STORE_GET_NVDS,
    /// Set NVDS content
    M_DBG_STORE_SET_NVDS,
    /// Clean NVDS content
    M_DBG_STORE_CLEAN_NVDS,

    /* ************************************************************************* */
    /* *                            LOW POWER NODE                             * */
    /* ************************************************************************* */
    /// Simulate reception by Network Layer of a valid non control data PDU sent by
    /// the friend node
    M_DBG_LPN_NET_RX,

    /* ************************************************************************* */
    /* *                               FRIEND                                  * */
    /* ************************************************************************* */
    /// Get detailed info about a Low Power Node
    M_DBG_FRIEND_GET_LPN_INFO,
    /// Set Friend Counter value
    M_DBG_FRIEND_SET_FRIEND_CNT,
};

/// Layers that can be used by the debug module
enum m_dbg_layer
{
    /// Abstraction Layer
    M_DBG_LAYER_ABSTRACT = 0,
    /// Bearer Layer
    M_DBG_LAYER_BEARER,
    /// Network Layer
    M_DBG_LAYER_NETWORK,
    /// Lower Transport Layer
    M_DBG_LAYER_LTRANSPORT,
    /// Upper Transport Layer
    M_DBG_LAYER_UTRANSPORT,
    /// Access Layer
    M_DBG_LAYER_ACCESS,
    /// Beacon Layer
    M_DBG_LAYER_BEACON,
    /// Provisioning Layer
    M_DBG_LAYER_PROVISIONING,
    /// Friend Layer (Network interface)
    M_DBG_LAYER_FRIEND_NET,
};

/// Storage type
enum m_dbg_storage_type
{
    /// No storage
    M_DBG_STORAGE_TYPE_NONE = 0,
    /// WVT storage
    M_DBG_STORAGE_TYPE_WVT,
    /// NVDS storage
    M_DBG_STORAGE_TYPE_NVDS,
};

/// State types
enum m_dbg_state_type
{
     /// Provisioned state
     M_DBG_STATE_TYPE_PROV_STATE            = 0,
     /// Default TTL state
     M_DBG_STATE_TYPE_DEFAULT_TTL_STATE,
     /// Secure network beacon state
     M_DBG_STATE_TYPE_SEC_BCN_STATE,
     /// Network transmit state
     M_DBG_STATE_TYPE_NET_TX_STATE,
     /// Relay state
     M_DBG_STATE_TYPE_RELAY_STATE,
     /// GATT proxy state
     M_DBG_STATE_TYPE_GATT_PROXY_STATE,
     /// Friend state
     M_DBG_STATE_TYPE_FRIEND_STATE,
};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Debug command required structure (without parameters)
typedef struct m_dbg_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    uint32_t cmd_code;
} m_dbg_cmd_t;

/// Debug command complete event required structure (without parameters)
typedef struct m_dbg_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
} m_dbg_cmp_evt_t;

/// Get configuration complete event structure
typedef struct m_dbg_get_config_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Features (@see enum m_api_feat)
    uint32_t features;
    /// Size of subscription list for a model instance
    uint16_t subs_list_size;
    /// Number of supported virtual addresses
    uint8_t  nb_virt_addr;
    /// Maximum number of models
    uint8_t  nb_models;
    /// Maximum number of network keys
    uint8_t  nb_net_keys;
    /// Maximum number of application keys
    uint8_t  nb_app_keys;
    /// Maximum number of block buffers
    uint8_t  nb_buf_block;
    /// Size of long buffers
    uint16_t long_buf_size;
    /// Initial number of long buffers
    uint8_t  nb_long_buf;
    /// Initial number of small buffers
    uint8_t  nb_small_buf;
    /// Maximum number of opened bearers
    uint8_t  nb_bearer;
    /// Storage type (@see enum m_dbg_storage_type)
    uint8_t  storage_type;
    /// Maximum number of friendship that can be established
    uint8_t  nb_lpn_max;
} m_dbg_get_config_cmp_evt_t;

/* ******************************** */
/* *        MODEL IO TESTS        * */
/* ******************************** */

/// Set Model Publication Parameter Command structure
typedef struct m_dbg_mio_set_publi_param_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_SET_PUBLI_PARAM: Set publication parameters
    uint32_t cmd_code;

    /// Model LID
    uint8_t  model_lid;
    /// Local index of Application Key to be used for publication
    uint8_t  app_key_lid;
    /// Publish address
    uint16_t addr;
    /// Publish TTL
    uint8_t  ttl;
    /// Period for periodic status publishing
    uint8_t  period;
    /// Retransmission parameters
    /// Bit 0-2: Number of retransmissions for each message
    /// Bit 3-7: Number of 50-ms steps between retransmissions
    uint8_t  retx_params;
    /// Friendship credential flag
    uint8_t  friend_cred;
    /// Label UUID (in case address is a virtual address)
    uint8_t  label_uuid[M_LABEL_UUID_LEN];
} m_dbg_mio_set_publi_param_cmd_t;

/// Get Model Publication Parameters Command structure
typedef struct m_dbg_mio_get_publi_param_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_GET_PUBLI_PARAM: Get publication parameters
    uint32_t cmd_code;

    /// Model LID
    uint8_t  model_lid;
} m_dbg_mio_get_publi_param_cmd_t;

/// Get Model Publication Parameters Complete event structure
typedef struct m_dbg_mio_get_publi_param_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_GET_PUBLI_PARAM: Get publication parameters
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Model LID
    uint8_t  model_lid;
    /// Local index of Application Key to be used for publication
    uint8_t  app_key_lid;
    /// Publish address
    uint16_t addr;
    /// Publish TTL
    uint8_t  ttl;
    /// Period for periodic status publishing
    uint8_t  period;
    /// Retransmission parameters
    /// Bit 0-2: Number of retransmissions for each message
    /// Bit 3-7: Number of 50-ms steps between retransmissions
    uint8_t  retx_params;
    /// Friendship credential flag
    uint8_t  friend_cred;
} m_dbg_mio_get_publi_param_cmp_evt_t;

/// Add address in subscription list Command structure
typedef struct m_dbg_mio_add_addr_subs_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_ADD_ADDR_SUBS: Add address in subscription list
    uint32_t cmd_code;

    /// Address
    uint16_t addr;
    /// Model LID
    uint8_t  model_lid;
    /// Label UUID (in case address is a virtual address)
    uint8_t  label_uuid[M_LABEL_UUID_LEN];
} m_dbg_mio_add_addr_subs_cmd_t;

/// Delete address from subscription list Command structure
typedef struct m_dbg_mio_del_addr_subs_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_DEL_ADDR_SUBS: Delete address from subscription list
    uint32_t cmd_code;

    /// Address
    uint16_t addr;
    /// Model LID
    uint8_t  model_lid;
    /// Label UUID (in case address is a virtual address)
    uint8_t  label_uuid[M_LABEL_UUID_LEN];
} m_dbg_mio_del_addr_subs_cmd_t;

/// Get content of subscription list Command structure
typedef struct m_dbg_mio_get_subs_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_GET_SUBS: Get content of subscription list
    uint32_t cmd_code;

    /// Model LID
    uint8_t  model_lid;
} m_dbg_mio_get_subs_cmd_t;

/// Get content of subscription list Complete event structure
typedef struct m_dbg_mio_get_subs_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_GET_SUBS: Get content of subscription list
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Number of addresses in the list
    uint16_t nb_addr;
    /// Model LID
    uint8_t  model_lid;
    /// List of addresses
    uint16_t addr[__ARRAY_EMPTY];
} m_dbg_mio_get_subs_cmp_evt_t;

/// Clear content of subscription list Command structure
typedef struct m_dbg_mio_clear_subs_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_CLEAR_SUBS: Clear content of subscription list
    uint32_t cmd_code;

    /// Model LID
    uint8_t  model_lid;
} m_dbg_mio_clear_subs_cmd_t;

/// Look for address in the subscription list Command
typedef struct m_dbg_mio_is_addr_subs_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_IS_ADDR_SUBS: Look for address in subscription list
    uint32_t cmd_code;

    /// Address
    uint16_t addr;
    /// Model LID
    uint8_t  model_lid;
} m_dbg_mio_is_addr_subs_cmd_t;

/// Set device primary address
typedef struct m_dbg_mio_set_prim_addr_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_SET_PRIM_ADDR: Set device primary address
    uint32_t cmd_code;

    /// Primary address
    uint16_t prim_addr;
} m_dbg_mio_set_prim_addr_cmd_t;

/// Get device primary address command complete event structure
typedef struct m_dbg_mio_get_prim_addr_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_MIO_GET_PRIM_ADDR: Get primary element address
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Primary element address
    uint16_t prim_addr;
} m_dbg_mio_get_prim_addr_cmp_evt_t;

/* ******************************** */
/* *      KEY MANAGER TESTS       * */
/* ******************************** */

/// Add a new device key Command structure
typedef struct m_dbg_key_dev_add_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_DEV_ADD: Add a new device key
    uint32_t cmd_code;
    /// Key to add
    uint8_t  key[MESH_KEY_LEN];
} m_dbg_key_dev_add_cmd_t;
/// Add a new device key event structure - use m_dbg_cmp_evt

/// Delete a device key Command structure
typedef struct m_dbg_key_dev_delete_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_DEV_DELETE: Delete a device key
    uint32_t cmd_code;
} m_dbg_key_dev_delete_cmd_t;
/// Delete a new device key event structure - use m_dbg_cmp_evt

/// Add a new network key Command structure
typedef struct m_dbg_key_net_add_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_NET_ADD: Add a new network key
    uint32_t cmd_code;
    /// Network key index
    uint16_t net_key_id;
    /// Key to add
    uint8_t  key[MESH_KEY_LEN];
} m_dbg_key_net_add_cmd_t;

/// Add a new network key command complete event structure
typedef struct m_dbg_key_net_add_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_NET_ADD: Add a new network key
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Network key local identifier
    m_lid_t  net_key_lid;
} m_dbg_key_net_add_cmp_evt_t;

/// Update an existing network key Command structure
typedef struct m_dbg_key_net_update_add_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_NET_UPDATE: Update an existing network key
    uint32_t cmd_code;
    /// Network key index
    uint16_t net_key_id;
    /// Key to add
    uint8_t  key[MESH_KEY_LEN];
} m_dbg_key_net_update_cmd_t;

/// Update an existing network key event structure
typedef struct m_dbg_key_net_update_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_NET_UPDATE: Update an existing network key
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Network key local identifier
    m_lid_t  net_key_lid;
} m_dbg_key_net_update_cmp_evt_t;

/// Delete a network key Command structure
typedef struct m_dbg_key_net_delete_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_NET_DELETE: Delete a network key
    uint32_t cmd_code;
    /// Network key index
    uint16_t net_key_id;
} m_dbg_key_net_delete_cmd_t;
/// Delete a network key event structure - use m_dbg_cmp_evt

/// Force usage of new key for transmission
typedef struct m_dbg_key_use_new_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_USE_NEW: Use new key for transmission
    uint32_t cmd_code;
    /// Network key index
    uint16_t net_key_id;
} m_dbg_key_use_new_cmd_t;
/// Force usage of new key for transmission event structure - use m_dbg_cmp_evt

/// Remove the old key - only new key will be used for reception
typedef struct m_dbg_key_revoke_old_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_REVOKE_OLD: Remove old key for reception
    uint32_t cmd_code;
    /// Network key index
    uint16_t net_key_id;
} m_dbg_key_revoke_old_cmd_t;
///  Remove the old key - only new key will be used for reception event structure - use m_dbg_cmp_evt

/// Get a network key Command structure
typedef struct m_dbg_key_net_get_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_NET_GET: Get a network key
    uint32_t cmd_code;
    /// Network key local identifier
    m_lid_t  net_key_lid;
    /// Key for TX
    bool     tx;
} m_dbg_key_net_get_cmd_t;

/// Get a network key event structure
typedef struct m_dbg_key_net_get_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_NET_GET: Get a network key
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Network key local index
    m_lid_t  net_key_lid;
    /// Key state (@see enum m_tb_key_state)
    uint8_t  state;
    /// Key
    uint8_t  key[MESH_KEY_LEN];
    /// Network Encryption key value
    uint8_t  enc_key[MESH_KEY_LEN];
    /// Network privacy key value
    uint8_t  priv_key[MESH_KEY_LEN];
    /// Network beacon key value
    uint8_t  beacon_key[MESH_KEY_LEN];
    /// NID (Network Identifier)
    uint8_t  nid;
    /// Identity key value - proxy feature only
    uint8_t  identity[MESH_KEY_LEN];
    /// Network key index in network database (unique and manage by configuration)
    uint16_t net_key_id;
    /// Network ID
    uint8_t  net_id[M_NET_ID_LEN];
    /// Number of application keys currently bound with this network key
    uint8_t  nb_app_keys;
} m_dbg_key_net_get_cmp_evt_t;

/// Get network key local index command structure
typedef struct m_dbg_key_net_get_lid_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_NET_GET_LID: Get network key local index
    uint32_t cmd_code;
    /// Network key identifier
    uint16_t net_key_id;
} m_dbg_key_net_get_lid_cmd_t;

/// Get network key local index event structure
typedef struct m_dbg_key_net_get_lid_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_NET_GET_LID: Get network key local index
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Network key local index
    m_lid_t  net_key_lid;
} m_dbg_key_net_get_lid_cmp_evt_t;

/// Add a new application key Command structure
typedef struct m_dbg_key_app_add_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_APP_ADD: Add a new application key
    uint32_t cmd_code;
    /// Network key index
    uint16_t net_key_id;
    /// Application key index
    uint16_t app_key_id;
    /// Key to add
    uint8_t  key[MESH_KEY_LEN];
} m_dbg_key_app_add_cmd_t;

/// Add a new application key command complete event structure
typedef struct m_dbg_key_app_add_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_APP_ADD: Add a new application key key
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Application key local identifier
    m_lid_t  app_key_lid;
} m_dbg_key_app_add_cmp_evt_t;

/// Update an existing application key Command structure
typedef struct m_dbg_key_app_update_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_APP_UPDATE: Update an existing application key
    uint32_t cmd_code;
    /// Network key index
    uint16_t net_key_id;
    /// Application key index
    uint16_t app_key_id;
    /// Key to add
    uint8_t  key[MESH_KEY_LEN];
} m_dbg_key_app_update_cmd_t;

/// Update an existing application key event structure
typedef struct m_dbg_key_app_update_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_APP_UPDATE: Update an existing application key
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Application key local identifier
    m_lid_t  app_key_lid;
} m_dbg_key_app_update_cmp_evt_t;

/// Delete an application key Command structure
typedef struct m_dbg_key_app_delete_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_APP_DELETE: Delete an application key
    uint32_t cmd_code;
    /// Network key index
    uint16_t net_key_id;
    /// Application key index
    uint16_t app_key_id;
} m_dbg_key_app_delete_cmd_t;
/// Delete an application key event structure - use m_dbg_cmp_evt

/// Get a device key Command structure
typedef struct m_dbg_key_dev_get_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_DEV_GET: Get a device key
    uint32_t cmd_code;
} m_dbg_key_dev_get_cmd_t;

/// Get a device key event structure
typedef struct m_dbg_key_dev_get_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_DEV_GET: Get a device key
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Key
    uint8_t  key[MESH_KEY_LEN];
} m_dbg_key_dev_get_cmp_evt_t;

/// Get an application Command structure
typedef struct m_dbg_key_app_get_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_APP_GET: Get an application key
    uint32_t cmd_code;
    /// Application key local identifier
    m_lid_t  app_key_lid;
    /// 1 if usage is for TX purpose, 0 for RX purpose
    uint8_t  tx;
} m_dbg_key_app_get_cmd_t;

/// Get an application key event structure
typedef struct m_dbg_key_app_get_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_APP_GET: Get an application key
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Application key local index
    m_lid_t  app_key_lid;
    /// Key state (@see enum m_tb_key_state)
    uint8_t  state;
    /// Key
    uint8_t  key[MESH_KEY_LEN];
    /// Application identifier
    uint8_t  aid;
    /// Application key identifier
    uint16_t app_key_id;
    /// Network key identifier this application key is bound with
    uint16_t net_key_id;
    /// Number of models instances currently bound with this application key
    uint8_t  nb_models;
} m_dbg_key_app_get_cmp_evt_t;

/// Get application key local index command structure
typedef struct m_dbg_key_app_get_lid_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_APP_GET_LID: Get application key local index
    uint32_t cmd_code;
    /// Application key identifier
    uint16_t app_key_id;
} m_dbg_key_app_get_lid_cmd_t;

/// Get application key local index event structure
typedef struct m_dbg_key_app_get_lid_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_APP_GET_LID: Get application key local index
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Application key local index
    m_lid_t  app_key_lid;
} m_dbg_key_app_get_lid_cmp_evt_t;

/// Bind a model to an application key
typedef struct m_dbg_key_model_bind_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_MODEL_BIND: Bind a model to an application key
    uint32_t cmd_code;
    /// Application key index
    uint16_t app_key_id;
    /// Model Local Identifier
    m_lid_t  model_lid;
} m_dbg_key_model_bind_cmd_t;
/// Bind a model to an application key event structure - use m_dbg_cmp_evt


/// Unbind a model from an application key
typedef struct m_dbg_key_model_unbind_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_MODEL_UNBIND: Unbind a model from an application key
    uint32_t cmd_code;
    /// Application key index
    uint16_t app_key_id;
    /// Model Local Identifier
    m_lid_t  model_lid;
} m_dbg_key_model_unbind_cmd_t;
/// Unbind a model from an application key event structure - use m_dbg_cmp_evt

/// Set IV and SEQ values command structure
typedef struct m_dbg_key_set_iv_seq_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_SET_IV_SEQ: Set IV and SEQ values
    uint32_t cmd_code;
    /// IV
    uint32_t iv;
    /// Sequence number
    uint32_t seq;
} m_dbg_key_set_iv_seq_cmd_t;

/// Get sequence number command complete event structure
typedef struct m_dbg_key_get_iv_seq_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_GET_IV_SEQ: Get IV and SEQ values
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;
    /// Sequence number
    uint32_t seq;
    /// IV
    uint32_t iv;
} m_dbg_key_get_iv_seq_cmp_evt_t;

/// Start generation of security credentials from a friendship command structure
typedef struct m_dbg_key_friend_start_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_FRIEND_START: Start generation of security credentials from a friendship
    uint32_t cmd_code;

    /// Friendship local index
    m_lid_t  friend_lid;
    /// Friend address
    uint16_t friend_addr;
    /// Friend counter
    uint16_t friend_cnt;
    /// Low Power Node address
    uint16_t lpn_addr;
    /// Low Power Node counter
    uint16_t lpn_cnt;
} m_dbg_key_friend_start_cmd_t;

/// End use of security credentials from a friendship command structure
typedef struct m_dbg_key_friend_end_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_FRIEND_END: End use of security credentials from a friendship
    uint32_t cmd_code;

    /// Friendship local index
    m_lid_t  friend_lid;
} m_dbg_key_friend_end_cmd_t;

/// Get friendship security credentials command structure
typedef struct m_dbg_key_friend_get_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_FRIEND_GET: Get Friendship Security Credentials
    uint32_t cmd_code;

    /// Friendship local index
    m_lid_t friend_lid;
    /// Network key local index
    m_lid_t net_key_lid;
    /// Credentials used for transmission (true) or not (false)
    bool    tx;
} m_dbg_key_friend_get_cmd_t;

/// Get friendship security credentials command complete event structure
typedef struct m_dbg_key_friend_get_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_FRIEND_GET: Get Friendship Security Credentials
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Friendship local index
    m_lid_t friend_lid;
    /// Network key local index
    m_lid_t net_key_lid;
    /// Friend encryption key
    uint8_t enc_key[MESH_KEY_LEN];
    /// Friend privacy key
    uint8_t priv_key[MESH_KEY_LEN];
    /// Friend NID
    uint8_t nid;
} m_dbg_key_friend_get_cmp_evt_t;

/// Dump content of NID -> Network Key Local Index for friendship security credentials
/// command complete event structure
typedef struct m_dbg_key_friend_dump_nid_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_KEY_FRIEND_DUMP_NID: Dump content of NID -> Network Key Local Index for friendship
    /// security credentials
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Number of NIDs
    uint16_t nb_nids;
    /// Table content
    uint8_t  table[__ARRAY_EMPTY];
} m_dbg_key_friend_dump_nid_cmp_evt_t;

/* ******************************** */
/* *          DATA TESTS          * */
/* ******************************** */

/// Send data command structure
typedef struct m_dbg_data_send_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_DATA_SEND: Send data
    uint32_t cmd_code;

    /// Layer to which data will be inserted (@see enum m_dbg_layer)
    uint8_t  layer;
    /// Data Length
    uint16_t data_len;
    /// Environment Length
    uint16_t env_len;
    /// Environment + Data
    uint8_t  env_data[__ARRAY_EMPTY];
} m_dbg_data_send_cmd_t;

/// Receive data command structure
typedef struct m_dbg_data_rx_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_DATA_RX: Receive data
    uint32_t cmd_code;

    /// Layer to which data will be inserted (@see enum m_dbg_layer)
    uint8_t  layer;
    /// Data Length
    uint16_t data_len;
    /// Environment Length
    uint16_t env_len;
    /// Environment + Data
    uint8_t  env_data[__ARRAY_EMPTY];
} m_dbg_data_rx_cmd_t;

/// Sent data command structure
typedef struct m_dbg_data_sent_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_DATA_SENT: Data Sent result
    uint32_t cmd_code;
    /// Layer to which data will be inserted (@see enum m_dbg_layer)
    uint8_t  layer;
    /// Status of the Sent execution
    uint16_t status;
} m_dbg_data_sent_cmd_t;

/// Debug data received indication structure
typedef struct m_dbg_data_rx_ind
{
    /// Layer (@see m_dbg_layer)
    uint8_t layer;
    /// Data length
    uint16_t data_len;
    /// Environment length
    uint16_t env_len;
    /// Environment + Data
    uint8_t env_data[__ARRAY_EMPTY];
} m_dbg_data_rx_ind_t;

/// Debug data send indication structure
typedef struct m_dbg_data_send_ind
{
    /// Layer (@see m_dbg_layer)
    uint8_t layer;
    /// Data length
    uint16_t data_len;
    /// Environment length
    uint16_t env_len;
    /// Environment + Data
    uint8_t env_data[__ARRAY_EMPTY];
} m_dbg_data_send_ind_t;

/// Debug data sent indication structure
typedef struct m_dbg_data_sent_ind
{
    /// Layer (@see m_dbg_layer)
    uint8_t layer;
    /// Status
    uint16_t status;
} m_dbg_data_sent_ind_t;

/// Configure layer command structure
typedef struct m_dbg_lay_config_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_LAY_CONFIG: Configure layer
    uint32_t cmd_code;

    /// Layer (@see enum m_dbg_layer)
    uint8_t  layer;
    /// True if debug module must modify callback function used by the layer, False if API functions
    /// must be modified.
    bool     cb;
    /// Connect (use debug functions) or disconnect (use layer functions)
    bool     connect;
} m_dbg_lay_config_cmd_t;

/// Control the bearer for layer that use bearer
/// Default command complete sent back
typedef struct m_dbg_lay_bearer_ctl_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_LAY_CONFIG: Configure layer
    uint32_t cmd_code;
    /// Layer (@see enum m_dbg_layer)
    uint8_t  layer;
    /// Bearer Local identifier
    m_lid_t  bearer_lid;
    /// Bearer type
    uint8_t  bearer_type;
    /// True to open bearer, False to close it
    bool     open;
} m_dbg_lay_bearer_ctl_cmd_t;

/// Request bearer operation (open or close)
typedef struct m_dbg_lay_bearer_operation_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_LAY_BEARER_OPERATION: Request bearer operation
    uint32_t cmd_code;
    /// Operation type (@see m_bearer_op_type)
    uint8_t op_type;
} m_dbg_lay_bearer_operation_cmd_t;

/// Delay start/stop scan response command structure
/// NO COMMAND COMPLETE EVENT SENT FOR THIS COMMAND
typedef struct m_dbg_lay_delay_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_LAY_DELAY
    uint32_t            cmd_code;

    /// Bearer local identifier
    m_lid_t             bearer_lid;
    /// Callback function to call upon reception of this command
    mal_adv_scan_op_cb cb;
} m_dbg_lay_delay_cmd_t;

/* ******************************** */
/* *        BEACONING TESTS         */
/* ******************************** */

/// Request beacon layer to start transmission of unprovisioned device beacons
typedef struct m_dbg_bcn_start_unprov_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_BCN_START_UNPROV: Start transmission of unprovisioned beacons
    uint32_t cmd_code;

    /// Device unique identifier
    uint8_t  dev_uuid[MESH_DEV_UUID_LEN];
    /// Out of band information
    uint16_t oob_info;
    /// URI hash
    uint32_t uri_hash;
    /// URI hash present
    bool     uri_hash_present;
} m_dbg_bcn_start_unprov_cmd_t;

/* ******************************** */
/* *          STATE TESTS           */
/* ******************************** */

/// Set state value command structure
typedef struct m_dbg_state_set_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_STATE_SET: Set state value
    uint32_t cmd_code;

    /// State type (@see enum m_dbg_state_type)
    uint8_t  type;
    /// Length
    uint8_t  length;
    /// Value
    uint8_t  val[__ARRAY_EMPTY];
} m_dbg_state_set_cmd_t;

/// Set time of last IV update command structure
typedef struct m_dbg_state_set_iv_upd_time_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_STATE_SET_IV_UPD_TIME: Set last IV update time
    uint32_t cmd_code;

    /// Time in milliseconds
    uint32_t time_ms;
    /// Number of times the value above has wrapped
    uint16_t nb_wrap;
    /// Status
    uint8_t status;
} m_dbg_state_set_iv_upd_time_cmd_t;

/// Get state value command structure
typedef struct m_dbg_state_get_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_STATE_GET: Get state value
    uint32_t cmd_code;

    /// State type (@see enum m_dbg_state_type)
    uint8_t type;
} m_dbg_state_get_cmd_t;

/// Get state value command complete event structure
typedef struct m_dbg_state_get_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
        /// - M_DBG_STATE_GET: Get state value
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// State type (@see enum m_dbg_state_type)
    uint8_t type;
    /// State value
    union
    {
        /// Network Transmit
        uint8_t net_tx;
    } u;
} m_dbg_state_get_cmp_evt_t;

/* ******************************** */
/* *         STORAGE TESTS          */
/* ******************************** */

/// Get NVDS content command structure
typedef struct m_dbg_store_get_nvds_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_STORE_GET_NVDS: Get NVDS content
    uint32_t cmd_code;

    /// Tag
    uint8_t  tag;
} m_dbg_store_get_nvds_cmd_t;

/// Get NVDS content command complete event structure
typedef struct m_dbg_store_get_nvds_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_STORE_GET_NVDS: Get NVDS content
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Tag
    uint8_t  tag;
    /// Length
    uint8_t  length;
    /// Data
    uint8_t  data[__ARRAY_EMPTY];
} m_dbg_store_get_nvds_cmp_evt_t;

/// Set NVDS content command structure
typedef struct m_dbg_store_set_nvds_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_STORE_SET_NVDS: Set NVDS content
    uint32_t cmd_code;

    /// Tag
    uint8_t  tag;
    /// Length
    uint8_t  length;
    /// Data
    uint8_t  data[__ARRAY_EMPTY];
} m_dbg_store_set_nvds_cmd_t;

/// Debug bearer opened indication structure
typedef struct m_dbg_bearer_opened_ind
{
    /// Bearer local index
    m_lid_t bearer_lid;
    /// Bearer type
    uint8_t type;
    /// Targeted layer (network, beacon or provisioning)
    uint8_t layer;
} m_dbg_bearer_opened_ind_t;

/// Debug bearer closed indication structure
typedef struct m_dbg_bearer_closed_ind
{
    /// Bearer LID
    m_lid_t bearer_lid;
    /// Targeted layer (network, beacon or provisioning)
    uint8_t layer;
} m_dbg_bearer_closed_ind_t;

/* ******************************** */
/* *     LOW POWER NODE TESTS       */
/* ******************************** */

/// Simulate reception by Network Layer command structure
typedef struct m_dbg_lpn_net_rx_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_LPN_NET_RX: Simulate reception by Network Layer of a valid non control data PDU sent by
    /// the friend node
    uint32_t cmd_code;

    /// Friend address
    uint16_t friend_addr;;
} m_dbg_lpn_net_rx_cmd_t;

/* ******************************** */
/* *          FRIEND TESTS          */
/* ******************************** */

/// Debug Friendship indication structure
typedef struct m_dbg_friend_lpn_ind
{
    /// Low Power Node address
    uint16_t lpn_addr;
    /// Low Power Node local index
    uint8_t  lpn_lid;
    /// Event (@see enum m_tb_friend_event)
    uint8_t  event;
} m_dbg_friend_lpn_ind_t;

/// Get information about a Low Power Node command structure
typedef struct m_dbg_friend_get_lpn_info_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_FRIEND_GET_LPN_INFO: Get information about a Low Power Node
    uint32_t cmd_code;

    /// Low Power Node local index
    uint8_t  lpn_lid;
} m_dbg_friend_get_lpn_info_cmd_t;

/// Get information about a Low Power Node command complete event structure
typedef struct m_dbg_friend_get_lpn_info_cmp_evt
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_FRIEND_GET_LPN_INFO: Get information about a Low Power Node
    uint32_t cmd_code;
    /// Status of the command execution
    uint16_t status;

    /// Low Power Node address
    uint16_t lpn_addr;
    /// Poll Timeout in milliseconds
    uint32_t poll_timeout_ms;
    /// Low Power Node counter
    uint16_t lpn_cnt;
    /// Friend counter
    uint16_t friend_cnt;
    /// Receive delay in microseconds
    uint8_t  rx_delay_ms;
    /// Local index
    uint8_t  lpn_lid;
    /// Number of elements on LPN side
    uint8_t  nb_elements;
    /// Current number of addresses in the subscription list
    uint8_t  nb_addr;
    /// Subscription list
    uint16_t subs_list[__ARRAY_EMPTY];
} m_dbg_friend_get_lpn_info_cmp_evt_t;

/// Set Friend Counter value command structure
typedef struct m_dbg_friend_set_friend_cnt_cmd
{
    /// Debug Command code (@see enum m_dbg_cmd_code)
    /// - M_DBG_FRIEND_SET_FRIEND_CNT: Set Friend Counter value
    uint32_t cmd_code;

    /// Friend counter
    uint16_t friend_cnt;
} m_dbg_friend_set_friend_cnt_cmd_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Mesh debug layer initialization
 *
 * @note called when Mesh stack is initialized or when a SW reset is requested.
 *
 * @param[in] reset True means SW reset, False means task initialization
 * @param[in] p_env Pointer to the environment structure
 * @param[in] p_cfg Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this module.
 ****************************************************************************************
 */
uint16_t m_dbg_init(bool reset, void* p_env, const m_cfg_t* p_cfg);

/**
 ****************************************************************************************
 * @brief Mesh debug module memory requirement
 *
 * @note this function is called before init function in order to know memory requirements of the module
 *
 * @param[in] p_cfg    Pointer to mesh stack initialization parameters
 *
 * @return Size of environment required for this mesh module
 ****************************************************************************************
 */
uint16_t m_dbg_get_env_size(const m_cfg_t* p_cfg);

/**
 ****************************************************************************************
 * @brief Debug message hander
 *
 * @param[in] msg_id  Identifier of the message received
 * @param[in] src_id  Identifier of the task that issue message
 * @param[in] p_param Message parameters
 *
 * @return Status of message after execution of the handler (@see enum mal_msg_status)
 ****************************************************************************************
 */
uint8_t m_dbg_msg_handler(uint16_t msg_id, uint16_t src_id, const void *p_param);

#if (BLE_MESH_FRIEND)
/**
 ****************************************************************************************
 * @brief Inform the application about update of a friendship status for a given Low Power Node
 *
 * @param[in] lpn_addr      Low Power Node address
 * @param[in] lpn_lid       Friendship local index
 * @param[in] event         Event
 ****************************************************************************************
 */
void m_dbg_friend_event_ind(uint16_t lpn_addr, uint8_t lpn_lid, uint8_t event);
#endif //(BLE_MESH_FRIEND)

#endif //(BLE_MESH_DBG)

/// @} M_DBG

#endif /* M_DBG_ */
