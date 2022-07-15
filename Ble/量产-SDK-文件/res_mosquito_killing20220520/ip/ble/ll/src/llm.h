/**
 ****************************************************************************************
 *
 * @file llm.h
 *
 * @brief Main API file for the Link Layer manager
 *
 * Copyright (C) RADIAWAVE  2017-2020
 *
 ****************************************************************************************
 */

#ifndef LLM_H_
#define LLM_H_

/**
 ****************************************************************************************
 * @defgroup LLM Link Layer Manager
 * @ingroup ROOT
 * @brief BLE Lower Layers
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h"      // Task definitions
#include "co_bt.h"          // BLE standard definitions

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * MESSAGES
 ****************************************************************************************
 */

/// Message API of the LLM task
/*@TRACE
 * llm_pub_key_gen_ind  = ecc_result_ind
 * llm_dh_key_gen_ind   = ecc_result_ind
 * llm_encrypt_ind      = aes_func_res_ind*/
enum llm_msg_id
{
    LLM_MSG_ID_FIRST = TASK_FIRST_MSG(TASK_ID_LLM),//!< LLM_MSG_ID_FIRST

    #if (BLE_OBSERVER)
    /// Send ACAD Data information
    LLM_ACAD_DATA_IND,                             //!< LLM_ACAD_DATA_IND
    #endif // (BLE_OBSERVER)

    /*
     * ************** Msg LLM->LLC****************
     */
    #if (BLE_CENTRAL)
    /// Indicate that the channel map has to be updated on link connection
    LLM_CH_MAP_UPDATE_IND,                         //!< LLM_CH_MAP_UPDATE_IND
    #endif //(BLE_CENTRAL)

    /*
     * ************** Msg LLM->LLM****************
     */
    LLM_SCAN_PERIOD_TO,                            //!< LLM_SCAN_PERIOD_TO
    LLM_CH_MAP_TO,                                 //!< LLM_CH_MAP_TO
    #if (BLE_CENTRAL || BLE_BROADCASTER)
    LLM_NEW_HOST_CLASS_TO,
    #endif //(BLE_CENTRAL || BLE_BROADCASTER)
    LLM_RPA_RENEW_TO,                              //!< LLM_RPA_RENEW_TO
    /// Inform that encryption has been performed
    LLM_ENCRYPT_IND,                               //!< LLM_ENCRYPT_IND

    #if (BLE_MESH || BLE_CENTRAL || BLE_PERIPHERAL)
    #if (SECURE_CONNECTIONS)
    LLM_PUB_KEY_GEN_IND,                           //!< LLM_PUB_KEY_GEN_IND
    LLM_DH_KEY_GEN_IND,                            //!< LLM_DH_KEY_GEN_IND
    #endif // (SECURE_CONNECTIONS)
    #endif // (BLE_MESH || BLE_CENTRAL || BLE_PERIPHERAL)
};

/// LLM Encryption Request parameters structure
/*@TRACE*/
struct llm_encrypt_req
{
    ///Long term key structure
    struct ltk     key;
    ///Pointer to buffer with plain data to encrypt - 16 bytes
    uint8_t        plain_data[ENC_DATA_LEN];
};

/// LLM Encryption indication structure
/*@TRACE*/
struct llm_encrypt_rsp
{
    /// Status of the encryption
    uint8_t status;
    ///Encrypted data
    uint8_t encrypted_data[ENC_DATA_LEN];
};


/// AES function execution result
/*@TRACE*/
struct llm_encrypt_ind
{
    /// Status of AES execution
    uint8_t status;
    /// Result of the
    uint8_t result[KEY_LEN];
};


#if (BLE_OBSERVER)
/// LLM ACAD data indication structure
/*@TRACE*/
struct llm_acad_data_ind
{
    /// Sync handle
    uint16_t sync_handle;
    /// Status the indication - use to detect loss of periodic sync
    uint8_t  status;
    /// ADV Data Type
    uint8_t  ad_type;
    /// Reference event counter of the periodic advertiser when report received
    uint16_t ref_evt_cnt;

    /// length of the data field
    uint8_t  data_len;
    /// ACAD Data
    uint8_t  data[__ARRAY_EMPTY];
};
#endif // (BLE_OBSERVER)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the BLE LLM task
 *
 * This function initializes the the LLM task, as well as the environment of the LLM
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void llm_init(uint8_t init_type);

#if (BLE_CENTRAL || BLE_PERIPHERAL)
/**
 ****************************************************************************************
 * @brief This function handles LE link disconnection.
 *
 * @param[in] link_id            Link Identifier
 ****************************************************************************************
 */
void llm_link_disc(uint8_t link_id);
#endif // (BLE_CENTRAL || BLE_PERIPHERAL)

/**
 ****************************************************************************************
 * @brief This function is used to get the master channel map
 *
 * @return Pointer to master channel map
 ****************************************************************************************
 */
struct le_chnl_map* llm_master_ch_map_get(void);

/**
 ****************************************************************************************
 * @brief This function is used to check if an event is unmasked/masked
 *
 * @param[in]  event_id  Bit position of the event in the mask (see standard specification part II.E.7.8.1)
 *
 * @return True: event allowed | False: event masked
 ****************************************************************************************
 */
bool llm_le_evt_mask_check(uint8_t event_id);

/**
 ****************************************************************************************
 * @brief Retrieve LE Local Supported features
 *
 * @return Local feature mask
 ****************************************************************************************
 */
const struct le_features* llm_le_features_get(void);

/**
 ****************************************************************************************
 * @brief This function is used to get the RF Tx path compensation value
 *
 * @return RF Tx path compensation value (Range: -128.0 dB <= N <= 128.0 dB, Units: 0.1 dB)
 ****************************************************************************************
 */
int16_t llm_tx_path_comp_get(void);

/**
 ****************************************************************************************
 * @brief This function is used to get the RF Rx path compensation value
 *
 * @return RF Rx path compensation value (Range: -128.0 dB <= N <= 128.0 dB, Units: 0.1 dB)
 ****************************************************************************************
 */
int16_t llm_rx_path_comp_get(void);

/**
 ****************************************************************************************
 * @brief This function returns a pointer to the interval element of the link_id provided.
 *
 * @param[in] link_id            Link Identifier
 *
 * @return Pointer to the associated interval element
 ****************************************************************************************
 */
struct sch_plan_elt_tag *llm_plan_elt_get(uint8_t link_id);

/**
 ****************************************************************************************
 * @brief Get available activity index
 *
 * @param[out] act_id            Activity Identifier allocated
 *
 * @return  - CO_ERROR_NO_ERROR:         if succeed
 *          - CO_ERROR_CON_LIMIT_EXCEED: if no activity available
 ****************************************************************************************
 */
uint8_t llm_activity_free_get(uint8_t* act_id);
/**
 ****************************************************************************************
 * @brief Free activity handle related to the activity
 *
 * @param[in] act_id            Activity Identifier
 ****************************************************************************************
 */
void llm_activity_free_set(uint8_t act_id);

#if (BLE_ISO_PRESENT)
#if (BLE_CIS)
/**
 ****************************************************************************************
 * @brief Mark activity to be an CIS in reserved state
 *
 * @param[in] act_id            Activity Identifier
 ****************************************************************************************
 */
void llm_iso_activity_cis_reserve(uint8_t act_id);

/**
 ****************************************************************************************
 * @brief Mark CIS activity in enable state. In parallel reserve required bandwidth in schedule planner.
 *
 * @param[in] act_id            Activity Identifier
 * @param[in] role              Master or slave role
 * @param[in] act_offset        Activity Offset (in slots)
 * @param[in] interval          Channel interval (in slots)
 * @param[in] bandwidth         Bandwidth required by activity in slots
 ****************************************************************************************
 */
void llm_iso_activity_cis_enable(uint8_t act_id, uint8_t role, uint16_t act_offset, uint16_t interval, uint16_t bandwidth);
#endif // (BLE_CIS)

#if (BLE_BIS)
/**
 ****************************************************************************************
 * @brief Mark activity to be an BIS in reserved state
 *
 * @param[in] act_id            Activity Identifier
 ****************************************************************************************
 */
void llm_iso_activity_bis_reserve(uint8_t act_id);

/**
 ****************************************************************************************
 * @brief Mark BIS activity in enable state. In parallel reserve required bandwidth in schedule planner.
 *
 * @note only the stream activity is marked enabled, BIS channel activities are kept in reserved state because
 *       stream and channels are enabled/stopped in same time
 *
 *
 * @param[in] act_id            Activity Identifier
 * @param[in] role              Master or slave role
 * @param[in] interval          Stream interval (in slots)
 * @param[in] act_offset        Activity Offset (in slots)
 * @param[in] act_bandwidth     Bandwidth required by activity in slots
 ****************************************************************************************
 */
void llm_iso_activity_bis_enable(uint8_t act_id, uint8_t role, uint16_t interval, uint16_t act_offset, uint16_t act_bandwidth);
#endif // (BLE_BIS)
#if (BLE_ISO_MODE_0)

/**
 ****************************************************************************************
 * @brief Get if Activity is an active link
 *
 * @param[out] act_id            Activity Identifier
 *
 * @return  true if in connected state, false else
 ****************************************************************************************
 */
bool llm_iso_link_active(uint8_t act_id);
#endif // (BLE_ISO_MODE_0)
#endif // (BLE_ISO_PRESENT)

#if (BLE_BIS)


/**
 ****************************************************************************************
 * @brief Retrieve Periodic Advertising activity identifier
 *
 * @param[in]  adv_hdl      Advertising handle of a Periodic Advertiser
 * @param[out] per_act_id   Returned periodic advertiser activity identifier
 *
 * @return CO_ERROR_NO_ERROR if the periodic advertiser is found
 ****************************************************************************************
 */
uint8_t llm_adv_per_id_get(uint8_t adv_hdl, uint8_t* per_act_id);

/**
 ****************************************************************************************
 * @brief Attach an BIG to a periodic advertising driver
 *
 * @param[in] per_act_id   Periodic advertiser activity identifier
 * @param[in] big_act_id   Activity ID allocated for BIG
 *
 * @return CO_ERROR_NO_ERROR if the periodic advertiser is found
 ****************************************************************************************
 *
 */
uint8_t llm_adv_big_attach(uint8_t per_act_id, uint8_t big_act_id);

/**
 ****************************************************************************************
 * @brief Detach an BIG from a periodic advertising driver
 *
 * @param[in] per_act_id   Periodic advertiser activity identifier
 * @param[in] big_act_id   Activity ID allocated for BIG
 *
 * @return CO_ERROR_NO_ERROR if the periodic advertiser is found
 ****************************************************************************************
 */
uint8_t llm_adv_big_detach(uint8_t per_act_id, uint8_t big_act_id);
#endif // (BLE_BIS)

#if BT_DUAL_MODE
/**
 ****************************************************************************************
 * @brief Get if Activity is ongoing
 *
 * @return  true if active
 ****************************************************************************************
 */
bool llm_activity_ongoing_check(void);
#endif //BT_DUAL_MODE

/**
 ****************************************************************************************
 * @brief Attach a task with a periodic scanner to receive specific AD Type data
 *
 * @param[in] sync_act_id Activity identifier of the periodic sync
 * @param[in] ad_type     AD Type to retrieve in ACAD data
 * @param[in] task        Task that expect the LLM_ACAD_DATA_IND message
 *
 * @return CO_ERROR_NO_ERROR if the periodic sync is found
 ****************************************************************************************
 */
uint8_t llm_scan_sync_acad_attach(uint8_t sync_act_id, uint8_t ad_type, uint16_t task);


/**
 ****************************************************************************************
 * @brief Detach periodic scanner ACAD Data watcher
 *
 * @param[in] sync_act_id Activity identifier of the periodic sync
 *
 * @return CO_ERROR_NO_ERROR if the periodic sync is found
 ****************************************************************************************
 */
uint8_t llm_scan_sync_acad_detach(uint8_t sync_act_id);

/// @} LLM

#endif // LLM_H_
