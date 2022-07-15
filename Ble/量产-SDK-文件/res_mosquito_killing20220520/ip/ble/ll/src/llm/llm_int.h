/**
 ****************************************************************************************
 *
 * @file llm.h
 *
 * @brief Link layer manager internal definition
 *
 * Copyright (C) Radiawave 2017-2021
 *
 ****************************************************************************************
 */

#ifndef LLM_INT_H_
#define LLM_INT_H_

/**
 ****************************************************************************************
 * @addtogroup LLM Link layer Manager
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>          // standard integer
#include <stdbool.h>         // standard boolean

#include "co_bt.h"           // BT defines
#include "sch_plan.h"        // Scheduling Planner

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of instances of the LLM task
#define LLM_IDX_MAX                       1

/// Invalid activity identifier
#define LLM_ACT_IDX_INVL     0xFF

/// Invalid advertising handle (assigned to legacy advertising for internal management)
#define LLM_ADV_HDL_INVL     0xFF

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/**
 * Information bit field for advertising duplicate filtering
 *       7:6           5:4            3:2       1      0
 * +-------------+-------------+-------------+------+------+
 * |             | CHAIN_CODED |  CHAIN_1M   | RST  |  EN  |
 * +-------------+-------------+-------------+------+------+
 */
enum adv_dup_filt_info
{
    /// Duplicate filtering enabled
    LLM_ADV_DUP_FILT_EN_POS            = 0,
    LLM_ADV_DUP_FILT_EN_BIT            = (1 << 0),

    /// Duplicate filtering reset at each scan period
    LLM_ADV_DUP_FILT_RST_POS           = 1,
    LLM_ADV_DUP_FILT_RST_BIT           = (1 << 1),

    /// Advertising report chain status from 1M scan
    LLM_ADV_DUP_FILT_CHAIN_1M_LSB      = 2,
    LLM_ADV_DUP_FILT_CHAIN_1M_MASK     = (0x3 << 2),

    /// Advertising report chain status from coded scan
    LLM_ADV_DUP_FILT_CHAIN_CODED_LSB   = 4,
    LLM_ADV_DUP_FILT_CHAIN_CODED_MASK  = (0x3 << 4),

    /**
     * 0: No chain
     * 1: A valid advertising report chain is ongoing
     * 2: A filtered advertising report chain is ongoing
     */
    LLM_ADV_CHAIN_NO         = 0,
    LLM_ADV_CHAIN_VALID      = 1,
    LLM_ADV_CHAIN_FILTERED   = 2,
};

/// Status bits for a device list entry
///
///    7      6      5      4      3       2       1      0
/// +------+------+------+------+------+-------+-------+------+
/// |      |      |      |      |      | IN_PL | IN_WL | USED |
/// +------+------+------+------+------+-------+-------+------+
///
enum dev_list_stat
{
    /// Device list entry is used
    LLM_DEV_LIST_ENTRY_USED_POS         = 0,
    LLM_DEV_LIST_ENTRY_USED_BIT         = (1 << 0),

    /// Device is in white list
    LLM_DEV_IN_WL_POS                   = 1,
    LLM_DEV_IN_WL_BIT                   = (1 << 1),

    /// Device is in periodic advertiser list
    LLM_DEV_IN_PL_POS                   = 2,
    LLM_DEV_IN_PL_BIT                   = (1 << 2),
};

/// Advertising interface version, see HCI:3.1.1
enum adv_itf_version
{
    /// Interface not yet detected
    LLM_ADV_ITF_UNKNOWN = 0,
    /// Legacy interface
    LLM_ADV_ITF_LEGACY,
    /// Extended interface
    LLM_ADV_ITF_EXTENDED,
};

/// States of an activity
enum LLM_ACTIVITY_STATE
{
    LLM_FREE,
    LLM_ADV_RSVD,
    LLM_ADV_EN,
    LLM_ADV_STOPPING,
    LLM_CON_RSVD,
    LLM_INITIATING,
    LLM_SCAN_RSVD,
    LLM_SCAN_EN,
    LLM_SCAN_STOPPING,
    LLM_CONNECTED,
    LLM_PER_ADV_RSVD,
    LLM_PER_ADV_PENDING, // This state means that the host has enabled per adv but the driver has not been started yet
    LLM_PER_ADV_EN,
    LLM_PER_ADV_STOPPING,
    LLM_PER_SCAN_SYNCING,
    LLM_PER_SCAN_SYNCED,
    LLM_PER_SCAN_STOPPING,

    #if (BLE_CIS)
    LLM_CIS_RSVD,
    LLM_CIS_EN,
    #endif // (BLE_CIS)
    #if (BLE_BIS)
    LLM_BIS_RSVD,
    LLM_BIS_EN,
    #endif // (BLE_BIS)
};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Device list entry structure
struct dev_list_entry
{
    /// BD address
    struct bd_addr addr;

    /// Advertising Set IDs (for periodic advertiser list), 1 bit per ADV_SID
    uint16_t adv_sids;

    /** Address type
     * 0x00 Public Device Address or Public Identity Address
     * 0x01 Random Device Address or Random (static) Identity Address
     * 0xFF Address type of devices sending anonymous advertisements
     */
    uint8_t addr_type;

    /** Bit field indicating:
     * bit 0: entry is used
     * bit 1: device is in white list
     * bit 2: device is in periodic advertiser list
     */
    uint8_t status;
};

/// Information about one single advertiser discovered
struct adv_dup_filt_entry
{
    /// Advertising device address
    struct bd_addr adv_addr;

    /// Advertising Data ID (if Set ID is valid)
    uint16_t did;

    /// Periodic advertising interval
    uint16_t per_adv_intv;

    /**
     * Bit
     * 0        Connectable advertising
     * 1        Scannable advertising
     * 2        Directed advertising
     * 3        Scan response
     * 4        Legacy advertising PDUs used
     */
    uint8_t adv_evt_type;

    /**
     * Peer address type
     * 0x00 Public Device Address
     * 0x01 Random Device Address
     * 0x02 Public Identity Address (Corresponds to Resolved Private Address)
     * 0x03 Random (static) Identity Address (Corresponds to Resolved Private Address)
     * 0x04-0xFF Reserved for future use
     */
    uint8_t addr_type;

    /// Advertising Set ID
    uint8_t sid;
};

#if (BLE_OBSERVER)
/// Advertising duplicate filtering table information structure
struct adv_dup_filt_tab_info
{
    /// Table listing all found advertisers
    struct adv_dup_filt_entry  tab[BLE_DUPLICATE_FILTER_MAX];

    /// Current position in the table for placing the next discovered advertiser
    uint8_t curr_pos;

    /// Number of discovered advertisers in the table
    uint8_t nb_adv;
};
#endif //(BLE_OBSERVER)

/// Channel map parameters structure
struct llm_ch_map_info
{
    /// Channel classification from Host
    struct le_chnl_map host_ch_class;

    /// Master channel map (map used for all master links)
    struct le_chnl_map master_ch_map;

    /// Channel map management state
    uint8_t active;

    /**
     * Block the HCI LE Set Host Channel Classification Command during 1 second, as per standard
     * The interval between two successive commands sent shall be at least one second.
     */
    bool new_host_class;
};

/// Information about extended advertising data or scan response data received from Host
struct llm_adv_tx_info
{
    /// EM buffer containing complete advertising or extended scan response data received from Host
    uint16_t curr_buf;

    /// Total length of advertising or scan response data in the current buffer
    uint16_t curr_len;

    /// EM buffer containing advertising data or extended scan response data in construction from Host
    uint16_t new_buf;

    /// Total length of advertising or scan response data in the new buffer
    uint16_t new_len;
};

/// Specific information for advertiser
struct llm_adv_info
{
    /// Information about extended advertising data received from Host
    struct llm_adv_tx_info          adv_data;

    /// Information about extended scan response data received from Host
    struct llm_adv_tx_info          scan_rsp_data;

    /// Selected extended advertising Tx power (selected by LLM, applied by LLD)
    int8_t                          tx_pwr;

     /// Per adv linked to the set: ID of the periodic advertising
    uint8_t                         per_act_id;

    /// The ID of the resulting connection if advertising is connectable
    uint8_t                         con_act_id;
};

/// Specific information for periodic advertiser
struct llm_per_adv_info
{
    /// Information about extended advertising data received from Host
    struct llm_adv_tx_info          adv_data;

    /**
     *  Associated activity ID:
     *    - BIG Master linked to periodic advertising
     */
    uint8_t                         ass_act_id;
};

/// Specific information for periodic synchronization
struct llm_sync_info
{
    /// Advertising SID (for periodic scanning)
    uint8_t                         adv_sid;

    // Periodic Sync ACAD data transfer:
    /// Adv data type to transfer
    uint8_t                         ad_type;
    /// Task identifier that expect the advertising data.
    /// - TASK_NONE ==> means no activity expect ACAD data
    uint16_t                        dst_id;
};

/// Specific information for connection
struct llm_con_info
{
    /// Role (0x00: MASTER / 0x01: SLAVE) (for connection)
    uint8_t                         role;
};

/// Specific information for scanning
struct llm_scan_info
{
    ///Scan duration (Time=N*10ms)  | 0x0000: Scan continuously until explicitly disable
    uint16_t            duration;
    ///Scan period (Time=N*1.28sec) | 0x0000: Periodic scanning disabled
    uint16_t            period;
};


/// State for all activities
struct llm_act_info
{
    /// Pointer to parameters received from the Host (advertising, scanning or initiating)
    void const *                    host_params;

    /**
     * BD address field used for several situations:
     *
     * Advertising:     local random address
     * Scanning:        not used
     * Initiating:      local random address
     * Per Adv:         not used
     * Per Adv Sync:    peer device identity Address
     * Connection:      peer device identity Address
     */
    struct bd_addr                  bd_addr;

    /// Interval element linked to this activity
    struct sch_plan_elt_tag    plan_elt;

    union
    {
        /// Scanning specific activity info
        struct llm_scan_info  scan;
        /// Advertising specific activity info
        struct llm_adv_info  adv;
        /// Periodic advertising specific activity info
        struct llm_per_adv_info  per_adv;
        /// Periodic sync specific activity info
        struct llm_sync_info sync;
        /// Connection specific activity info
        struct llm_con_info  con;
    } info;


    /// State
    uint8_t                         state;

    /**
     * Peer device address type (for connection)
     * 0x00 Public Device Address (default)
     * 0x01 Random Device Address
     * 0x02 Controller generates Resolvable Private Address based on the local IRK from resolving list. If resolving
     *  list contains no matching entry, use public address.
     * 0x03 Controller generates Resolvable Private Address based on the local IRK from resolving list. If resolving
     *  list contains no matching entry, use random address from LE_Set_Random_Address.
     */
    uint8_t                         addr_type;

};

/// LLM environment structure
struct llm_env_tag
{
    /// LE Event mask
    struct evt_mask                     le_event_mask;

    /// State and parameters for all activities
    struct llm_act_info                 act_info[BLE_ACTIVITY_MAX];

    /// Local public BD address
    struct bd_addr                      local_pub_addr;

    /// Local random BD address
    struct bd_addr                      local_rand_addr;

    /// Channel map management parameters
    struct llm_ch_map_info              ch_map_info;

    /// Device list information table
    struct dev_list_entry dev_list[BLE_WHITELIST_MAX];

    #if (BLE_CENTRAL || BLE_PERIPHERAL)
    /// Default preferences for a link
    link_pref_t                         pref;
    #endif //(BLE_CENTRAL || BLE_PERIPHERAL)

    #if (BLE_MESH || BLE_CENTRAL || BLE_PERIPHERAL)
    #if (SECURE_CONNECTIONS)
    /// Private P256 key for secure connection
    uint8_t                             secret_key256[PRIV_KEY_256_LEN];
    #endif // (SECURE_CONNECTIONS)
    #endif // (BLE_MESH || BLE_CENTRAL || BLE_PERIPHERAL)

    /// Resolvable private addresses renew timeout (in seconds)
    uint16_t                            rpa_renew_to;
    /// Address resolution enabled
    bool                                addr_resolution_en;

    /// RF Rx path compensation value (Range: -128.0 dB <= N <= 128.0 dB, Units: 0.1 dB)
    int16_t rx_path_comp;
    /// RF Tx path compensation value (Range: -128.0 dB <= N <= 128.0 dB, Units: 0.1 dB)
    int16_t tx_path_comp;

    #if (BLE_OBSERVER)
    /// Advertising duplicate filtering table information
    struct adv_dup_filt_tab_info          dup_filt;

    /// Information bit field for duplicate filtering (@see enum adv_dup_filt_info)
    uint8_t adv_dup_filt_info;

    /// Enable scanning for extended advertising PDUs
    bool ext_scan;
    #endif //(BLE_OBSERVER)

    #if (BLE_BROADCASTER)
    /// Number of extended advertising sets that are stopping
    uint8_t nb_ext_adv_sets_stopping;
    #endif //(BLE_BROADCASTER)

    #if (BLE_ADV_LEGACY_ITF)
    /// Indicate if the interface version the Host is using (legacy/extended), see HCI:3.1.1
    uint8_t                               adv_itf_version;
    #endif //(BLE_ADV_LEGACY_ITF)

    #if (BLE_CENTRAL || BLE_PERIPHERAL)
    /// Connection move enable
    uint8_t con_move_en;
    #endif // (BLE_CENTRAL || BLE_PERIPHERAL)

    /// Enable channel assessment usage when building channel maps
    bool ch_ass_en;
};


/*
 * GLOBAL VARIABLE DECLARATIONS
 *****************************************************************************************
 */

 /// LLM task message handlers table
extern const struct ke_task_desc TASK_DESC_LLM;

/// LLM environment variable
extern struct llm_env_tag llm_env;

/// LLM local supported features
extern uint8_t const llm_local_supp_feats[FEATURE_PAGE_MAX][FEATS_LEN];


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if (BLE_BROADCASTER)
/**
 ****************************************************************************************
 * @brief Retrieve Advertising identifier from advertising handle.
 *
 * @param[in]  adv_hdl    Advertising Handle
 * @param[out] adv_param  if not null, used to return advertising parameter that correspond to the handle.
 *
 * @return Activity identifier, or invalid identifier if not found
 ****************************************************************************************
 */
uint8_t llm_adv_hdl_to_id(uint16_t adv_hdl, struct hci_le_set_ext_adv_param_cmd** adv_param);
#endif // (BLE_BROADCASTER)

/// Send HCI CC event returning a status only
void llm_cmd_cmp_send(uint16_t opcode, uint8_t status);

/// Send HCI CS event
void llm_cmd_stat_send(uint16_t opcode, uint8_t status);

#if (BLE_CENTRAL || BLE_PERIPHERAL || BLE_OBSERVER)
/// Check if a device is already connected
bool llm_is_dev_connected(struct bd_addr const *peer_addr, uint8_t peer_addr_type);
#endif //(BLE_CENTRAL || BLE_PERIPHERAL || BLE_OBSERVER)

#if (BLE_OBSERVER)
/// Check if a device is a synchronized periodic advertiser
bool llm_is_dev_synced(struct bd_addr const *peer_addr, uint8_t peer_addr_type, uint8_t adv_sid);
#endif //(BLE_OBSERVER)

/**
 ****************************************************************************************
 * @brief Find an empty entry in device list
 *
 * @return position of an empty entry in the list (Max value if not found)
 ****************************************************************************************
 */
uint8_t llm_dev_list_empty_entry(void);

/**
 ****************************************************************************************
 * @brief Search if a device is in device list
 *
 * @param[in] bd_addr      BD Address
 * @param[in] bd_addr_type BD Address type (0: public | 1: random)
 *
 * @return position of the device in the list (Max value if not found)
 ****************************************************************************************
 */
uint8_t llm_dev_list_search(const struct bd_addr *bd_addr, uint8_t bd_addr_type);

/**
 ****************************************************************************************
 * @brief Entry point to update the channel map
 ****************************************************************************************
 */
void llm_ch_map_update(void);

/// @} LLM

#endif // LLM_INT_H_
