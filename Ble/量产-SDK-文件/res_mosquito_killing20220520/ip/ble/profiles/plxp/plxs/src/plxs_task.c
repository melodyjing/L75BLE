/**
 ****************************************************************************************
 *
 * @file plxs_task.c
 *
 * @brief Pulse Oximeter Profile Service Task Implementation.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup PLXSTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_PLX_SERVER)
#include "plxp_common.h"

#include "gapc.h"
#include "gattc_task.h"
#include "attm.h"
#include "plxs.h"
#include "plxs_task.h"
#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"
/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
static void plxs_racp_resp_send(ke_task_id_t const dest_id, ke_task_id_t const src_id, uint8_t cp_opcode, uint8_t request_cp_opcode, uint8_t response_code, uint16_t number_of_records);


// SEND RACP response indication
// if cp_opcode == PLXP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP
// then sends the number of stored records response using number_of_records
static void plxs_racp_resp_send(ke_task_id_t const dest_id, ke_task_id_t const src_id, uint8_t cp_opcode, uint8_t request_cp_opcode, uint8_t response_code, uint16_t number_of_records)
{
    // Allocate the GATT notification message
    struct gattc_send_evt_cmd *ind;
    // length of the data block
    uint16_t length;
    // handle to send the indication
    uint16_t handle;

    handle = plxs_att2handle(PLXS_IDX_RACP_VAL);
    length = 4; //OpCode -1oct,Operator=Null-1oct,Operand -2oct
    // Allocate the GATT notification message
    ind = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
            dest_id,
            src_id,
            gattc_send_evt_cmd, length);

    // Fill in the parameter structure
    ind->operation = GATTC_INDICATE;
    ind->handle = handle;
    ind->length = length;
    // Fill data
    // Procedure Op Code
    ind->value[0] = cp_opcode;
    // Operator - Null
    ind->value[1] = PLXP_OPERATOR_NULL;
    if (cp_opcode == PLXP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP)
    {
        co_write16p(&(ind->value[2]), number_of_records);
    }
    else
    {
        ind->value[2] = request_cp_opcode;
        ind->value[3] = response_code;
    }

    // send notification to peer device
    ke_msg_send(ind);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @see PLXS_ENABLE_REQ message.
 * @param[in] msgid Id of the message received
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int plxs_enable_req_handler(ke_msg_id_t const msgid,
                                    struct plxs_enable_req *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);
    uint8_t conidx = KE_IDX_GET(src_id);
    // Status
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    uint8_t state = ke_state_get(dest_id);


    if (state == PLXS_IDLE)
    { 
        /// CCCs for the current connection
        plxs_env->prfl_ind_cfg_spot[conidx] = param->ind_cfg_spot_meas;
        plxs_env->prfl_ind_cfg_cont[conidx] = param->ind_cfg_cont_meas;
        plxs_env->prfl_ind_cfg_racp[conidx] = param->ind_cfg_racp_meas;
        status = GAP_ERR_NO_ERROR;
    }
    
    // send completed information to APP task that contains error status
    struct plxs_enable_rsp *cmp_evt = KE_MSG_ALLOC(PLXS_ENABLE_RSP, src_id, dest_id, plxs_enable_rsp);
    cmp_evt->status     = status;

    ke_msg_send(cmp_evt);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the read request from peer device
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_read_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_CONSUMED;
    uint8_t state = ke_state_get(dest_id);
    // check that task is in idle state
    if(state == PLXS_IDLE)
    {
        // Get the address of the environment
        struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);
        uint8_t conidx = KE_IDX_GET(src_id);
        uint8_t att_idx = plxs_handle2att(param->handle);

        // Send data to peer device
        struct gattc_read_cfm* cfm = NULL;

        uint8_t status = ATT_ERR_NO_ERROR;

        switch(att_idx)
        {
            case PLXS_IDX_SPOT_MEASUREMENT_CCC:
            case PLXS_IDX_CONT_MEASUREMENT_CCC:
            case PLXS_IDX_RACP_CCC:
            {

                // Fill data
                cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(uint16_t));
                cfm->length = sizeof(uint16_t);
                switch(att_idx)
                {
                    case PLXS_IDX_SPOT_MEASUREMENT_CCC:
                    {/// CCC for Spot-Measurement 
                        co_write16p(cfm->value, plxs_env->prfl_ind_cfg_spot[conidx]);
                    } break;
                    case PLXS_IDX_CONT_MEASUREMENT_CCC:
                    {/// CCC for Cont-Measurement 
                        co_write16p(cfm->value, plxs_env->prfl_ind_cfg_cont[conidx]);
                    } break;
                    case PLXS_IDX_RACP_CCC:
                    {/// CCC for RACP 
                        co_write16p(cfm->value, plxs_env->prfl_ind_cfg_racp[conidx]);
                    } break;
                    default:
                    break;
                }
            } break;

            default:
            {
                cfm = KE_MSG_ALLOC(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm);
                cfm->length = 0;
                status = ATT_ERR_REQUEST_NOT_SUPPORTED;
            } break;
        }

        if (cfm != NULL)
        {
            cfm->handle = param->handle;
            cfm->status = status;

            // Send value to peer device.
            ke_msg_send(cfm);
        }

    }
    // else process it later
    else
    {
        msg_status = KE_MSG_SAVED;
    }
    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gattc_att_info_req_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Get the address of the environment
//    struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);
    uint8_t att_idx = plxs_handle2att(param->handle);
    struct gattc_att_info_cfm * cfm;

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    switch(att_idx)
    {
        case PLXS_IDX_SPOT_MEASUREMENT_CCC:
        case PLXS_IDX_CONT_MEASUREMENT_CCC:
        case PLXS_IDX_RACP_CCC:
        {
            // check if it's a client configuration char
            // CCC attribute length = 2
            cfm->length = sizeof(uint16_t);
            cfm->status = GAP_ERR_NO_ERROR;
        }break;

        case PLXS_IDX_RACP_VAL:
        {
            // control point 
            cfm->length = 4;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;

        default:
        {
            // not expected request
            cfm->length = 0;
            cfm->status = ATT_ERR_WRITE_NOT_PERMITTED;
        }break;
    }

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}



/**
 ****************************************************************************************
 * @brief Handles reception of the @see GATTC_WRITE_REQ_IND message.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                       struct gattc_write_req_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);
    uint8_t conidx = KE_IDX_GET(src_id);
    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;
    // Status
    uint8_t status = GAP_ERR_NO_ERROR;

    // Check the connection handle
    if (plxs_env != NULL)
    {
        uint8_t state = ke_state_get(dest_id);
        // check that task is in idle state
        if(state == PLXS_IDLE)
        {
            uint8_t att_idx = plxs_handle2att(param->handle);

            switch(att_idx)
            {
                case PLXS_IDX_SPOT_MEASUREMENT_CCC:
                case PLXS_IDX_CONT_MEASUREMENT_CCC:
                case PLXS_IDX_RACP_CCC:
                {
                    uint16_t ntf_cfg;
                    // Inform the HL about the new configuration
                    struct plxs_wr_char_ccc_ind *ind = KE_MSG_ALLOC(PLXS_WR_CHAR_CCC_IND,
                            prf_dst_task_get(&plxs_env->prf_env, conidx),
                            prf_src_task_get(&plxs_env->prf_env, conidx),
                            plxs_wr_char_ccc_ind);
                    // Get the value
                    co_write16p(&ntf_cfg, param->value[0]);

                    switch(att_idx)
                    {
                        case PLXS_IDX_SPOT_MEASUREMENT_CCC:
                        {/// CCC for Spot-Measurement 
                            ind->char_type = PLXS_SPOT_CHECK_MEAS_CODE;
                            plxs_env->prfl_ind_cfg_spot[conidx] = ntf_cfg;
                        } break;
                        case PLXS_IDX_CONT_MEASUREMENT_CCC:
                        {/// CCC for Cont-Measurement 
                            ind->char_type = PLXS_CONTINUOUS_MEAS_CODE;
                            plxs_env->prfl_ind_cfg_cont[conidx] = ntf_cfg;
                        } break;
                        case PLXS_IDX_RACP_CCC:
                        {/// CCC for RACP 
                            ind->char_type = PLXS_RACP_CODE;
                            plxs_env->prfl_ind_cfg_racp[conidx] = ntf_cfg;
                        } break;
                        default:
                        break;
                    }

                    ind->ind_cfg = ntf_cfg;

                    ke_msg_send(ind);
                }break;

                case PLXS_IDX_RACP_VAL:
                {
                    // if command to RACP and RACP is not configured for indication
                    // or Command to report Records and SPOT_MEAS CCC is not configured for indication
                    if (((plxs_env->prfl_ind_cfg_racp[conidx] & PLXP_CCC_IND_ENABLED) != PLXP_CCC_IND_ENABLED) ||
                        (( param->value[0] == PLXP_OPCODE_REPORT_STORED_RECORDS) && 
                        ((plxs_env->prfl_ind_cfg_spot[conidx] & PLXP_CCC_IND_ENABLED) != PLXP_CCC_IND_ENABLED)))
                    {// CPP improperly configured
                        status = PRF_CCCD_IMPR_CONFIGURED;
                    }
                    else
                    if ((plxs_env->racp_in_progress[conidx]) && (param->value[0] != PLXP_OPCODE_ABORT_OPERATION))
                    {// procedure already in progress
                        status = PRF_PROC_IN_PROGRESS;
                    }
                    else
                    // Check it is the valid request
                    if ((( param->value[0] >= PLXP_OPCODE_REPORT_STORED_RECORDS) && (param->value[0] <= PLXP_OPCODE_REPORT_NUMBER_OF_STORED_RECORDS)) &&
                        (((param->value[0] != PLXP_OPCODE_ABORT_OPERATION) && (param->value[1] == PLXP_OPERATOR_ALL_RECORDS)) ||
                         ((param->value[0] == PLXP_OPCODE_ABORT_OPERATION) && (param->value[1] == PLXP_OPERATOR_NULL)))
                        )
                    {
                        // Inform the HL about the new Control Point Command
                        struct plxs_wr_racp_ind *ind = KE_MSG_ALLOC(PLXS_WR_RACP_IND,
                                prf_dst_task_get(&plxs_env->prf_env, conidx),
                                prf_src_task_get(&plxs_env->prf_env, conidx),
                                plxs_wr_racp_ind);
                        // Get the value
                        ind->cp_opcode = param->value[0];
                        ind->cp_operator = param->value[1];

                        ke_msg_send(ind);
                        // set flag that procedure is started
                        plxs_env->racp_in_progress[conidx] = true;
                    }
                    else
                    { //error condition
                        uint8_t response_code = PLXP_RESP_INVALID_OPERATOR;
                        if ((param->value[0]==0) ||(param->value[0]>PLXP_OPCODE_REPORT_NUMBER_OF_STORED_RECORDS))
                        { // incorrect opcode
                            response_code = PLXP_RESP_OP_CODE_NOT_SUPPORTED;
                        }
                        else if ((param->value[0] != PLXP_OPCODE_ABORT_OPERATION) && (param->value[1] > PLXP_OPERATOR_ALL_RECORDS))
                        {
                            response_code = PLXP_RESP_OPERATOR_NOT_SUPPORTED;
                        }
                        else //if ((param->value[0] != PLXP_OPCODE_ABORT_OPERATION) && (param->value[1] == PLXP_OPERATOR_NULL))
                        {
                            response_code = PLXP_RESP_INVALID_OPERATOR;
                        }
                        // procedure complete
                        plxs_env->racp_in_progress[conidx] = 0;
                        plxs_racp_resp_send(
                                            src_id,
                                            dest_id,
                                            PLXP_OPCODE_RESPONSE_CODE, param->value[0], response_code, 0);
                        // special state as we don't need response to the server
                        ke_state_set(dest_id, PLXS_OP_INDICATE_RACP_NO_CMP);

                    }
                }break;

                default:
                {
                    // not expected request
                    status = ATT_ERR_WRITE_NOT_PERMITTED;
                }break;
            }
            
            // Send the write response to the peer device
            struct gattc_write_cfm *cfm = KE_MSG_ALLOC(
                    GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
            cfm->handle = param->handle;
            cfm->status = status;
            ke_msg_send(cfm);
        }
        else
        {
            msg_status = KE_MSG_SAVED;
        }
    }
    // else drop the message

    return (int)msg_status;
}

/**
 ****************************************************************************************
 * @brief Handles @see GATT_NOTIFY_CMP_EVT message meaning that an indication
 * has been correctly sent to peer device (but not confirmed by peer device).
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,  struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    // Get the address of the environment
    struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);

    uint8_t state = ke_state_get(dest_id);

    if (plxs_env != NULL)
    {
        // Check if a connection exists
        switch (param->operation)
        {
            case (GATTC_NOTIFY):
            case (GATTC_INDICATE):
            {
                // Spot-Measurement sent or RACP sent
                // Inform the application that a procedure has been completed
                if ((state == PLXS_OP_INDICATE_MEAS ) || (state == PLXS_OP_INDICATE_RACP ) || (state == PLXS_OP_NOTIFY ))
                {
                    /// Send a command complete to the App indicate msg could not be sent.
                    struct plxs_cmp_evt *cmp_evt = KE_MSG_ALLOC(PLXS_CMP_EVT,
                            prf_dst_task_get(&plxs_env->prf_env, conidx),
                            prf_src_task_get(&plxs_env->prf_env, conidx),
                            plxs_cmp_evt);

                    if (state == PLXS_OP_NOTIFY )
                    { // Continuous Measurement sent
                        cmp_evt->operation = PLXS_CONTINUOUS_MEAS_CMD_OP_CODE;
                    }
                    else
                    { // Indication is Spot-Meas Record or RACP
                        cmp_evt->operation = (state == PLXS_OP_INDICATE_MEAS? PLXS_SPOT_CHECK_MEAS_CMD_OP_CODE:PLXS_RACP_CMD_OP_CODE);
                    }
                    cmp_evt->status = param->status;

                    ke_msg_send(cmp_evt);

                    // go to idle state
                    ke_state_set(dest_id, PLXS_IDLE);
                }
                else if (state == PLXS_OP_INDICATE_RACP_NO_CMP )
                {
                    // go to idle state
                    ke_state_set(dest_id, PLXS_IDLE);
                }
            } break;
                // else ignore the message
            default:
            {
                ASSERT_ERR(0);
            } break;
        }
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see PLXS_MEAS_VALUE_CMD message.
 * @brief Send MEASUREMENT INDICATION/NOTIFICATION to the connected peer case if CCC enabled
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int plxs_meas_value_cmd_handler(ke_msg_id_t const msgid,
                                          struct plxs_meas_value_cmd *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    // Message status
    int msg_status = KE_MSG_CONSUMED;
    // State
    uint8_t state = ke_state_get(dest_id);
    // Connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint16_t length;
    uint8_t status = ATT_ERR_NO_ERROR;

    // Get the address of the environment
    struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);
    uint16_t handle; // = plxs_env->shdl + PLXS_IDX_PRU_ALERT_VAL;

     // check that task is in idle state
    if(state == PLXS_IDLE)
    {
        // Allocate the GATT notification message
        struct gattc_send_evt_cmd *ind;
        // data pointer
        uint8_t * ptr;

        switch (param->operation)
        {
            case PLXS_CONTINUOUS_MEAS_CMD_OP_CODE:
            {
                if ((plxs_env->prfl_ind_cfg_cont[conidx] & PLXP_CCC_NTF_ENABLED) && (plxs_env->type_of_operation != PLXS_OPTYPE_SPOT_CHECK_ONLY))
                {
                    handle = plxs_att2handle(PLXS_IDX_CONT_MEASUREMENT_VAL);
                    
                    length = 5;// Flags-1oct SpO2PR-Normal-4oct
                    // check the supported_features vs Flags settings
                    // mask off unsupported features
                    if ((plxs_env->supported_features & PLXP_FEATURE_SPO2PR_FAST_SUPPORTED) != PLXP_FEATURE_SPO2PR_FAST_SUPPORTED)
                    {
                        param->value.cont_meas.cont_flags &= ~PLXP_CONT_MEAS_FLAGS_SPO2PR_FAST_PRESENT;
                    }
                    if ((plxs_env->supported_features & PLXP_FEATURE_SPO2PR_SLOW_SUPPORTED) != PLXP_FEATURE_SPO2PR_SLOW_SUPPORTED)
                    {
                        param->value.cont_meas.cont_flags &= ~PLXP_CONT_MEAS_FLAGS_SPO2PR_SLOW_PRESENT;
                    }
                    if ((plxs_env->supported_features & PLXP_FEATURE_MEAS_STATUS_PRESENT) != PLXP_FEATURE_MEAS_STATUS_PRESENT)
                    {
                        param->value.cont_meas.cont_flags &= ~PLXP_CONT_MEAS_FLAGS_MEAS_STATUS_PRESENT;
                    }
                    if ((plxs_env->supported_features & PLXP_FEATURE_DEVICE_SENSOR_STATUS_PRESENT) != PLXP_FEATURE_DEVICE_SENSOR_STATUS_PRESENT)
                    {
                        param->value.cont_meas.cont_flags &= ~PLXP_CONT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT;
                    }
                    if ((plxs_env->supported_features & PLXP_FEATURE_PULSE_AMPLITUDE_SUPPORTED) != PLXP_FEATURE_PULSE_AMPLITUDE_SUPPORTED)
                    {
                        param->value.cont_meas.cont_flags &= ~PLXP_CONT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT;
                    }
                    // allow only valid flags
                    param->value.cont_meas.cont_flags &= PLXP_CONT_MEAS_FLAGS_VALID_MASK;
                    
                    // calculate size of data record
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_SPO2PR_FAST_PRESENT)
                    {
                        length += 4; // SpO2PR-Fast-4oct
                    }
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_SPO2PR_SLOW_PRESENT)
                    {
                        length += 4; // SpO2PR-Slow-4oct
                    }
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_MEAS_STATUS_PRESENT)
                    {
                        length += 2; // Measurement Status
                    }
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT)
                    {
                        length += 3; // Device & Sensor Status
                    }
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT)
                    {
                        length += 2; // Pulse Amplitude Index
                    }

                    // Allocate the GATT notification message
                    ind = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                            KE_BUILD_ID(TASK_GATTC, conidx),
                            dest_id,
                            gattc_send_evt_cmd, length);

                    // Fill in the parameter structure
                    ind->operation = GATTC_NOTIFY;
                    ind->handle = handle;
                    ind->length = length;
                    // fill data
                    ind->value[0] = param->value.cont_meas.cont_flags;
                    ptr = &(ind->value[1]);
                    co_write16p(ptr,param->value.cont_meas.normal.sp_o2);
                    ptr +=2;
                    co_write16p(ptr,param->value.cont_meas.normal.pr);
                    ptr +=2;
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_SPO2PR_FAST_PRESENT)
                    {
                        // SpO2PR-Fast-4oct
                        co_write16p(ptr,param->value.cont_meas.fast.sp_o2);
                        ptr +=2;
                        co_write16p(ptr,param->value.cont_meas.fast.pr);
                        ptr +=2;
                    }
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_SPO2PR_SLOW_PRESENT)
                    {
                        // SpO2PR-Slow-4oct
                        co_write16p(ptr,param->value.cont_meas.slow.sp_o2);
                        ptr +=2;
                        co_write16p(ptr,param->value.cont_meas.slow.pr);
                        ptr +=2;
                    }
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_MEAS_STATUS_PRESENT)
                    {
                        // Measurement Status
                        co_write16p(ptr,param->value.cont_meas.meas_status);
                        ptr +=2;
                    }
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT)
                    {
                        // Device & Sensor Status
                        co_write24p(ptr,param->value.cont_meas.device_sensor_status);
                        ptr +=3;
                    }
                    if (param->value.cont_meas.cont_flags & PLXP_CONT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT)
                    {
                        // Pulse Amplitude Index
                        co_write16p(ptr,param->value.cont_meas.pulse_amplitude);
                        ptr +=2;
                    }

                    // send notification to peer device
                    ke_msg_send(ind);

                    // go to busy state
                    ke_state_set(dest_id, PLXS_OP_NOTIFY);
                }
                else
                {
                    status = PRF_ERR_IND_DISABLED;
                }
            } break;
            
            case PLXS_SPOT_CHECK_MEAS_CMD_OP_CODE:
            {
                if ((plxs_env->prfl_ind_cfg_spot[conidx] & PLXP_CCC_IND_ENABLED) && (plxs_env->type_of_operation != PLXS_OPTYPE_CONTINUOUS_ONLY))
                {
                    handle = plxs_att2handle(PLXS_IDX_SPOT_MEASUREMENT_VAL);
                    length = 5; // Flags-1oct SpO2PR-Spot-Check-4oct
                    // check the supported_features vs Flags settings
                    // mask off unsupported features
                    if ((plxs_env->supported_features & PLXP_FEATURE_TIMESTAMP_SUPPORTED) != PLXP_FEATURE_TIMESTAMP_SUPPORTED)
                    {
                        param->value.spot_meas.spot_flags &= ~PLXP_SPOT_MEAS_FLAGS_TIMESTAMP_PRESENT;
                    }
                    if ((plxs_env->supported_features & PLXP_FEATURE_MEAS_STATUS_PRESENT) != PLXP_FEATURE_MEAS_STATUS_PRESENT)
                    {
                        param->value.spot_meas.spot_flags &= ~PLXP_SPOT_MEAS_FLAGS_MEAS_STATUS_PRESENT;
                    }
                    if ((plxs_env->supported_features & PLXP_FEATURE_DEVICE_SENSOR_STATUS_PRESENT) != PLXP_FEATURE_DEVICE_SENSOR_STATUS_PRESENT)
                    {
                        param->value.spot_meas.spot_flags &= ~PLXP_SPOT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT;
                    }
                    if ((plxs_env->supported_features & PLXP_FEATURE_PULSE_AMPLITUDE_SUPPORTED) != PLXP_FEATURE_PULSE_AMPLITUDE_SUPPORTED)
                    {
                        param->value.spot_meas.spot_flags &= ~PLXP_SPOT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT;
                    }
                    // allow only valid flags
                    param->value.spot_meas.spot_flags &= PLXP_SPOT_MEAS_FLAGS_VALID_MASK;
                    
                    
                    // calculate size of data record
                    if (param->value.spot_meas.spot_flags & PLXP_SPOT_MEAS_FLAGS_TIMESTAMP_PRESENT)
                    {
                        length += 7; // Timestamp
                    }
                    if (param->value.spot_meas.spot_flags & PLXP_SPOT_MEAS_FLAGS_MEAS_STATUS_PRESENT)
                    {
                        length += 2; // Measurement Status
                    }
                    if (param->value.spot_meas.spot_flags & PLXP_SPOT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT)
                    {
                        length += 3; // Device & Sensor Status
                    }
                    if (param->value.spot_meas.spot_flags & PLXP_SPOT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT)
                    {
                        length += 2; // Pulse Amplitude Index
                    }

                    // Allocate the GATT notification message
                    ind = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                            KE_BUILD_ID(TASK_GATTC, conidx),
                            dest_id,
                            gattc_send_evt_cmd, length);

                    // Fill in the parameter structure
                    ind->operation = GATTC_INDICATE;
                    ind->handle = handle;
                    ind->length = length;
                    // fill data
                    ind->value[0] = param->value.spot_meas.spot_flags;
                    ptr = &(ind->value[1]);
                    co_write16p(ptr,param->value.spot_meas.sp_o2);
                    ptr +=2;
                    co_write16p(ptr,param->value.spot_meas.pr);
                    ptr +=2;
                    if (param->value.spot_meas.spot_flags & PLXP_SPOT_MEAS_FLAGS_TIMESTAMP_PRESENT)
                    {
                        // Timestamp-7oct
                        ptr +=prf_pack_date_time(ptr, &param->value.spot_meas.timestamp);
                    }
                    if (param->value.spot_meas.spot_flags & PLXP_SPOT_MEAS_FLAGS_MEAS_STATUS_PRESENT)
                    {
                        // Measurement Status
                        co_write16p(ptr,param->value.spot_meas.meas_status);
                        ptr +=2;
                    }
                    if (param->value.spot_meas.spot_flags & PLXP_SPOT_MEAS_FLAGS_DEVICE_SENSOR_STATUS_PRESENT)
                    {
                        // Device & Sensor Status
                        co_write24p(ptr,param->value.spot_meas.device_sensor_status);
                        ptr +=3;
                    }
                    if (param->value.spot_meas.spot_flags & PLXP_SPOT_MEAS_FLAGS_PULSE_AMPLITUDE_PRESENT)
                    {
                        // Pulse Amplitude Index
                        co_write16p(ptr,param->value.spot_meas.pulse_amplitude);
                        ptr +=2;
                    }

                    // send notification to peer device
                    ke_msg_send(ind);

                    // go to busy state
                    ke_state_set(dest_id, PLXS_OP_INDICATE_MEAS);
                    
                }
                else
                {
                    status = PRF_ERR_IND_DISABLED;
                }
            } break;
            
            default:
            {
                status = PRF_ERR_IND_DISABLED;
            } break;
        }
        
        // in error case immediately send response to HL
        if (status != ATT_ERR_NO_ERROR)
        {
            /// Send a command complete to the App indicate msg could not be sent.
            struct plxs_cmp_evt *cmp_evt = KE_MSG_ALLOC(PLXS_CMP_EVT,
                    src_id,
                    dest_id, plxs_cmp_evt);

            cmp_evt->operation = param->operation;
            cmp_evt->status = status;

            ke_msg_send(cmp_evt);
        }
    }
    // else process it later
    else
    {
        msg_status = KE_MSG_SAVED;
    }
    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @see PLXS_RACP_RESP_CMD message.
 * @brief Send MEASUREMENT INDICATION to the connected peer case if CCC enabled
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int plxs_racp_resp_cmd_handler(ke_msg_id_t const msgid,
                                          struct plxs_racp_resp_cmd *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    // Message status
    int msg_status = KE_MSG_CONSUMED;
    // State
    uint8_t state = ke_state_get(dest_id);
    // Connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = ATT_ERR_NO_ERROR;

    // Get the address of the environment
    struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);

     // check that task is in idle state
    if(state == PLXS_IDLE)
    {
        switch (param->operation)
        {
            
            case PLXS_RACP_CMD_OP_CODE:
            {
                if ((plxs_env->prfl_ind_cfg_racp[conidx] & PLXP_CCC_IND_ENABLED) && (plxs_env->type_of_operation != PLXS_OPTYPE_CONTINUOUS_ONLY))
                {
                    // procedure complete
                    plxs_env->racp_in_progress[conidx] = 0;
                    plxs_racp_resp_send(
                                        KE_BUILD_ID(TASK_GATTC, conidx), 
                                        dest_id, 
                                        param->cp_opcode, 
                                        param->request_cp_opcode, 
                                        param->response_code, 
                                        param->number_of_records);

                    // go to busy state
                    ke_state_set(dest_id, PLXS_OP_INDICATE_RACP);
                }
                else
                {
                    status = PRF_ERR_IND_DISABLED;
                }
            } break;
            
            default:
            {
                status = PRF_ERR_IND_DISABLED;
            } break;
        }
        
        // in error case immediately send response to HL
        if (status != ATT_ERR_NO_ERROR)
        {
            /// Send a command complete to the App indicate msg could not be sent.
            struct plxs_cmp_evt *cmp_evt = KE_MSG_ALLOC(PLXS_CMP_EVT,
                    src_id,
                    dest_id, plxs_cmp_evt);

            cmp_evt->operation = param->operation;
            cmp_evt->status = status;

            ke_msg_send(cmp_evt);
        }

    }
    // else process it later
    else
    {
        msg_status = KE_MSG_SAVED;
    }
    return (msg_status);
}



/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Specifies the default message handlers
KE_MSG_HANDLER_TAB(plxs)
{
    {PLXS_ENABLE_REQ,          (ke_msg_func_t) plxs_enable_req_handler},
    /// Send the Spot-check Measurement or Measurement Record
    /// Send Continuous Measurement
    {PLXS_MEAS_VALUE_CMD,      (ke_msg_func_t) plxs_meas_value_cmd_handler},
    /// Send Control Point Response
    {PLXS_RACP_RESP_CMD,       (ke_msg_func_t) plxs_racp_resp_cmd_handler},
  
    {GATTC_READ_REQ_IND,       (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_ATT_INFO_REQ_IND,   (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,      (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_CMP_EVT,            (ke_msg_func_t) gattc_cmp_evt_handler},
};

void plxs_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct plxs_env_tag *plxs_env = PRF_ENV_GET(PLXS, plxs);

    task_desc->msg_handler_tab = plxs_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(plxs_msg_handler_tab);
    task_desc->state           = plxs_env->state;
    task_desc->idx_max         = PLXS_IDX_MAX;
}

#endif //(BLE_PLX_SERVER)

/// @} PLXSTASK
