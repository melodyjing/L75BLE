/**
 ****************************************************************************************
 *
 * @file cgmc_task.c
 *
 * @brief Continuous Glucose Monitoring Profile Collector Task implementation.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @addtogroup CGMCTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_CGM_CLIENT)
#include "cgmp_common.h"
#include "cgmc_task.h"
#include "cgmc.h"

#include "gap.h"
#include "attm.h"
#include "gattc_task.h"
#include "ke_mem.h"
#include "ke_timer.h"
#include "co_utils.h"


/*
 * STRUCTURES
 ****************************************************************************************
 */

/// State machine used to retrieve Continuous Glucose Monitoring Service characteristics information
const struct prf_char_def cgmc_cgm_char[CGMC_CHAR_MAX] =
{
    [CGMC_CHAR_MEASUREMENT]          = {ATT_CHAR_CGM_MEASUREMENT,
                                        ATT_MANDATORY,
                                       (ATT_CHAR_PROP_NTF) },

    [CGMC_CHAR_FEATURES]             = {ATT_CHAR_CGM_FEATURES,
                                        ATT_MANDATORY,
                                       (ATT_CHAR_PROP_RD) },

    [CGMC_CHAR_STATUS]               = {ATT_CHAR_CGM_STATUS,
                                        ATT_MANDATORY,
                                       (ATT_CHAR_PROP_RD) },

    [CGMC_CHAR_SESSION_START_TIME]   = {ATT_CHAR_CGM_SESSION_START,
                                        ATT_MANDATORY,
                                       (ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD) },

    [CGMC_CHAR_SESSION_RUN_TIME]     = {ATT_CHAR_CGM_SESSION_RUN,
                                        ATT_MANDATORY,
                                       (ATT_CHAR_PROP_RD) },

    [CGMC_CHAR_RACP]                 = {ATT_CHAR_REC_ACCESS_CTRL_PT,
                                        ATT_MANDATORY,
                                       (ATT_CHAR_PROP_WR | ATT_CHAR_PROP_IND) },

    [CGMC_CHAR_SPECIFIC_OPS_CTRL_PT] = {ATT_CHAR_CGM_SPECIFIC_OPS_CTRL_PT,
                                        ATT_MANDATORY,
                                       (ATT_CHAR_PROP_WR | ATT_CHAR_PROP_IND) },
};


/// State machine used to retrieve Continuous Glucose Monitoring Service characteristic Description information
const struct prf_char_desc_def cgmc_cgm_char_desc[CGMC_DESC_MAX] =
{
    /// Client config
    [CGMC_DESC_MEASUREMENT_CCC]          = {ATT_DESC_CLIENT_CHAR_CFG,
                                         ATT_MANDATORY,
                                         CGMC_CHAR_MEASUREMENT},
    /// Client config
    [CGMC_DESC_RACP_CCC]                 = {ATT_DESC_CLIENT_CHAR_CFG,
                                         ATT_MANDATORY,
                                         CGMC_CHAR_RACP},
    /// Client config
    [CGMC_DESC_SPECIFIC_OPS_CTRL_PT_CCC] = {ATT_DESC_CLIENT_CHAR_CFG,
                                         ATT_MANDATORY,
                                         CGMC_CHAR_SPECIFIC_OPS_CTRL_PT},
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
__STATIC int gattc_sdp_svc_ind_handler(ke_msg_id_t const msgid,
                                             struct gattc_sdp_svc_ind const *ind,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{

    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t state = ke_state_get(dest_id);

    if (state == CGMC_DISCOVERING_SVC)
    {
        // Continuous Glucose Monitoring Collector Role Task Environment
        struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);

        ASSERT_INFO(cgmc_env != NULL, dest_id, src_id);
        if (cgmc_env != NULL)
        {

            ASSERT_INFO(cgmc_env->env[conidx] != NULL, dest_id, src_id);

            if(attm_uuid16_comp((unsigned char *)ind->uuid, ind->uuid_len, ATT_SVC_CONTINUOUS_GLUCOSE_MONITORING))
            {
                // Retrieve WS characteristics and descriptors
                prf_extract_svc_info(ind, CGMC_CHAR_MAX, &cgmc_cgm_char[0],  &cgmc_env->env[conidx]->cgmc.chars[0],
                        CGMC_DESC_MAX, &cgmc_cgm_char_desc[0], &cgmc_env->env[conidx]->cgmc.descs[0]);

                //Even if we get multiple responses we only store 1 range
                cgmc_env->env[conidx]->cgmc.svc.shdl = ind->start_hdl;
                cgmc_env->env[conidx]->cgmc.svc.ehdl = ind->end_hdl;

                cgmc_env->env[conidx]->nb_svc++;
            }
        }
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see CGMC_ENABLE_REQ message.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

__STATIC int cgmc_enable_req_handler(ke_msg_id_t const msgid,
        struct cgmc_enable_req *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Status
    uint8_t status     = GAP_ERR_NO_ERROR;
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    uint8_t state = ke_state_get(dest_id);
    // Continuous Glucose Monitoring Collector Role Task Environment
    struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);

    ASSERT_INFO(cgmc_env != NULL, dest_id, src_id);
    if ((state == CGMC_IDLE) && (cgmc_env->env[conidx] == NULL))
    {
        // allocate environment variable for task instance
        cgmc_env->env[conidx] = (struct cgmc_cnx_env*) ke_malloc(sizeof(struct cgmc_cnx_env),KE_MEM_ATT_DB);
        memset(cgmc_env->env[conidx], 0, sizeof(struct cgmc_cnx_env));

        // Start discovering
        if (param->con_type == PRF_CON_DISCOVERY)
        {
            // Start discovery with Continuous Glucose Monitoring
            prf_disc_svc_send(&(cgmc_env->prf_env), conidx, ATT_SVC_CONTINUOUS_GLUCOSE_MONITORING);

            // Go to DISCOVERING SERVICE state
            ke_state_set(dest_id, CGMC_DISCOVERING_SVC);
        }
        //normal connection, get saved att details
        else
        {
            uint16_t search_hdl;
            struct prf_svc *svc;
            cgmc_env->env[conidx]->cgmc = param->cgmc;

            // start reading features
            //send APP confirmation that can start normal connection to TH
            // Attribute Handle
            search_hdl = cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_FEATURES].val_hdl;
            // Service
            svc = &cgmc_env->env[conidx]->cgmc.svc;
            
            // Check if handle is viable
            if ((svc != NULL) && (search_hdl != ATT_INVALID_SEARCH_HANDLE))
            {
                // Force the operation value
                cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_READ_FEATURES_AT_INIT;

                // Send the write request
                prf_read_char_send(&(cgmc_env->prf_env), conidx, svc->shdl, svc->ehdl, search_hdl);
                // Go to the Busy state
                ke_state_set(dest_id, CGMC_BUSY);

                status = GAP_ERR_NO_ERROR;
            }

        }
    }
    else if (state != CGMC_FREE)
    {
        // The message will be forwarded towards the good task instance
       status = PRF_ERR_REQ_DISALLOWED;
    }

    if(status != GAP_ERR_NO_ERROR)
    {
        // The request is disallowed (profile already enabled for this connection, or not enough memory, ...)
        cgmc_enable_rsp_send(cgmc_env, conidx, status);
    }

    return (KE_MSG_CONSUMED);
}




/**
 ****************************************************************************************
 * @brief Handles reception of the @see CGMC_READ_CMD message from the application.
 * @brief To read the Feature Characteristic in the peer server.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

__STATIC int cgmc_read_cmd_handler(ke_msg_id_t const msgid,
        struct cgmc_read_cmd *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    // Message status
    int msg_status = KE_MSG_CONSUMED;
    // Get the address of the environment
    struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    if (state == CGMC_IDLE)
    {
        ASSERT_INFO(cgmc_env != NULL, dest_id, src_id);
        // environment variable not ready
        if(cgmc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
            // Attribute Handle
            uint16_t handle = ATT_INVALID_SEARCH_HANDLE;
            // Service
            struct prf_svc *svc = &cgmc_env->env[conidx]->cgmc.svc;

            if ((svc != NULL) && (svc->shdl != ATT_INVALID_SEARCH_HANDLE))
            {
                if (param->operation == CGMC_OP_CODE_READ_FEATURES)
                {
                    // handle = cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_FEATURES].val_hdl;
                    // immediate results as features kept in the profile
                    // Send value ind and next complete event.
                    //indication Build a CGMC_VALUE_IND message
                    struct cgmc_value_ind* ind = KE_MSG_ALLOC(CGMC_VALUE_IND,
                                               prf_dst_task_get(&cgmc_env->prf_env, conidx),
                                               prf_src_task_get(&cgmc_env->prf_env, conidx),
                                               cgmc_value_ind);
                    ind->operation = CGMC_OP_CODE_READ_FEATURES;
                    ind->value.feature.cgm_feature     = cgmc_env->env[conidx]->cgm_feature;
                    ind->value.feature.type_sample     = cgmc_env->env[conidx]->type_sample;
                    ind->value.feature.sample_location = cgmc_env->env[conidx]->sample_location;
                    
                    // send indication to the application
                    ke_msg_send(ind);

                    status = GAP_ERR_NO_ERROR;
                    cgmc_send_cmp_evt(cgmc_env, conidx, CGMC_OP_CODE_READ_FEATURES, status);
                    
                    cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_NO_OP;
                    return msg_status;
                }
                else if (param->operation == CGMC_OP_CODE_READ_STATUS)
                {
                    handle = cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_STATUS].val_hdl;
                    cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_READ_STATUS;
                }
                else if (param->operation == CGMC_OP_CODE_READ_SESSION_START_TIME)
                {
                    handle = cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_SESSION_START_TIME].val_hdl;
                    cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_READ_SESSION_START_TIME;
                }
                else if (param->operation == CGMC_OP_CODE_READ_SESSION_RUN_TIME)
                {
                    handle = cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_SESSION_RUN_TIME].val_hdl;
                    cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_READ_SESSION_RUN_TIME;
                }
            }

            // Check if handle is viable
            if (handle != ATT_INVALID_SEARCH_HANDLE)
            {
                // Send the read request
                prf_read_char_send(&(cgmc_env->prf_env), conidx, svc->shdl, svc->ehdl, handle);
                // Go to the Busy state
                ke_state_set(dest_id, CGMC_BUSY);

                status = GAP_ERR_NO_ERROR;
            }
            else
            {
                status =  PRF_ERR_INEXISTENT_HDL;
            }

        }
    }
    else if (state == CGMC_FREE)
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
        cgmc_send_cmp_evt(cgmc_env, conidx, param->operation, status);
    }

    return msg_status;
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see CGMC_WRITE_SESS_START_TIME_CMD message from the application.
 * @brief To write command to the Records Access Control Point in the peer server.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

__STATIC int cgmc_write_sess_start_time_cmd_handler(ke_msg_id_t const msgid,
        struct cgmc_write_sess_start_time_cmd *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    // Message status
    int msg_status = KE_MSG_CONSUMED;
    // Get the address of the environment
    struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    if (state == CGMC_IDLE)
    {
        ASSERT_INFO(cgmc_env != NULL, dest_id, src_id);
        // environment variable not ready
        if(cgmc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
            // Attribute Handle
            uint16_t search_hdl = cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_SESSION_START_TIME].val_hdl;
            // Service
            struct prf_svc *svc = &cgmc_env->env[conidx]->cgmc.svc;

           // Check if handle is viable
            if ((svc != NULL) && (search_hdl != ATT_INVALID_SEARCH_HANDLE))
            {
                 // Length (7 -packed data 1 - zone 1- offset, 2-CRC)
                uint8_t length= CGMP_IDX_SESSION_START_TIME_SIZE_MAX;
                uint8_t data[CGMP_IDX_SESSION_START_TIME_SIZE_MAX];
                
                // fill parameters
                prf_pack_date_time(&data[0], &param->session_start_time);
                data[7] = param->time_zone;
                data[8] = param->dst_offset;

                // add E2E-CRC
                if (cgmc_env->env[conidx]->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
                {
                    uint16_t crc = prf_e2e_crc(&data[0],length-CGMP_CRC_LEN);
                    co_write16p(&data[length-CGMP_CRC_LEN], crc);
                }
                else
                {   // less the crc 16bit
                    length -= 2; // sizeof(uint16_t)
                }
                
                // Force the operation value
                cgmc_env->env[conidx]->op_pending = CGMC_OP_CODE_WRITE_SESSION_START_TIME;

                // Send the write request
                prf_gatt_write(&(cgmc_env->prf_env), conidx, search_hdl, &data[0], length, GATTC_WRITE);
                // Go to the Busy state
                ke_state_set(dest_id, CGMC_BUSY);

                status = GAP_ERR_NO_ERROR;
            }
            else
            {
                status =  PRF_ERR_INEXISTENT_HDL;
            }
        }
    }
    else if (state == CGMC_FREE)
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
        cgmc_send_cmp_evt(cgmc_env, conidx, CGMC_OP_CODE_WRITE_CTRL_PT, status);
    }

    return msg_status;
}



/**
 ****************************************************************************************
 * @brief Handles reception of the @see CGMC_WRITE_RACP_CMD message from the application.
 * @brief To write command to the Records Access Control Point in the peer server.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

__STATIC int cgmc_write_racp_cmd_handler(ke_msg_id_t const msgid,
        struct cgmc_write_racp_cmd *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    // Message status
    int msg_status = KE_MSG_CONSUMED;
    // Get the address of the environment
    struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    if (state == CGMC_IDLE)
    {
        ASSERT_INFO(cgmc_env != NULL, dest_id, src_id);
        // environment variable not ready
        if(cgmc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
            // Service
            struct prf_svc *svc = &cgmc_env->env[conidx]->cgmc.svc;
            // Attribute Handle
            uint16_t search_hdl = cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_RACP].val_hdl;

            if ((svc != NULL) && (search_hdl != ATT_INVALID_SEARCH_HANDLE))
            {
                 // Length
                uint8_t length= 2+5; // op_code+operand + filter +2x2byte time offset
                uint8_t data[2+5];

                // fill parameters
                data[0] = param->cp_opcode;
                data[1] = param->cp_operator;

                switch (param->cp_opcode)
                {
                    case CGMP_OPCODE_REPORT_STORED_RECORDS:
                    case CGMP_OPCODE_REPORT_NUMBER_OF_STORED_RECORDS:
                    case CGMP_OPCODE_DELETE_STORED_RECORDS:
                    {
                        switch (param->cp_operator)
                        {
                            case CGMP_OP_ALL_RECS:
                            case CGMP_OP_FIRST_REC:
                            case CGMP_OP_LAST_REC:
                            {
                                length = 2;
                            } break;
                            
                            case CGMP_OP_LT_OR_EQ:
                            {
                                data[2] = CGMP_FILTER_TIME_OFFSET; // filter type
                                co_write16p(&data[3], param->max_time_offset);
                                length = 5;
                            } break;
                            
                            case CGMP_OP_GT_OR_EQ:
                            {
                                data[2] = CGMP_FILTER_TIME_OFFSET; // filter type
                                co_write16p(&data[3], param->min_time_offset);
                                length = 5;
                            } break;
                            
                            case CGMP_OP_WITHIN_RANGE_OF:
                            {
                                data[2] = CGMP_FILTER_TIME_OFFSET; // filter type
                                co_write16p(&data[3], param->min_time_offset);
                                co_write16p(&data[5], param->max_time_offset);
                                length = 7;
                            } break;
                            default:
                            {
                                length = 0; // don't send across
                            } break;
                        }
                    } break;
                    
                    case CGMP_OPCODE_ABORT_OPERATION:
                    {
                        // fill parameters
                        data[1] = CGMP_OP_NULL; // only 
                        length = 2;
                    } break;
                    
                    default:
                    {
                        length = 0; // don't send across
                    } break;
                    
                }

               // Check if handle is viable
                if (length)
                {
                    // Force the operation value
                    cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_WRITE_RACP;

                    // Send the write request
                    prf_gatt_write(&(cgmc_env->prf_env), conidx, search_hdl, &data[0], length, GATTC_WRITE);
                    // Go to the Busy state
                    ke_state_set(dest_id, CGMC_BUSY);
                    // start the timer; will destroy the link if it expires
                    ke_timer_set(CGMC_CP_REQ_TIMEOUT, dest_id, CGMC_CP_TIMEOUT);


                    status = GAP_ERR_NO_ERROR;
                }
            }
            else
            {
                status =  PRF_ERR_INEXISTENT_HDL;
            }
        }
    }
    else if (state == CGMC_FREE)
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
        cgmc_send_cmp_evt(cgmc_env, conidx, CGMC_OP_CODE_WRITE_CTRL_PT, status);
    }

    return msg_status;
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see CGMC_WRITE_OPS_CTRL_PT_CMD message from the application.
 * @brief To write command to the Records Access Control Point in the peer server.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

__STATIC int cgmc_write_ops_ctrl_pt_cmd_handler(ke_msg_id_t const msgid,
        struct cgmc_write_ops_ctrl_pt_cmd *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    // Message status
    int msg_status = KE_MSG_CONSUMED;
    // Get the address of the environment
    struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    // data pointer
    uint8_t  *ptr;

    if (state == CGMC_IDLE)
    {
        ASSERT_INFO(cgmc_env != NULL, dest_id, src_id);
        // environment variable not ready
        if(cgmc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
            // Attribute Handle
            uint16_t search_hdl = cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_SPECIFIC_OPS_CTRL_PT].val_hdl;
            // Service
            struct prf_svc *svc = &cgmc_env->env[conidx]->cgmc.svc;

             // Length
            uint8_t length= 2*sizeof(uint8_t); //min size response consist of opcode & response
            uint8_t data[(2+CGMP_CALIBR_SIZE + CGMP_CRC_LEN)];

           // Check if handle is viable
            if ((svc != NULL) && (search_hdl != ATT_INVALID_SEARCH_HANDLE))
            {
                // treat request as correct
                status = GAP_ERR_NO_ERROR;
                // fill parameters
                data[0] = param->opcode;
                data[1] = param->response;
                
                ptr = &(data[2]);
                switch(param->opcode)
                {
                    case CGMP_OPS_CODE_SET_CGM_COMMUNICATION_INTERVAL: // 1,
                    {
                        co_write16p(ptr, param->operand.interval);
                        ptr +=2;
                        length += 2;
                    } break;
                    case CGMP_OPS_CODE_SET_GLUCOSE_CALIBRATION_VALUE: // 4,
                    {
                        // Glucose Concentration at Calibration
                        co_write16p(ptr, param->operand.calibration.cal_glucose_concentration);
                        ptr +=2;
                        length += 2;
                        // Calibration Time in minutes
                        co_write16p(ptr, param->operand.calibration.cal_time);
                        ptr +=2;
                        length += 2;
                        // Calibration Type of Sample @see common enum cgmp_type_sample_id
                        *ptr++ = param->operand.calibration.cal_type_sample;
                        length++;
                        // Calibration Location of Sample @see common enum cgmp_sample_location_id
                        *ptr++ = param->operand.calibration.cal_sample_location;
                        length++;
                        // Next Calibration Time in minutes
                        co_write16p(ptr, param->operand.calibration.next_cal_time);
                        ptr +=2;
                        length += 2;
                        // Calibration Record Number
                        co_write16p(ptr, param->operand.calibration.cal_record_number);
                        ptr +=2;
                        length += 2;
                        // Calibration Status - bitfield @see common CGMP_CAL_STATUS_xx
                        *ptr++ = param->operand.calibration.cal_status;
                        length++;
                    } break;
                    case CGMP_OPS_CODE_SET_PATIENT_HIGH_ALERT_LEVEL: // 7,
                    {
                        co_write16p(ptr, param->operand.patient_high_bg_value);
                        ptr +=2;
                        length += 2;
                    } break;
                    case CGMP_OPS_CODE_SET_PATIENT_LOW_ALERT_LEVEL: // 10,
                    {
                        co_write16p(ptr, param->operand.patient_low_bg_value);
                        ptr +=2;
                        length += 2;
                    } break;
                    case CGMP_OPS_CODE_SET_HYPO_ALERT_LEVEL: // 13,
                    {
                        co_write16p(ptr, param->operand.hypo_alert_level_value);
                        ptr +=2;
                        length += 2;
                    } break;
                    case CGMP_OPS_CODE_SET_HYPER_ALERT_LEVEL: // 16,
                    {
                        co_write16p(ptr, param->operand.hyper_alert_level_value);
                        ptr +=2;
                        length += 2;
                    } break;
                    case CGMP_OPS_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL: // 19,
                    {
                        co_write16p(ptr, param->operand.rate_of_decrease_alert_level_value);
                        ptr +=2;
                        length += 2;
                    } break;
                    case CGMP_OPS_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL: // 22,
                    {
                        co_write16p(ptr, param->operand.rate_of_increase_alert_level_value);
                        ptr +=2;
                        length += 2;
                    } break;
                    
                    // valid codes with no parameters
                    case CGMP_OPS_CODE_GET_CGM_COMMUNICATION_INTERVAL: // 2,
                    case CGMP_OPS_CODE_GET_GLUCOSE_CALIBRATION_VALUE: // 5,
                    case CGMP_OPS_CODE_GET_PATIENT_HIGH_ALERT_LEVEL: // 8,
                    case CGMP_OPS_CODE_GET_PATIENT_LOW_ALERT_LEVEL: // 11,
                    case CGMP_OPS_CODE_GET_HYPO_ALERT_LEVEL: // 14,
                    case CGMP_OPS_CODE_GET_HYPER_ALERT_LEVEL: // 17,
                    case CGMP_OPS_CODE_GET_RATE_OF_DECREASE_ALERT_LEVEL: // 20,
                    case CGMP_OPS_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL: // 23,
                    case CGMP_OPS_CODE_RESET_DEVICE_SPECIFIC_ALERT: // 25,
                    case CGMP_OPS_CODE_START_THE_SESSION: // 26,
                    case CGMP_OPS_CODE_STOP_THE_SESSION: // 27,
                    {
                    } break;
                    // invalid codes for request
                    case CGMP_OPS_CODE_CGM_COMMUNICATION_INTERVAL_RESPONSE: // 3,
                    case CGMP_OPS_CODE_GLUCOSE_CALIBRATION_VALUE_RESPONSE: // 6,
                    case CGMP_OPS_CODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE: // 9,
                    case CGMP_OPS_CODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE: // 12,
                    case CGMP_OPS_CODE_HYPO_ALERT_LEVEL_RESPONSE: // 15,
                    case CGMP_OPS_CODE_HYPER_ALERT_LEVEL_RESPONSE: // 18,
                    case CGMP_OPS_CODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE: // 21,
                    case CGMP_OPS_CODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE: // 24,
                    case CGMP_OPS_CODE_RESPONSE_CODE: // 28,
                    default :
                    {
                        status = PRF_ERR_INVALID_PARAM;
                    } break;
                }

                if (status == GAP_ERR_NO_ERROR)
                {
                    // add E2E-CRC if applicable
                    if (cgmc_env->env[conidx]->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
                    {
                        uint16_t crc = prf_e2e_crc(&data[0],length);
                        co_write16p(ptr, crc);
                        ptr +=CGMP_CRC_LEN;
                        length += CGMP_CRC_LEN;
                    }
                    
                    // Force the operation value
                    cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_WRITE_SPECIFIC_OPS_CTRL_PT;

                    // Send the write request
                    prf_gatt_write(&(cgmc_env->prf_env), conidx, search_hdl, &data[0], length, GATTC_WRITE);
                    // Go to the Busy state
                    ke_state_set(dest_id, CGMC_BUSY);
                    // start the timer; will destroy the link if it expires
                    ke_timer_set(CGMC_CP_REQ_TIMEOUT, dest_id, CGMC_CP_TIMEOUT);
                }
            }
            else
            {
                status =  PRF_ERR_INEXISTENT_HDL;
            }
        }
    }
    else if (state == CGMC_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        if (cgmc_env->env[conidx]->op_pending == CGMC_OP_PENDING_WRITE_SPECIFIC_OPS_CTRL_PT)
        {
            // Another Control Point request is pending
            status = PRF_PROC_IN_PROGRESS;
        }
        else
        {
            // Another procedure is pending, keep the command for later
            msg_status = KE_MSG_SAVED;
            status = GAP_ERR_NO_ERROR;
        }
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        cgmc_send_cmp_evt(cgmc_env, conidx, CGMC_OP_CODE_WRITE_CTRL_PT, status);
    }

    return msg_status;
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see CGMC_CFG_CCC_CMD message.
 * Allows the application to write new CCC values to a Alert Characteristic in the peer server
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int cgmc_cfg_ccc_cmd_handler(ke_msg_id_t const msgid,
                                   struct cgmc_cfg_ccc_cmd *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Status
    uint8_t status     = GAP_ERR_NO_ERROR;
    // Message status
    int msg_status = KE_MSG_CONSUMED;

    // Get the address of the environment
    struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);
    // Get connection index

    if (state == CGMC_IDLE)
    {

        ASSERT_INFO(cgmc_env != NULL, dest_id, src_id);
        // environment variable not ready
        if(cgmc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
             // Attribute Handle
            uint16_t handle = ATT_INVALID_SEARCH_HANDLE;
            // Service
            struct prf_svc *svc = &cgmc_env->env[conidx]->cgmc.svc;
            if (param->operation == CGMC_OP_CODE_MEASUREMENT)
            {
                handle = cgmc_env->env[conidx]->cgmc.descs[CGMC_DESC_MEASUREMENT_CCC].desc_hdl;
            }
            else
            if (param->operation == CGMC_OP_CODE_RACP_CMD)
            {
                handle = cgmc_env->env[conidx]->cgmc.descs[CGMC_DESC_RACP_CCC].desc_hdl;
            }
            else
            if (param->operation == CGMC_OP_CODE_SPECIFIC_OPS_CTRL_PT)
            {
                handle = cgmc_env->env[conidx]->cgmc.descs[CGMC_DESC_SPECIFIC_OPS_CTRL_PT_CCC].desc_hdl;
            }

            // Check if handle is viable
            if ((svc != NULL) && (handle != ATT_INVALID_SEARCH_HANDLE))
            {
                // Force the operation value
                cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_WRITE_CCC;

                // Send the write request
                prf_gatt_write_ntf_ind(&(cgmc_env->prf_env), conidx, handle, param->ccc);

                // Go to the Busy state
                ke_state_set(dest_id, CGMC_BUSY);

                status = GAP_ERR_NO_ERROR;
            }
            else
            {
                status = PRF_ERR_INEXISTENT_HDL;
            }
        }
    }
    else if (state == CGMC_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        // Another procedure is pending, keep the command for later
        msg_status = KE_MSG_SAVED;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        cgmc_send_cmp_evt(cgmc_env, conidx, CGMC_OP_CODE_WRITE_CCC, status);
    }

    return msg_status;
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
__STATIC int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                 struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);

    if (cgmc_env != NULL)
    {
        // Status
        uint8_t status = PRF_APP_ERROR;
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t state = ke_state_get(dest_id);

        if (state == CGMC_DISCOVERING_SVC)
        {
            // clean state for the new discovering
            cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_NO_OP;
            
            if ((param->status == GAP_ERR_NO_ERROR) || (param->status == ATT_ERR_ATTRIBUTE_NOT_FOUND))
            {
                // Check service (if found)
                if(cgmc_env->env[conidx]->nb_svc)
                {
                    // Check service (mandatory)
                    status = prf_check_svc_char_validity(CGMC_CHAR_MAX,
                            cgmc_env->env[conidx]->cgmc.chars,
                            cgmc_cgm_char);

                    // Check Descriptors (mandatory)
                    if(status == GAP_ERR_NO_ERROR)
                    {
                        status = prf_check_svc_char_desc_validity(CGMC_DESC_MAX,
                                cgmc_env->env[conidx]->cgmc.descs,
                                cgmc_cgm_char_desc,
                                cgmc_env->env[conidx]->cgmc.chars);
                    }
                }
            }
            
            // request read FEATURES
            if (status == GAP_ERR_NO_ERROR)
            {
                // Attribute Handle
                uint16_t search_hdl = cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_FEATURES].val_hdl;
                // Service
                struct prf_svc *svc = &cgmc_env->env[conidx]->cgmc.svc;
                
               // Check if handle is viable
                if ((svc != NULL) && (search_hdl != ATT_INVALID_SEARCH_HANDLE))
                {
                    // Force the operation value
                    cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_READ_FEATURES_AT_INIT;

                    // Send the read request
                    prf_read_char_send(&(cgmc_env->prf_env), conidx, svc->shdl, svc->ehdl, search_hdl);
                    // Go to the Busy state
                    ke_state_set(dest_id, CGMC_BUSY);

                }
                else
                {
                    // Raise an CGMC_ENABLE_REQ complete event.
                    // Was an issue reading features
                    status = ATT_ERR_INVALID_HANDLE;
                }
            }
            

            // at this stage we send response only if it fail in some case
            if (status != GAP_ERR_NO_ERROR)
            {
                // Raise an CGMC_ENABLE_REQ complete event.
                ke_state_set(dest_id, CGMC_IDLE);

                cgmc_enable_rsp_send(cgmc_env, conidx, param->status);
            }
        }
        else if (state == CGMC_BUSY)
        {
            uint8_t op_pending = cgmc_env->env[conidx]->op_pending;
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
                            case CGMC_OP_PENDING_READ_FEATURES_AT_INIT:
                            {
                               // Raise an CGMC_ENABLE_REQ complete event.
                               // Was an issue reading features
                               cgmc_enable_rsp_send(cgmc_env, conidx, status);
                            }
                            break;
                            case CGMC_OP_PENDING_READ_FEATURES:
                            {
                                // Send read complete event.
                                cgmc_send_cmp_evt(cgmc_env, conidx, CGMC_OP_CODE_READ_FEATURES, status);
                            }
                            break;

                            default:
                            break;
                        }
                        ke_state_set(dest_id, CGMC_IDLE);
                    }
                }
                break;

                case GATTC_WRITE:
                case GATTC_WRITE_NO_RESPONSE:
                {
                    uint8_t operation = CGMC_OP_PENDING_NO_OP;

                    switch(op_pending)
                    {
                        case CGMC_OP_PENDING_WRITE_RACP:
                        {
                            operation = CGMC_OP_CODE_WRITE_CTRL_PT;
                        } break;
                        case CGMC_OP_CODE_WRITE_SESSION_START_TIME:
                        {
                            operation = CGMC_OP_CODE_WRITE_SESSION_START_TIME;
                        } break;
                        case CGMC_OP_PENDING_WRITE_CCC:
                        {
                            operation = CGMC_OP_CODE_WRITE_CCC;
                        } break;
                        case CGMC_OP_PENDING_WRITE_SPECIFIC_OPS_CTRL_PT:
                        {
                            operation = CGMC_OP_CODE_WRITE_OPS_CTRL_PT;
                        } break;
                        default:
                            break;
                    }
                    // Send the complete event status
                    cgmc_send_cmp_evt(cgmc_env, conidx, operation, param->status);
                    ke_state_set(dest_id, CGMC_IDLE);

                } break;

                case GATTC_REGISTER:
                {
                    if (status != GAP_ERR_NO_ERROR)
                    {
                        // Send the complete event error status
                        cgmc_send_cmp_evt(cgmc_env, conidx, CGMC_OP_CODE_GATTC_REGISTER, param->status);
                    }
                    // Go to connected state
                    ke_state_set(dest_id, CGMC_IDLE);
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
            cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_NO_OP;
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
__STATIC int gattc_read_ind_handler(ke_msg_id_t const msgid,
                                    struct gattc_read_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == CGMC_BUSY)
    {
        // Get the address of the environment
        struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);


        uint8_t conidx = KE_IDX_GET(dest_id);
        // Service
        uint8_t op_pending;
        uint8_t status = GAP_ERR_NO_ERROR;

        // E2E crc field
        uint16_t  feature_crc;
        // calculated crc
        // e2e crc to protect the fields
        // predefined value when crc is not supported
        // this field exist in all the cases
        uint16_t crc = 0xFFFF;
        
        // Send value ind and next complete event.
        // indication in a CGMC_VALUE_IND message
        struct cgmc_value_ind* ind;

        ASSERT_INFO(cgmc_env != NULL, dest_id, src_id);
        ASSERT_INFO(cgmc_env->env[conidx] != NULL, dest_id, src_id);

        // Find the op_pending -  and check it is a valid read code.
        op_pending = cgmc_env->env[conidx]->op_pending;
        
        switch (op_pending)
        {
            case CGMC_OP_PENDING_READ_FEATURES_AT_INIT:
            case CGMC_OP_PENDING_READ_FEATURES:
            {
                // If enabled in CGM Features @see common cgm_features
                uint32_t cgm_feature;

                cgm_feature     = co_read24p(&param->value[0]);

                feature_crc     = co_read16p(&param->value[4]);

                // add E2E-CRC
                if (cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
                {
                    crc = prf_e2e_crc(&param->value[0], 3+1); // 24bits 4bit 4bits
                }
                
                // only take reading if crc match
                if (feature_crc == crc)
                {
                    cgmc_env->env[conidx]->cgm_feature     = cgm_feature;
                    cgmc_env->env[conidx]->type_sample     = (param->value[3]) & 0x0f;
                    cgmc_env->env[conidx]->sample_location = (param->value[3] >> 4) & 0x0f;
                }
                else
                {
                    status = CGM_ERROR_INVALID_CRC;
                }
                    
                if (op_pending == CGMC_OP_PENDING_READ_FEATURES_AT_INIT)
                {
                    // Raise an CGMC_ENABLE_RSP complete event.
                    cgmc_enable_rsp_send(cgmc_env, conidx, status);
                    // no need for complete event at the end
                    status = GAP_ERR_NO_ERROR;
                }
                
            } break;
            
            case CGMC_OP_PENDING_READ_STATUS:
            case CGMC_OP_PENDING_READ_SESSION_START_TIME:
            case CGMC_OP_PENDING_READ_SESSION_RUN_TIME:
            {
                // Offset
                uint8_t offs = 0;
                // Send value ind and next complete event.
                ind = KE_MSG_ALLOC(CGMC_VALUE_IND,
                                           prf_dst_task_get(&cgmc_env->prf_env, conidx),
                                           prf_src_task_get(&cgmc_env->prf_env, conidx),
                                           cgmc_value_ind);
                if (op_pending == CGMC_OP_PENDING_READ_STATUS)
                {
                    ind->operation = CGMC_OP_CODE_READ_STATUS;
                    
                    ind->value.status.time_offset     = co_read16p(&param->value[0]);
                    ind->value.status.warning         = param->value[2];
                    ind->value.status.cal_temp        = param->value[3];
                    ind->value.status.annunc_status   = param->value[4];
                    offs = 5;
                }
                else if (op_pending == CGMC_OP_PENDING_READ_SESSION_START_TIME)
                {
                    ind->operation = CGMC_OP_CODE_READ_SESSION_START_TIME;
                    
                    offs = prf_unpack_date_time((uint8_t *) &(param->value[0]), &(ind->value.start_time.session_start_time));
                    ind->value.start_time.time_zone         = param->value[offs];
                    ind->value.start_time.dst_offset        = param->value[offs+1];
                    offs += 2;
                }
                else if (op_pending == CGMC_OP_PENDING_READ_SESSION_RUN_TIME)
                {
                    ind->operation = CGMC_OP_CODE_READ_SESSION_RUN_TIME;
                    
                    ind->value.run_time.run_time     = co_read16p(&param->value[0]);
                    offs = 2;
                }

                // check E2E-CRC
                if (cgmc_env->env[conidx]->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
                {
                    if (param->length < (offs+2)) // 16bit 24bits +16bit CRC
                    {
                        status = CGM_ERROR_MISSING_CRC;
                    }
                    else
                    {
                        feature_crc     = co_read16p(&param->value[offs]);
                        crc = prf_e2e_crc(&param->value[0], offs); // 16bit 24bits
                        // only take reading if crc match
                        if (feature_crc != crc)
                        {
                            status = CGM_ERROR_INVALID_CRC;
                        }
                    }
                }
                
                if (status == GAP_ERR_NO_ERROR)
                {
                    // send indication to the application
                    ke_msg_send(ind);
                }
                else
                {
                    // No need to send prepared message
                    KE_MSG_FREE(ind);
                }
                // event complete sent as a result of operation command request
                cgmc_send_cmp_evt(cgmc_env, conidx, op_pending == CGMC_OP_PENDING_READ_STATUS?CGMC_OP_CODE_READ_STATUS: op_pending == CGMC_OP_PENDING_READ_SESSION_START_TIME?CGMC_OP_CODE_READ_SESSION_START_TIME:CGMC_OP_CODE_READ_SESSION_RUN_TIME, status);
            } break;

            default:
            {
                ASSERT_ERR(0);
            } break;
        }
        
        // translation from op_pending to NO_OP opcode
        cgmc_env->env[conidx]->op_pending = CGMC_OP_PENDING_NO_OP;

        ke_state_set(dest_id, CGMC_IDLE);

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
__STATIC int gattc_event_ind_handler(ke_msg_id_t const msgid,
                                   struct gattc_event_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);

    if (cgmc_env != NULL)
    {
        if ((param->type == GATTC_INDICATE) || (param->type == GATTC_NOTIFY))
        {
            uint8_t conidx = KE_IDX_GET(dest_id);
            // data pointer
            const uint8_t  *ptr;
            // length of data block
            uint16_t length;

            // E2E crc field
            uint16_t  feature_crc;
            // calculated crc
            // e2e crc to protect the fields
            uint16_t crc;
            //indication Build a CGMC_VALUE_IND message
            struct cgmc_value_ind* ind = NULL;

            length = param->length;
            
            if(param->handle == cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_RACP].val_hdl)
            {   // no crc for legacy reasons
                ind = KE_MSG_ALLOC(CGMC_VALUE_IND,
                                           prf_dst_task_get(&cgmc_env->prf_env, conidx),
                                           prf_src_task_get(&cgmc_env->prf_env, conidx),
                                           cgmc_value_ind);
                memset(ind, 0, sizeof(struct cgmc_value_ind));
                ind->operation = CGMC_OP_CODE_RACP_CMD;

                // RACP response carry number of records or Requested OpCode+ Response code
                ind->value.racp_resp.cp_opcode = param->value[0];
                // in both cases we don't care about the Operator (param->value[1]) - it should be Null(0x00)
                if (param->value[0] == CGMP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP)
                {
                    ind->value.racp_resp.number_of_records = co_read16p(&param->value[2]);
                }
                else
                {
                    // Request Control Point OpCode @see enum cgmp_cp_opcodes_id
                    ind->value.racp_resp.request_cp_opcode = param->value[2];
                    // Response Code @see enum cgmp_cp_resp_code_id
                    ind->value.racp_resp.response_code = param->value[3];
                }
                // stop timer.
                ke_timer_clear(CGMC_CP_REQ_TIMEOUT, dest_id);
            }
            else
            {
                //check E2E crc 
                if (cgmc_env->env[conidx]->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
                {
                    feature_crc  = co_read16p(&param->value[length-CGMP_CRC_LEN]);

                    // calc E2E-CRC
                    crc = prf_e2e_crc(&param->value[0], length-CGMP_CRC_LEN);
                
                    // only take reading if crc match
                    if (feature_crc != crc)
                    {
                        uint8_t operation = CGMC_OP_CODE_SPECIFIC_OPS_CTRL_PT;
                        if(param->handle == cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_MEASUREMENT].val_hdl)
                        {
                            operation = CGMC_OP_CODE_MEASUREMENT;
                        }
                        // Send the complete event status
                        cgmc_send_cmp_evt(cgmc_env, conidx, operation, CGM_ERROR_INVALID_CRC);
                    }
                    else
                    {
                        ind = KE_MSG_ALLOC(CGMC_VALUE_IND,
                                           prf_dst_task_get(&cgmc_env->prf_env, conidx),
                                           prf_src_task_get(&cgmc_env->prf_env, conidx),
                                           cgmc_value_ind);
                        memset(ind, 0, sizeof(struct cgmc_value_ind));

                        if(param->handle == cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_MEASUREMENT].val_hdl)
                        {
                            uint8_t flags = param->value[1];
                            
                            ind->operation = CGMC_OP_CODE_MEASUREMENT;
                            ind->value.meas.flags = flags;
                            // CGM Glucose Concentration (SFLOAT/16bit)
                            ptr = &(param->value[2]);
                            ind->value.meas.glucose_concentration = co_read16p(ptr);
                            ptr +=2;
                            // Time Offset
                            ind->value.meas.time_offset = co_read16p(ptr);
                            ptr +=2;

                            // verify flags vs supported_features
                            // annunciation data
                            if (CMPF(flags, CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_WARNING_PRESENT))
                            {
                                ind->value.meas.warning = *(ptr++);
                            }
                            if (CMPF(flags, CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_CAL_TEMP_PRESENT))
                            {
                                ind->value.meas.cal_temp = *(ptr++);
                            }
                            if (CMPF(flags, CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_STATUS_PRESENT))
                            {
                                ind->value.meas.status = *(ptr++);
                            }
                            
                            // Trend data
                            if (CMPF(flags, CGMS_MEAS_FLAGS_CGM_TREND_INFORMATION_PRESENT))
                            {
                                ind->value.meas.trend_information = co_read16p(ptr);
                                ptr += 2;
                            }
                            
                            // Quality data
                            if (CMPF(flags, CGMS_MEAS_FLAGS_CGM_QUALITY_PRESENT))
                            {
                                ind->value.meas.quality = co_read16p(ptr);
                                ptr += 2;
                            }
                            
                            // start the timer; will destroy the link if it expires
                            ke_timer_set(CGMC_CP_REQ_TIMEOUT, dest_id, CGMC_CP_TIMEOUT);
                        }
                        else if(param->handle == cgmc_env->env[conidx]->cgmc.chars[CGMC_CHAR_SPECIFIC_OPS_CTRL_PT].val_hdl)
                        {
                            // Control Point OpCode @see enum cgmp_cp_opcodes_id
                            uint8_t cp_opcode = param->value[0];
                            union cgmp_ops_operand *operand = &ind->value.ops_cp_resp.operand;
                            
                            ind->operation = CGMC_OP_CODE_SPECIFIC_OPS_CTRL_PT;
                            
                            // Op Code
                            ind->value.ops_cp_resp.opcode = param->value[0];
                            // Response
                            ind->value.ops_cp_resp.response = param->value[1];
                            //
                            ptr = &(param->value[2]);
                            // operand based on op code
                            switch (cp_opcode)
                            {
                            case  1://     Communication interval in minutes
                            case  3://     Communication interval in minutes
                                operand->interval = co_read16p(&(param->value[2]));
                                ptr += 2;
                                break;
                            case  5://     Calibration Data Record Number 
                                operand->cal_data_record_number = co_read16p(&(param->value[2]));
                                ptr += 2;
                                break;
                            case  7://     Patient High bG value in mg/dL 
                            case  9://     Patient High bG value in mg/dL 
                                operand->patient_high_bg_value = co_read16p(&(param->value[2]));
                                ptr += 2;
                                break;
                            case 10://     Patient Low bG value in mg/dL 
                            case 12://     Patient Low bG value in mg/dL 
                                operand->patient_low_bg_value = co_read16p(&(param->value[2]));
                                ptr += 2;
                                break;
                            case 13://     Hypo Alert Level value in mg/dL 
                            case 15://     Hypo Alert Level value in mg/dL 
                                operand->hypo_alert_level_value = co_read16p(&(param->value[2]));
                                ptr += 2;
                                break;
                            case 16://     Hyper Alert Level value in mg/dL
                            case 18://     Hyper Alert Level value in mg/dL 
                                operand->hyper_alert_level_value = co_read16p(&(param->value[2]));
                                ptr += 2;
                                break;
                            case 19://     Rate of Decrease Alert Level value in mg/dL/min 
                            case 21://     Rate of Decrease Alert Level value in mg/dL/min 
                                operand->rate_of_decrease_alert_level_value = co_read16p(&(param->value[2]));
                                ptr += 2;
                                break;
                            case 22://     Rate of Increase Alert Level value in mg/dL/min 
                            case 24://     Rate of Increase Alert Level value in mg/dL/min 
                                operand->rate_of_increase_alert_level_value = co_read16p(&(param->value[2]));
                                ptr += 2;
                                break;
                            case 28://     Request Op Code, Response Code Value
                                operand->response_code.request_op_code     = param->value[2];
                                operand->response_code.response_code_value = param->value[3];
                                ptr += 2;
                                break;
                            case 4://     Operand value as defined in the Calibration Value Fields. 
                            case 6://     Calibration Data .  ???
                            {
                                struct cgm_calibration_operand * calibr = &ind->value.ops_cp_resp.operand.calibration;
                                *calibr = (struct cgm_calibration_operand){
                                    .cal_glucose_concentration = co_read16p(&(param->value[2])),
                                    .cal_time = co_read16p(&(param->value[4])),
                                    .cal_type_sample = param->value[6],
                                    .cal_sample_location = param->value[7],
                                    .next_cal_time = co_read16p(&(param->value[8])),
                                    .cal_record_number = co_read16p(&(param->value[10])),
                                    .cal_status = param->value[12]
                                };
                                ptr += CGMP_CALIBR_SIZE;
                            } break;
                            default: break; //NA
                            }
                            // stop timer.
                            ke_timer_clear(CGMC_CP_REQ_TIMEOUT, dest_id);

                        }
                    }
                }
            }
            if (ind != NULL)
            {           // send indication to the application
                ke_msg_send(ind);
                
                // confirm that indication has been correctly received
                struct gattc_event_cfm * cfm = KE_MSG_ALLOC(GATTC_EVENT_CFM, src_id, dest_id, gattc_event_cfm);
                cfm->handle = param->handle;
                ke_msg_send(cfm);
            }
        }
            // else
            // {
                // // error case
                // ASSERT_ERR(0);
            // }


            // else drop the message
        // }
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Control Point request not executed by peer device or is freezed.
 * It can be a connection problem, disconnect the link.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GATTC).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 *
 ****************************************************************************************
 */
__STATIC int cgmc_cp_req_timeout_handler(ke_msg_id_t const msgid, void const *param,
                                         ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t op_pending;
    uint8_t operation;

    if(state != CGMC_BUSY)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);

        ASSERT_INFO(cgmc_env != NULL, dest_id, src_id);
        ASSERT_INFO(cgmc_env->env[conidx] != NULL, dest_id, src_id);

        // inform that racp execution is into a timeout state
        op_pending = cgmc_env->env[conidx]->op_pending;
        operation = CGMC_OP_PENDING_NO_OP;

        switch(op_pending)
        {
            case CGMC_OP_PENDING_WRITE_RACP:
            {
                operation = CGMC_OP_CODE_WRITE_CTRL_PT;
            } break;
            case CGMC_OP_PENDING_WRITE_SPECIFIC_OPS_CTRL_PT:
            {
                operation = CGMC_OP_CODE_WRITE_OPS_CTRL_PT;
            } break;
            default:
                break;
        }
        // Send the complete event status
        cgmc_send_cmp_evt(cgmc_env, conidx, operation, PRF_ERR_PROC_TIMEOUT);
        ke_state_set(dest_id, CGMC_IDLE);
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Specifies the default message handlers
KE_MSG_HANDLER_TAB(cgmc)
{
    {CGMC_ENABLE_REQ,                   (ke_msg_func_t)cgmc_enable_req_handler},

    {GATTC_SDP_SVC_IND,                 (ke_msg_func_t)gattc_sdp_svc_ind_handler},
    {GATTC_CMP_EVT,                     (ke_msg_func_t)gattc_cmp_evt_handler},

    {GATTC_READ_IND,                    (ke_msg_func_t)gattc_read_ind_handler},
    {GATTC_EVENT_IND,                   (ke_msg_func_t)gattc_event_ind_handler},
    {GATTC_EVENT_REQ_IND,               (ke_msg_func_t)gattc_event_ind_handler},

    {CGMC_READ_CMD,                     (ke_msg_func_t)cgmc_read_cmd_handler},

    {CGMC_WRITE_SESS_START_TIME_CMD,    (ke_msg_func_t)cgmc_write_sess_start_time_cmd_handler},

    {CGMC_WRITE_RACP_CMD,               (ke_msg_func_t)cgmc_write_racp_cmd_handler},

    {CGMC_WRITE_OPS_CTRL_PT_CMD,        (ke_msg_func_t)cgmc_write_ops_ctrl_pt_cmd_handler},

    {CGMC_CFG_CCC_CMD,                  (ke_msg_func_t)cgmc_cfg_ccc_cmd_handler},
    {CGMC_CP_REQ_TIMEOUT,               (ke_msg_func_t)cgmc_cp_req_timeout_handler},
};


void cgmc_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct cgmc_env_tag *cgmc_env = PRF_ENV_GET(CGMC, cgmc);

    task_desc->msg_handler_tab = cgmc_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(cgmc_msg_handler_tab);
    task_desc->state           = cgmc_env->state;
    task_desc->idx_max         = CGMC_IDX_MAX;
}


#endif //(BLE_CGM_CLIENT)

/// @} CGMCTASK
