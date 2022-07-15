/**
 ****************************************************************************************
 *
 * @file plxc_task.c
 *
 * @brief Pulse Oximeter Profile Collector Task implementation.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @addtogroup PLXCTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_PLX_CLIENT)
#include "plxp_common.h"
#include "plxc_task.h"
#include "plxc.h"

#include "gap.h"
#include "attm.h"
#include "gattc_task.h"
#include "ke_mem.h"
#include "co_utils.h"


/*
 * STRUCTURES
 ****************************************************************************************
 */

/// State machine used to retrieve Pulse Oximeter Service characteristics information
const struct prf_char_def plxc_plx_char[PLXC_CHAR_MAX] =
{
    [PLXC_CHAR_SPOT_MEASUREMENT] = {ATT_CHAR_PLX_SPOT_CHECK_MEASUREMENT_LOC,
                                         ATT_OPTIONAL,
                                        (ATT_CHAR_PROP_IND) },

    [PLXC_CHAR_CONT_MEASUREMENT] = {ATT_CHAR_PLX_CONTINUOUS_MEASUREMENT_LOC,
                                         ATT_OPTIONAL,
                                        (ATT_CHAR_PROP_NTF) },

    [PLXC_CHAR_FEATURES]         = {ATT_CHAR_PLX_FEATURES_LOC,
                                         ATT_MANDATORY,
                                        (ATT_CHAR_PROP_RD) },

    [PLXC_CHAR_RACP]             = {ATT_CHAR_REC_ACCESS_CTRL_PT,
                                         ATT_OPTIONAL,
                                        (ATT_CHAR_PROP_WR | ATT_CHAR_PROP_IND) },
};


/// State machine used to retrieve Pulse Oximeter Service characteristic Description information
const struct prf_char_desc_def plxc_plx_char_desc[PLXC_DESC_MAX] =
{
    /// Client config
    [PLXC_DESC_SPOT_MEASUREMENT_CCC] = {ATT_DESC_CLIENT_CHAR_CFG,
                                         ATT_OPTIONAL,
                                         PLXC_CHAR_SPOT_MEASUREMENT},
    /// Client config
    [PLXC_DESC_CONT_MEASUREMENT_CCC] = {ATT_DESC_CLIENT_CHAR_CFG,
                                         ATT_OPTIONAL,
                                         PLXC_CHAR_CONT_MEASUREMENT},
    /// Client config
    [PLXC_DESC_RACP_CCC]             = {ATT_DESC_CLIENT_CHAR_CFG,
                                         ATT_OPTIONAL,
                                         PLXC_CHAR_RACP},
};


/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @see GATTC_SDP_SVC_IND_HANDLER message.
 * The handler stores the found service details for service discovery.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_sdp_svc_ind_handler(ke_msg_id_t const msgid,
                                             struct gattc_sdp_svc_ind const *ind,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{

    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t state = ke_state_get(dest_id);

    if (state == PLXC_DISCOVERING_SVC)
    {
        /// Pulse Oximeter Collector Role Task Environment
        struct plxc_env_tag *plxc_env = PRF_ENV_GET(PLXC, plxc);

        ASSERT_INFO(plxc_env != NULL, dest_id, src_id);
        if (plxc_env != NULL)
        {

            ASSERT_INFO(plxc_env->env[conidx] != NULL, dest_id, src_id);

            if(attm_uuid16_comp((unsigned char *)ind->uuid, ind->uuid_len, ATT_SVC_PULSE_OXIMETER))
            {
                // Retrieve WS characteristics and descriptors
                prf_extract_svc_info(ind, PLXC_CHAR_MAX, &plxc_plx_char[0],  &plxc_env->env[conidx]->plxc.chars[0],
                        PLXC_DESC_MAX, &plxc_plx_char_desc[0], &plxc_env->env[conidx]->plxc.descs[0]);

                //Even if we get multiple responses we only store 1 range
                plxc_env->env[conidx]->plxc.svc.shdl = ind->start_hdl;
                plxc_env->env[conidx]->plxc.svc.ehdl = ind->end_hdl;

                plxc_env->env[conidx]->nb_svc++;
            }
        }
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see PLXC_ENABLE_REQ message.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

static int plxc_enable_req_handler(ke_msg_id_t const msgid,
        struct plxc_enable_req *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    /// Status
    uint8_t status     = GAP_ERR_NO_ERROR;
    /// Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    uint8_t state = ke_state_get(dest_id);
    /// Pulse Oximeter Collector Role Task Environment
    struct plxc_env_tag *plxc_env = PRF_ENV_GET(PLXC, plxc);

    ASSERT_INFO(plxc_env != NULL, dest_id, src_id);
    if ((state == PLXC_IDLE) && (plxc_env->env[conidx] == NULL))
    {
        /// allocate environment variable for task instance
        plxc_env->env[conidx] = (struct plxc_cnx_env*) ke_malloc(sizeof(struct plxc_cnx_env),KE_MEM_ATT_DB);
        memset(plxc_env->env[conidx], 0, sizeof(struct plxc_cnx_env));

        /// Start discovering
        if (param->con_type == PRF_CON_DISCOVERY)
        {
            /// Start discovery with Pulse Oximeter
            prf_disc_svc_send(&(plxc_env->prf_env), conidx, ATT_SVC_PULSE_OXIMETER);

            /// Go to DISCOVERING SERVICE state
            ke_state_set(dest_id, PLXC_DISCOVERING_SVC);
        }
        //normal connection, get saved att details
        else
        {
            uint16_t search_hdl;
            struct prf_svc *svc;
            plxc_env->env[conidx]->plxc = param->plxc;

            // start reading features
            //send APP confirmation that can start normal connection to TH
//            plxc_enable_rsp_send(plxc_env, conidx, GAP_ERR_NO_ERROR);
                // Attribute Handle
                search_hdl = plxc_env->env[conidx]->plxc.chars[PLXC_CHAR_FEATURES].val_hdl;
                // Service
                svc = &plxc_env->env[conidx]->plxc.svc;
                
               // Check if handle is viable
                if ((search_hdl != ATT_INVALID_SEARCH_HANDLE) && (svc != NULL))
                {
                    // Force the operation value
                    plxc_env->env[conidx]->op_pending = PLXC_OP_PENDING_READ_FEATURES_AT_INIT;

                    // Send the write request
                    prf_read_char_send(&(plxc_env->prf_env), conidx, svc->shdl, svc->ehdl, search_hdl);
                    // Go to the Busy state
                    ke_state_set(dest_id, PLXC_BUSY);

                    status = GAP_ERR_NO_ERROR;
                }
        }
    }
    else if (state != PLXC_FREE)
    {
        // The message will be forwarded towards the good task instance
       status = PRF_ERR_REQ_DISALLOWED;
    }

    if(status != GAP_ERR_NO_ERROR)
    {
        // The request is disallowed (profile already enabled for this connection, or not enough memory, ...)
        plxc_enable_rsp_send(plxc_env, conidx, status, 0, 0, 0);
    }

    return (KE_MSG_CONSUMED);
}




/**
 ****************************************************************************************
 * @brief Handles reception of the @see PLXC_READ_CMD message from the application.
 * @brief To read the Feature Characteristic in the peer server.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

static int plxc_read_cmd_handler(ke_msg_id_t const msgid,
        struct plxc_read_cmd *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;
    // Get the address of the environment
    struct plxc_env_tag *plxc_env = PRF_ENV_GET(PLXC, plxc);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    if (state == PLXC_IDLE)
    {
        ASSERT_INFO(plxc_env != NULL, dest_id, src_id);
        // environment variable not ready
        if(plxc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
            // Attribute Handle
            uint16_t handle = ATT_INVALID_SEARCH_HANDLE;
            // Service
            struct prf_svc *svc = &plxc_env->env[conidx]->plxc.svc;

            if ((svc != NULL) && (svc->shdl != ATT_INVALID_SEARCH_HANDLE))
            {
                if (param->operation == PLXC_OP_CODE_READ_FEATURES)
                {
                    handle = plxc_env->env[conidx]->plxc.chars[PLXC_CHAR_FEATURES].val_hdl;
                }
            }

            // Check if handle is viable
            if (handle != ATT_INVALID_SEARCH_HANDLE)
            {
                // Force the operation value
                plxc_env->env[conidx]->op_pending = PLXC_OP_PENDING_READ_FEATURES;

                // Send the read request
                prf_read_char_send(&(plxc_env->prf_env), conidx, svc->shdl, svc->ehdl, handle);
                // Go to the Busy state
                ke_state_set(dest_id, PLXC_BUSY);

                status = GAP_ERR_NO_ERROR;
            }
            else
            {
                status =  PRF_ERR_INEXISTENT_HDL;
            }

        }
    }
    else if (state == PLXC_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        // Another procedure is pending, keep the command for later
        msg_status = KE_MSG_SAVED;
        status = GAP_ERR_NO_ERROR;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        plxc_send_cmp_evt(plxc_env, conidx, param->operation, status);
    }

    return (int)msg_status;
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see PLXC_WRITE_RACP_CMD message from the application.
 * @brief To write command to the Records Access Control Point in the peer server.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

static int plxc_write_racp_cmd_handler(ke_msg_id_t const msgid,
        struct plxc_write_racp_cmd *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;
    // Get the address of the environment
    struct plxc_env_tag *plxc_env = PRF_ENV_GET(PLXC, plxc);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    if (state == PLXC_IDLE)
    {
        ASSERT_INFO(plxc_env != NULL, dest_id, src_id);
        // environment variable not ready
        if(plxc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
            // Attribute Handle
            uint16_t search_hdl = plxc_env->env[conidx]->plxc.chars[PLXC_CHAR_RACP].val_hdl;
            // Service
            struct prf_svc *svc = &plxc_env->env[conidx]->plxc.svc;

             // Length
            uint8_t data[2];

            // fill parameters
            data[0] = param->cp_opcode;
            data[1] = param->cp_operator;

           // Check if handle is viable
            if ((search_hdl != ATT_INVALID_SEARCH_HANDLE) && (svc != NULL))
            {
                // Force the operation value
                plxc_env->env[conidx]->op_pending = PLXC_OP_PENDING_WRITE_RACP;

                // Send the write request
                prf_gatt_write(&(plxc_env->prf_env), conidx, search_hdl, &data[0], 2, GATTC_WRITE);
                // Go to the Busy state
                ke_state_set(dest_id, PLXC_BUSY);

                status = GAP_ERR_NO_ERROR;
            }
            else
            {
                status =  PRF_ERR_INEXISTENT_HDL;
            }
        }
    }
    else if (state == PLXC_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        // Another procedure is pending, keep the command for later
        msg_status = KE_MSG_SAVED;
        status = GAP_ERR_NO_ERROR;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        plxc_send_cmp_evt(plxc_env, conidx, PLXC_OP_CODE_WRITE_RACP, status);
    }

    return (int)msg_status;
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see PLXC_CFG_CCC_CMD message.
 * Allows the application to write new CCC values to a Alert Characteristic in the peer server
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int plxc_cfg_ccc_cmd_handler(ke_msg_id_t const msgid,
                                   struct plxc_cfg_ccc_cmd *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Status
    uint8_t status     = GAP_ERR_NO_ERROR;
    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;

    // Get the address of the environment
    struct plxc_env_tag *plxc_env = PRF_ENV_GET(PLXC, plxc);
    // Get connection index

    if (state == PLXC_IDLE)
    {

        ASSERT_INFO(plxc_env != NULL, dest_id, src_id);
        // environment variable not ready
        if(plxc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
             // Attribute Handle
            uint16_t handle = 0;
            // Service
            struct prf_svc *svc = &plxc_env->env[conidx]->plxc.svc;
            if (param->operation == PLXC_SPOT_CHECK_MEAS_CMD_OP_CODE)
            {
                handle = plxc_env->env[conidx]->plxc.descs[PLXC_DESC_SPOT_MEASUREMENT_CCC].desc_hdl;
            }
            else
            if (param->operation == PLXC_CONTINUOUS_MEAS_CMD_OP_CODE)
            {
                handle = plxc_env->env[conidx]->plxc.descs[PLXC_DESC_CONT_MEASUREMENT_CCC].desc_hdl;
            }
            else
            if (param->operation == PLXC_RACP_CMD_OP_CODE)
            {
                handle = plxc_env->env[conidx]->plxc.descs[PLXC_DESC_RACP_CCC].desc_hdl;
            }

            // Check if handle is viable
            if ((handle != ATT_INVALID_SEARCH_HANDLE) && (svc != NULL))
            {
                // Force the operation value
                plxc_env->env[conidx]->op_pending = PLXC_OP_PENDING_WRITE_CCC;

                // Send the write request
                prf_gatt_write_ntf_ind(&(plxc_env->prf_env), conidx, handle, param->ccc);

                // Go to the Busy state
                ke_state_set(dest_id, PLXC_BUSY);

                status = GAP_ERR_NO_ERROR;
            }
            else
            {
                status = PRF_ERR_INEXISTENT_HDL;
            }
        }

        if (status != GAP_ERR_NO_ERROR)
        {
            plxc_send_cmp_evt(plxc_env, conidx, PLXC_OP_CODE_WRITE_CCC, status);
        }
    }
    else if (state == PLXC_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        // Another procedure is pending, keep the command for later
        msg_status = KE_MSG_SAVED;
        status = GAP_ERR_NO_ERROR;
    }

    return (int)msg_status;
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see GATTC_CMP_EVT message.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                 struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct plxc_env_tag *plxc_env = PRF_ENV_GET(PLXC, plxc);
    // Status
    uint8_t status = PRF_APP_ERROR;

    if (plxc_env != NULL)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t state = ke_state_get(dest_id);

        if (state == PLXC_DISCOVERING_SVC)
        {
            // clean state for the new discovering
            plxc_env->env[conidx]->op_pending = PLXC_OP_PENDING_NO_OP;
            
            if ((param->status == GAP_ERR_NO_ERROR) || (param->status == ATT_ERR_ATTRIBUTE_NOT_FOUND))
            {
                // Check service (if found)
                if(plxc_env->env[conidx]->nb_svc)
                {
                    // Check service (mandatory)
                    status = prf_check_svc_char_validity(PLXC_CHAR_MAX,
                            plxc_env->env[conidx]->plxc.chars,
                            plxc_plx_char);

                    // Check Descriptors (mandatory)
                    if(status == GAP_ERR_NO_ERROR)
                    {
                        status = prf_check_svc_char_desc_validity(PLXC_DESC_MAX,
                                plxc_env->env[conidx]->plxc.descs,
                                plxc_plx_char_desc,
                                plxc_env->env[conidx]->plxc.chars);
                    }
                }
            }
            
            // request read FEATURES
            if (status == GAP_ERR_NO_ERROR)
            {
                // Attribute Handle
                uint16_t search_hdl = plxc_env->env[conidx]->plxc.chars[PLXC_CHAR_FEATURES].val_hdl;
                // Service
                struct prf_svc *svc = &plxc_env->env[conidx]->plxc.svc;
                
               // Check if handle is viable
                if ((search_hdl != ATT_INVALID_SEARCH_HANDLE) && (svc != NULL))
                {
                    // Force the operation value
                    plxc_env->env[conidx]->op_pending = PLXC_OP_PENDING_READ_FEATURES_AT_INIT;

                    // Send the read request
                    prf_read_char_send(&(plxc_env->prf_env), conidx, svc->shdl, svc->ehdl, search_hdl);
                    // Go to the Busy state
                    ke_state_set(dest_id, PLXC_BUSY);

                    status = GAP_ERR_NO_ERROR;
                }
            }

            // at this stage we send response only if it fail in some case
            if (status != GAP_ERR_NO_ERROR)
            {
                // Raise an PLXC_ENABLE_REQ complete event.
                ke_state_set(dest_id, PLXC_IDLE);

                plxc_enable_rsp_send(plxc_env, conidx, param->status, 0 ,0 ,0);
            }
        }
        else if (state == PLXC_BUSY)
        {
            uint8_t op_pending = plxc_env->env[conidx]->op_pending;
            status = param->status;
            switch (param->operation)
            {
                case GATTC_READ:
                {
                    // Send the complete event status
                    if (status != GAP_ERR_NO_ERROR)
                    {
                        switch(op_pending)
                        {
                            case PLXC_OP_PENDING_READ_FEATURES_AT_INIT:
                            {
                                // Raise an PLXC_ENABLE_REQ complete event.
                                plxc_enable_rsp_send(plxc_env, conidx, status, 0 ,0 ,0);
                            }
                            break;
                            case PLXC_OP_PENDING_READ_FEATURES:
                            {
                                // Send read complete event.
                                plxc_send_cmp_evt(plxc_env, conidx, PLXC_OP_CODE_READ_FEATURES, status);
                            }
                            break;

                        default:
                            break;
                        }
                        ke_state_set(dest_id, PLXC_IDLE);
                    }
                }
                break;

                case GATTC_WRITE:
                case GATTC_WRITE_NO_RESPONSE:
                {
                    // Send the complete event status
                    plxc_send_cmp_evt(plxc_env, conidx, 
                                (op_pending == PLXC_OP_PENDING_WRITE_RACP?PLXC_OP_CODE_WRITE_RACP:PLXC_OP_CODE_WRITE_CCC),
                                param->status);
                    ke_state_set(dest_id, PLXC_IDLE);

                } break;

                case GATTC_REGISTER:
                {
                    if (status != GAP_ERR_NO_ERROR)
                    {
                        // Send the complete event error status
                        plxc_send_cmp_evt(plxc_env, conidx, PLXC_OP_CODE_GATTC_REGISTER, param->status);
                    }
                    // Go to connected state
                    ke_state_set(dest_id, PLXC_IDLE);
                } break;

                case GATTC_UNREGISTER:
                {
                    // Do nothing
                } break;

                default:
                {
                    ASSERT_ERR(0);
                } break;
            }
            plxc_env->env[conidx]->op_pending = PLXC_OP_PENDING_NO_OP;
        }
    }
    // else ignore the message

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @see GATTC_READ_IND message.
 * Generic event received after every simple read command sent to peer server.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_ind_handler(ke_msg_id_t const msgid,
                                    struct gattc_read_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct plxc_env_tag *plxc_env = PRF_ENV_GET(PLXC, plxc);

    if (ke_state_get(dest_id) == PLXC_BUSY)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t op_pending;
        uint8_t status = GAP_ERR_NO_ERROR;
        // length of data block
        uint16_t length;

        ASSERT_INFO(plxc_env != NULL, dest_id, src_id);
        ASSERT_INFO(plxc_env->env[conidx] != NULL, dest_id, src_id);

        // Find the op_pending -  and check it is a valid read code.
        op_pending = plxc_env->env[conidx]->op_pending;
        if ((op_pending == PLXC_OP_PENDING_READ_FEATURES_AT_INIT) || (op_pending == PLXC_OP_PENDING_READ_FEATURES))
        {
            /// Supported Features @see common supported_features
            uint16_t supported_features = 0;
            /// If enabled in  Supported Features @see common measurement_status_supported
            uint16_t measurement_status_supported = 0;
            /// If enabled in  Supported Features @see common device_status_supported
            uint32_t device_status_supported = 0;
            // data pointer
            uint8_t const *ptr;

            length = param->length;

            if (length >=2)
            {
                supported_features = co_read16p(&param->value[0]);
                ptr = &param->value[2];
                length -= 2; // uint16 supported_features;
                if ((supported_features & PLXP_FEATURE_MEAS_STATUS_PRESENT) && (length >=2))
                {
                    measurement_status_supported = co_read16p(ptr);
                    ptr += 2; // uint16 measurement_status_supported;
                    length -= 2;
                }
                if ((supported_features & PLXP_FEATURE_DEVICE_SENSOR_STATUS_PRESENT) && (length >=3))
                {
                    device_status_supported = co_read24p(ptr);
                }
                    
            }
            switch (op_pending)
            {
                case PLXC_OP_PENDING_READ_FEATURES_AT_INIT:
                {
                    // Raise an PLXC_ENABLE_REQ complete event.
                    plxc_enable_rsp_send(plxc_env, conidx, status, 
                                supported_features,
                                measurement_status_supported,
                                device_status_supported);

                }
                break;

                case PLXC_OP_PENDING_READ_FEATURES:
                {
                    // Send value ind and next complete event.
                    //indication Build a PLXC_VALUE_IND message
                    struct plxc_value_ind* ind = KE_MSG_ALLOC(PLXC_VALUE_IND,
                                               prf_dst_task_get(&plxc_env->prf_env, conidx),
                                               prf_src_task_get(&plxc_env->prf_env, conidx),
                                               plxc_value_ind);
                    ind->operation = PLXC_OP_CODE_READ_FEATURES;
                    ind->value.features.supported_features = supported_features;
                    ind->value.features.measurement_status_supported = measurement_status_supported;
                    ind->value.features.device_status_supported = device_status_supported;
                    
                    // send indication to the application
                    ke_msg_send(ind);

                    plxc_send_cmp_evt(plxc_env, conidx, PLXC_OP_CODE_READ_FEATURES, status);
                }
                break;


                default:
                {
                    ASSERT_ERR(0);
                } break;
            }
            plxc_env->env[conidx]->op_pending = PLXC_OP_PENDING_NO_OP;
            ke_state_set(dest_id, PLXC_IDLE);
        }


    }
    // else drop the message

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @see GATTC_EVENT_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_event_ind_handler(ke_msg_id_t const msgid,
                                   struct gattc_event_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Get the address of the environment
    struct plxc_env_tag *plxc_env = PRF_ENV_GET(PLXC, plxc);
    // data pointer
    const uint8_t  *val_ptr;
    // length of data block
    uint16_t length;

    if (plxc_env != NULL)
    {
        switch (param->type)
        {
            case (GATTC_INDICATE):
            case (GATTC_NOTIFY):
            {
                //indication Build a PLXC_VALUE_IND message
                struct plxc_value_ind* ind = KE_MSG_ALLOC(PLXC_VALUE_IND,
                                           prf_dst_task_get(&plxc_env->prf_env, conidx),
                                           prf_src_task_get(&plxc_env->prf_env, conidx),
                                           plxc_value_ind);
                memset(ind, 0, sizeof(struct plxc_value_ind));

                if(param->handle == plxc_env->env[conidx]->plxc.chars[PLXC_CHAR_SPOT_MEASUREMENT].val_hdl)
                {
                    uint8_t spot_flags  = param->value[0];
                    length = param->length;
                    val_ptr = &param->value[5];
                    
                    // TBD: verify flags vs supported_features
                    
                    ind->operation = PLXC_SPOT_CHECK_MEAS_CMD_OP_CODE;
                    // decode message
                    if (length >=5)
                    {
                        /// uint8_t  bitfield of supplied data @see common plxp_spot_meas_flag
                        ind->value.spot_meas.spot_flags = spot_flags;
                        /// uint16_t Percentage with a resolution of 1
                        ind->value.spot_meas.sp_o2 = co_read16p(&param->value[1]);
                        /// uint16_t Period (beats per minute) with a resolution of 1
                        ind->value.spot_meas.pr = co_read16p(&param->value[3]);
                        length -= 5;
                    }
                    else
                    { //no further parameters possible
                        length = 0;
                    }
                    
                    /// Timestamp (if present)
                    if (spot_flags & PLXP_SPOT_MEAS_FLAGS_TIMESTAMP_PRESENT)
                    {
                        if (length >=7)
                        {
                            val_ptr += prf_unpack_date_time((uint8_t *)val_ptr, (struct prf_date_time* )&ind->value.spot_meas.timestamp);
                            length -=7;
                        }
                        else
                        { //no further parameters possible
                            length = 0;
                        }
                    }
                    /// Measurement Status (bitfield) @see common measurement_status_supported
                    if ((spot_flags & PLXP_SPOT_MEAS_FLAGS_MEAS_STATUS_PRESENT) && (length >=2))
                    {
                        ind->value.spot_meas.meas_status = co_read16p(val_ptr);
                        val_ptr += 2;
                        length -= 2;
                    }
                    /// Device and Sensor Status (bitfield) @see common device_status_supported
                    if ((spot_flags & PLXP_SPOT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT) && (length >=2))
                    {
                        ind->value.spot_meas.device_sensor_status = co_read24p(val_ptr);
                        val_ptr += 3;
                        length -= 3;
                    }
                    /// Pulse Amplitude Index - Unit is percentage with a resolution of 1
                    if ((spot_flags & PLXP_SPOT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT) && (length >=2))
                    {
                        ind->value.spot_meas.pulse_amplitude = co_read16p(val_ptr);
                        val_ptr += 2;
                        length -= 2;
                    }
                    
                }
                else
                if(param->handle == plxc_env->env[conidx]->plxc.chars[PLXC_CHAR_CONT_MEASUREMENT].val_hdl)
                {
                    uint8_t cont_flags  = param->value[0];
                    length = param->length;
                    val_ptr = &param->value[5];
                    // TBD: verify flags vs supported_features
                    
                    ind->operation = PLXC_CONTINUOUS_MEAS_CMD_OP_CODE;
                    // decode message
                    if (length >=5)
                    {
                        /// bitfield of supplied data @see common plxp_cont_meas_flag
                        ind->value.cont_meas.cont_flags = cont_flags;
                        /// SpO2 - PR measurements - Normal
                        ind->value.cont_meas.normal.sp_o2 = co_read16p(&param->value[1]);
                        ind->value.cont_meas.normal.pr = co_read16p(&param->value[3]);
                        length -= 5;
                    }
                    else
                    { //no further parameters possible
                        length = 0;
                    }
                    

                    /// SpO2 - PR measurements - Fast
                    if (cont_flags & PLXP_CONT_MEAS_FLAGS_SPO2PR_FAST_PRESENT)
                    {
                        if(length >=4)
                        {
                            ind->value.cont_meas.fast.sp_o2 = co_read16p(val_ptr);
                            val_ptr += 2;
                            ind->value.cont_meas.fast.pr = co_read16p(val_ptr);
                            val_ptr += 2;
                            length -= 4;
                        }
                        else
                        { //no further parameters possible
                            length = 0;
                        }
                    }
                    /// SpO2 - PR measurements - Slow
                    if (cont_flags & PLXP_CONT_MEAS_FLAGS_SPO2PR_SLOW_PRESENT)
                    {
                        if (length >=4)
                        {
                            ind->value.cont_meas.slow.sp_o2 = co_read16p(val_ptr);
                            val_ptr += 2;
                            ind->value.cont_meas.slow.pr = co_read16p(val_ptr);
                            val_ptr += 2;
                            length -= 4;
                        }
                        else
                        { //no further parameters possible
                            length = 0;
                        }
                    }
                    /// Measurement Status (bitfield) @see common measurement_status_supported
                    if ((cont_flags & PLXP_CONT_MEAS_FLAGS_MEAS_STATUS_PRESENT) && (length >=2))
                    {
                        ind->value.cont_meas.meas_status = co_read16p(val_ptr);
                        val_ptr += 2;
                        length -= 2;
                    }
                        /// Device and Sensor Status (bitfield) @see common device_status_supported
                    if ((cont_flags & PLXP_CONT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT) && (length >=2))
                    {
                        ind->value.cont_meas.device_sensor_status = co_read24p(val_ptr);
                        val_ptr += 3;
                        length -= 3;
                    }
                        /// Pulse Amplitude Index - Unit is percentage with a resolution of 1
                    if ((cont_flags & PLXP_CONT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT) && (length >=2))
                    {
                        ind->value.cont_meas.pulse_amplitude = co_read16p(val_ptr);
                    }
                }
                else
                if(param->handle == plxc_env->env[conidx]->plxc.chars[PLXC_CHAR_RACP].val_hdl)
                {
                    /// Control Point OpCode @see enum plxp_cp_opcodes_id
                    uint8_t cp_opcode = param->value[0];
                    
                    ind->operation = PLXC_RACP_CMD_OP_CODE;
                    
                    ind->value.racp_resp.cp_opcode = cp_opcode;
                    // in both cases we don't care about the Operator (param->value[1]) - it should be Null(0x00)
                    if (cp_opcode == PLXP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP)
                    {
                        ind->value.racp_resp.number_of_records = co_read16p(&param->value[2]);
                    }
                    else
                    {
                        /// Request Control Point OpCode @see enum plxp_cp_opcodes_id
                        ind->value.racp_resp.request_cp_opcode = param->value[2];
                        /// Response Code @see enum plxp_cp_resp_code_id
                        ind->value.racp_resp.response_code = param->value[3];
                    }
                }
                else
                {
                    // error case
                    ASSERT_ERR(0);
                }
                // send indication to the application
                ke_msg_send(ind);

                // confirm that indication has been correctly received
                struct gattc_event_cfm * cfm = KE_MSG_ALLOC(GATTC_EVENT_CFM, src_id, dest_id, gattc_event_cfm);
                cfm->handle = param->handle;
                ke_msg_send(cfm);
                // else drop the message
            } break;

            default:
            {
                ASSERT_ERR(0);
            } break;
        }
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Specifies the default message handlers
KE_MSG_HANDLER_TAB(plxc)
{
    {PLXC_ENABLE_REQ,                   (ke_msg_func_t)plxc_enable_req_handler},

    {GATTC_SDP_SVC_IND,                 (ke_msg_func_t)gattc_sdp_svc_ind_handler},
    {GATTC_CMP_EVT,                     (ke_msg_func_t)gattc_cmp_evt_handler},

    {GATTC_READ_IND,                    (ke_msg_func_t)gattc_read_ind_handler},
    {GATTC_EVENT_IND,                   (ke_msg_func_t)gattc_event_ind_handler},
    {GATTC_EVENT_REQ_IND,               (ke_msg_func_t)gattc_event_ind_handler},

    {PLXC_READ_CMD,                     (ke_msg_func_t)plxc_read_cmd_handler},

    {PLXC_WRITE_RACP_CMD,               (ke_msg_func_t)plxc_write_racp_cmd_handler},
    
    {PLXC_CFG_CCC_CMD,                  (ke_msg_func_t)plxc_cfg_ccc_cmd_handler},
};

void plxc_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct plxc_env_tag *plxc_env = PRF_ENV_GET(PLXC, plxc);

    task_desc->msg_handler_tab = plxc_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(plxc_msg_handler_tab);
    task_desc->state           = plxc_env->state;
    task_desc->idx_max         = PLXC_IDX_MAX;
}

#endif //(BLE_PLX_CLIENT)

/// @} PLXCTASK
