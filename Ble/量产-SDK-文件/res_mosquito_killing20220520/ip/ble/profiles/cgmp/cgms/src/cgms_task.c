/**
 ****************************************************************************************
 *
 * @file cgms_task.c
 *
 * @brief Continuous Glucose Monitoring Profile Service Task Implementation.
 *
 * Copyright (C) Radiawave 2017-2021
 *
 * $ Rev $
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup CGMSTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_CGM_SERVER)
#include "cgmp_common.h"

#include "gapc.h"
#include "gattc_task.h"
#include "attm.h"
#include "cgms.h"
#include "cgms_task.h"
#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"
/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
__STATIC void cgms_racp_resp_send(ke_task_id_t const dest_id, ke_task_id_t const src_id, uint8_t cp_opcode, uint8_t request_cp_opcode, uint8_t response_code, uint16_t number_of_records);


// 
// if cp_opcode == CGMP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP
// then sends the number of stored records response using number_of_records
/**
 ****************************************************************************************
 * @brief SEND RACP response indication.
 * @brief  if cp_opcode == CGMP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP
 * @brief  then sends the number of stored records response using number_of_records
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance.
 * @param[in] cp_opcode Sent in the header of response- Mandatory
 * @param[in] request_cp_opcode Copy the requested op_code
 * @param[in] response_code Result code
 * @param[in] number_of_records Used if cp_opcode == CGMP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP
 ****************************************************************************************
 */
__STATIC void cgms_racp_resp_send(ke_task_id_t const dest_id, ke_task_id_t const src_id, uint8_t cp_opcode, uint8_t request_cp_opcode, uint8_t response_code, uint16_t number_of_records)
{
    // Allocate the GATT notification message
    struct gattc_send_evt_cmd *ind;
    // length of the data block
    uint16_t length;
    // handle to send the indication
    uint16_t handle;

    handle = cgms_att2handle(CGMS_IDX_RACP_VAL);
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
    ind->value[1] = CGMP_OP_NULL;
    if (cp_opcode == CGMP_OPCODE_NUMBER_OF_STORED_RECORDS_RESP)
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
 * @brief Handles reception of the @see CGMS_ENABLE_REQ message.
 * @param[in] msgid Id of the message received
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int cgms_enable_req_handler(ke_msg_id_t const msgid,
                                    struct cgms_enable_req *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);
    // Status
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    uint8_t state = ke_state_get(dest_id);
    // Allocate Response message
    struct cgms_enable_rsp *cmp_evt = KE_MSG_ALLOC(CGMS_ENABLE_RSP, src_id, dest_id, cgms_enable_rsp);

    if (state == CGMS_IDLE)
    { 
        uint8_t conidx = KE_IDX_GET(src_id);
        /// CCCs for the current connection
        cgms_env->prfl_state[conidx] = 0 
            | (param->prfl_ind_cfg_meas ?       CGMS_NTF_MEAS_ENABLED_BIT:0) 
            | (param->prfl_ind_cfg_racp ?       CGMS_IND_RACP_ENABLED_BIT:0) 
            | (param->prfl_ind_cfg_ops_ctl_pt ? CGMS_IND_SOPS_ENABLED_BIT:0);
        status = GAP_ERR_NO_ERROR;
    }
    
    // send completed information to APP task that contains error status
    cmp_evt->status = status;

    ke_msg_send(cmp_evt);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @see CGMS_RD_CHAR_CFM message.
 * @param[in] msgid Id of the message received
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int cgms_rd_char_cfm_handler(ke_msg_id_t const msgid,
                                    struct cgms_rd_char_cfm *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // State
    uint8_t state = ke_state_get(dest_id);

    if (state == CGMS_OP_READ)
    {
        struct gattc_read_cfm *cfm;
        // Get the address of the environment
        struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);
        uint8_t conidx = KE_IDX_GET(dest_id);
        // determine handle
        uint16_t handle = 0;
        // message size
        uint16_t length = 0;

        
        // add E2E-CRC
        if (cgms_env->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
        {
            length += CGMP_CRC_LEN; // E2E-CRC size
        }

        switch (param->char_type)
        {
            case CGMS_CHAR_ID_STATUS_VAL:
            {
                handle = cgms_env->shdl + CGMS_IDX_STATUS_VAL;
                length += CGMS_RD_STATUS_CFM_SIZE;
            } break;
            case CGMS_CHAR_ID_SESSION_START_TIME_VAL:
            {
                handle = cgms_env->shdl + CGMS_IDX_SESSION_START_TIME_VAL;
                length += CGMS_RD_SESS_START_TIME_CFM_SIZE;
            
            } break;
            case CGMS_CHAR_ID_SESSION_RUN_TIME_VAL:
            {
                handle = cgms_env->shdl + CGMS_IDX_SESSION_RUN_TIME_VAL;
                length += CGMS_RD_SESS_RUN_TIME_CFM_SIZE;
            } break;
            default:
              break;
        }

        // Send data to peer device
        cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM,
                KE_BUILD_ID(TASK_GATTC, conidx), dest_id,
                gattc_read_cfm, length);
    
        cfm->handle = handle;
        cfm->length = length;
        cfm->status = param->status;

        // Fill data per characteristic type
        switch (param->char_type)
        {
            case CGMS_CHAR_ID_STATUS_VAL:
            {
                co_write16p(&cfm->value[0], param->value.status.time_offset);
                cfm->value[2] = param->value.status.warning;
                cfm->value[3] = param->value.status.cal_temp;
                cfm->value[4] = param->value.status.annunc_status;
            } break;
            case CGMS_CHAR_ID_SESSION_START_TIME_VAL:
            {
                prf_pack_date_time(&cfm->value[0], &(param->value.sess_start_time.session_start_time));
                
                cfm->value[7] = param->value.sess_start_time.time_zone;
                cfm->value[8] = param->value.sess_start_time.dst_offset;
            } break;
            case CGMS_CHAR_ID_SESSION_RUN_TIME_VAL:
            {
                co_write16p(&cfm->value[0], param->value.sess_run_time.run_time);
            } break;
            default:
              break;
        }
        
        // add E2E-CRC
        if (cgms_env->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
        {
            uint16_t crc = prf_e2e_crc(&cfm->value[0],length-CGMP_CRC_LEN);
            co_write16p(&cfm->value[length-CGMP_CRC_LEN], crc);
        }
        
        ke_state_set(dest_id, CGMS_IDLE);
        // send message
        ke_msg_send(cfm);
    }
    else
    {
        // error condition
        /// Send a command complete to the App  with error reason
        struct cgms_cmp_evt *cmp_evt = KE_MSG_ALLOC(CGMS_CMP_EVT,
                src_id,
                dest_id, cgms_cmp_evt);

        // Read characteristic confirmation is not allowed
        cmp_evt->operation = CGMS_RD_CHAR_OP_CODE;
        cmp_evt->status = PRF_ERR_REQ_DISALLOWED;

        ke_msg_send(cmp_evt);
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see CGMS_WR_OPS_CTRL_PT_CFM message.
 * @param[in] msgid Id of the message received
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
 
 
 // send indication
__STATIC int cgms_wr_ops_ctrl_pt_cfm_handler(ke_msg_id_t const msgid,
                                    struct cgms_wr_ops_ctrl_pt_cfm *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // State
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = ATT_ERR_NO_ERROR;

    // Get the address of the environment
    struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);
    uint8_t conidx = KE_IDX_GET(dest_id);

    if (state != CGMS_OP_WRITE_OPS_CTRL_PT)
    {
        // error condition
        status = PRF_ERR_REQ_DISALLOWED;
    }

    // Only send if CCC is properly configured
    if ((status == ATT_ERR_NO_ERROR) && CMPF(cgms_env->prfl_state[conidx], CGMS_IND_SOPS_ENABLED_BIT))
    {
        // handle to send the indication
        uint16_t handle = cgms_env->shdl + CGMS_IDX_SPECIFIC_OPS_CTRL_PT_VAL;
        // length of the data block
        uint16_t length = 2 * sizeof(uint8_t); // minimum Op-Code & Response codes present
        // Allocate the GATT notification message
        struct gattc_send_evt_cmd *ind;

        switch (param->opcode)
        {
            case CGMP_OPS_CODE_SET_CGM_COMMUNICATION_INTERVAL://     Communication interval in minutes
            case CGMP_OPS_CODE_CGM_COMMUNICATION_INTERVAL_RESPONSE://     Communication interval in minutes
            case CGMP_OPS_CODE_GET_GLUCOSE_CALIBRATION_VALUE://     Calibration Data Record Number 
            case CGMP_OPS_CODE_SET_PATIENT_HIGH_ALERT_LEVEL://     Patient High bG value in mg/dL 
            case CGMP_OPS_CODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE://     Patient High bG value in mg/dL 
            case CGMP_OPS_CODE_SET_PATIENT_LOW_ALERT_LEVEL://     Patient Low bG value in mg/dL 
            case CGMP_OPS_CODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE://     Patient Low bG value in mg/dL 
            case CGMP_OPS_CODE_SET_HYPO_ALERT_LEVEL://     Hypo Alert Level value in mg/dL 
            case CGMP_OPS_CODE_HYPO_ALERT_LEVEL_RESPONSE://     Hypo Alert Level value in mg/dL 
            case CGMP_OPS_CODE_SET_HYPER_ALERT_LEVEL://     Hyper Alert Level value in mg/dL
            case CGMP_OPS_CODE_HYPER_ALERT_LEVEL_RESPONSE://     Hyper Alert Level value in mg/dL 
            case CGMP_OPS_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL://     Rate of Decrease Alert Level value in mg/dL/min 
            case CGMP_OPS_CODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE://     Rate of Decrease Alert Level value in mg/dL/min 
            case CGMP_OPS_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL://     Rate of Increase Alert Level value in mg/dL/min 
            case CGMP_OPS_CODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE://     Rate of Increase Alert Level value in mg/dL/min 
            case CGMP_OPS_CODE_RESPONSE_CODE://     Request Op Code, Response Code Value
            {
                length += 2; //sizeof(uint16_t)
            }break;
            case CGMP_OPS_CODE_SET_GLUCOSE_CALIBRATION_VALUE://     Operand value as defined in the Calibration Value Fields. 
            case CGMP_OPS_CODE_GLUCOSE_CALIBRATION_VALUE_RESPONSE://     Calibration Data .
            {
                length += CGMP_OPS_CALIBRATION_SIZE;
            }break;
            default: break; //NA
        }

        // add E2E-CRC
        if (cgms_env->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
        {
            length += CGMP_CRC_LEN; // E2E-CRC size
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
        // Fill data
        // Op Code
        ind->value[0] = param->opcode;
        // Response
        ind->value[1] = param->response;

        // operand based on op code
        switch (param->opcode)
        {
        case CGMP_OPS_CODE_SET_CGM_COMMUNICATION_INTERVAL://     Communication interval in minutes
        case CGMP_OPS_CODE_CGM_COMMUNICATION_INTERVAL_RESPONSE://     Communication interval in minutes
            co_write16p(&(ind->value[2]), param->operand.interval);
            break;
        case CGMP_OPS_CODE_GET_GLUCOSE_CALIBRATION_VALUE://     Calibration Data Record Number 
            co_write16p(&(ind->value[2]), param->operand.cal_data_record_number);
            break;
        case CGMP_OPS_CODE_SET_PATIENT_HIGH_ALERT_LEVEL://     Patient High bG value in mg/dL 
        case CGMP_OPS_CODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE://     Patient High bG value in mg/dL 
            co_write16p(&(ind->value[2]), param->operand.patient_high_bg_value);
            break;
        case CGMP_OPS_CODE_SET_PATIENT_LOW_ALERT_LEVEL://     Patient Low bG value in mg/dL 
        case CGMP_OPS_CODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE://     Patient Low bG value in mg/dL 
            co_write16p(&(ind->value[2]), param->operand.patient_low_bg_value);
            break;
        case CGMP_OPS_CODE_SET_HYPO_ALERT_LEVEL://     Hypo Alert Level value in mg/dL 
        case CGMP_OPS_CODE_HYPO_ALERT_LEVEL_RESPONSE://     Hypo Alert Level value in mg/dL 
            co_write16p(&(ind->value[2]), param->operand.hypo_alert_level_value);
            break;
        case CGMP_OPS_CODE_SET_HYPER_ALERT_LEVEL://     Hyper Alert Level value in mg/dL
        case CGMP_OPS_CODE_HYPER_ALERT_LEVEL_RESPONSE://     Hyper Alert Level value in mg/dL 
            co_write16p(&(ind->value[2]), param->operand.hyper_alert_level_value);
            break;
        case CGMP_OPS_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL://     Rate of Decrease Alert Level value in mg/dL/min 
        case CGMP_OPS_CODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE://     Rate of Decrease Alert Level value in mg/dL/min 
            co_write16p(&(ind->value[2]), param->operand.rate_of_decrease_alert_level_value);
            break;
        case CGMP_OPS_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL://     Rate of Increase Alert Level value in mg/dL/min 
        case CGMP_OPS_CODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE://     Rate of Increase Alert Level value in mg/dL/min 
            co_write16p(&(ind->value[2]), param->operand.rate_of_increase_alert_level_value);
            break;
        case CGMP_OPS_CODE_RESPONSE_CODE://     Request Op Code, Response Code Value
            ind->value[2] = param->operand.response_code.request_op_code;
            ind->value[3] = param->operand.response_code.response_code_value;
            break;
        case CGMP_OPS_CODE_SET_GLUCOSE_CALIBRATION_VALUE://     Operand value as defined in the Calibration Value Fields. 
        case CGMP_OPS_CODE_GLUCOSE_CALIBRATION_VALUE_RESPONSE://     Calibration Data .  ???
            co_write16p(&(ind->value[2]), param->operand.calibration.cal_glucose_concentration);
            co_write16p(&(ind->value[4]), param->operand.calibration.cal_time);
            ind->value[6] = param->operand.calibration.cal_type_sample;
            ind->value[7] = param->operand.calibration.cal_sample_location;
            co_write16p(&(ind->value[8]), param->operand.calibration.next_cal_time);
            co_write16p(&(ind->value[10]), param->operand.calibration.cal_record_number);
            ind->value[12] = param->operand.calibration.cal_status;
            break;
        default:
            //NA
            break;
        }
        
        // add E2E-CRC
        if (cgms_env->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
        {
            uint16_t crc = prf_e2e_crc(&ind->value[0], length-CGMP_CRC_LEN);
            co_write16p(&ind->value[length-CGMP_CRC_LEN], crc);
        }
        
        ke_state_set(dest_id, CGMS_OP_INDICATE_OPS_CTRL_PT);
        // send notification to peer device
        ke_msg_send(ind);
    }
    else
    {
        status = PRF_ERR_IND_DISABLED;
    }


    // in error case immediately send response to HL
    if (status != ATT_ERR_NO_ERROR)
    {
        /// Send a command complete to the App  with error reason why Indicate msg could not be sent.
        struct cgms_cmp_evt *cmp_evt = KE_MSG_ALLOC(CGMS_CMP_EVT,
                src_id,
                dest_id, cgms_cmp_evt);

        // Indication is Special OPS Control Point
        cmp_evt->operation = CGMS_OPS_CTRL_PT_OP_CODE;
        cmp_evt->status = status;

        ke_msg_send(cmp_evt);
    }

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
__STATIC int gattc_read_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_read_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_CONSUMED;
    uint8_t state = ke_state_get(dest_id);
    // check that task is in idle state
    if(state == CGMS_IDLE)
    {
        // Get the address of the environment
        struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);
        uint8_t conidx = KE_IDX_GET(src_id);
        uint8_t att_idx = cgms_handle2att(param->handle);

        // Send data to peer device
        struct gattc_read_cfm* cfm = NULL;

        uint8_t status = ATT_ERR_NO_ERROR;

        switch(att_idx)
        {
            case CGMS_IDX_MEASUREMENT_CCC:
            case CGMS_IDX_SPECIFIC_OPS_CTRL_PT_CCC:
            case CGMS_IDX_RACP_CCC:
            {

                // Fill data
                cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(uint16_t));
                cfm->length = sizeof(uint16_t);
                switch(att_idx)
                {
                    uint16_t ccc;
                    case CGMS_IDX_MEASUREMENT_CCC:
                    {/// CCC for Measurement 
                        ccc = (CMPF(cgms_env->prfl_state[conidx], CGMS_NTF_MEAS_ENABLED_BIT)? CGMP_CCC_NTF_ENABLED: 0);
                        co_write16p(cfm->value, ccc);
                    } break;
                    case CGMS_IDX_SPECIFIC_OPS_CTRL_PT_CCC:
                    {/// CCC for Specific OPS Ctrl Pt 
                        ccc = (CMPF(cgms_env->prfl_state[conidx], CGMS_IND_SOPS_ENABLED_BIT)? CGMP_CCC_IND_ENABLED: 0);
                        co_write16p(cfm->value, ccc);
                    } break;
                    case CGMS_IDX_RACP_CCC:
                    {/// CCC for RACP 
                        ccc = (CMPF(cgms_env->prfl_state[conidx], CGMS_IND_RACP_ENABLED_BIT)? CGMP_CCC_IND_ENABLED: 0);
                        co_write16p(cfm->value, ccc);
                    } break;

                    default:
                    break;
                }
            } break;

            case CGMS_IDX_STATUS_VAL:
            case CGMS_IDX_SESSION_START_TIME_VAL:
            case CGMS_IDX_SESSION_RUN_TIME_VAL:
            { // need request APP
                struct cgms_rd_char_req_ind *ind = KE_MSG_ALLOC(CGMS_RD_CHAR_REQ_IND,
                        prf_dst_task_get(&(cgms_env->prf_env), conidx),
                        dest_id,
                        cgms_rd_char_req_ind);

                switch(att_idx)
                {
                    case CGMS_IDX_STATUS_VAL:
                    {
                        /// Characteristic id @ref cgms_char_type
                        ind->char_type = CGMS_CHAR_ID_STATUS_VAL;
                    } break;
                    case CGMS_IDX_SESSION_START_TIME_VAL:
                    {
                        /// Characteristic id @ref cgms_char_type
                        ind->char_type = CGMS_CHAR_ID_SESSION_START_TIME_VAL;
                    } break;
                    case CGMS_IDX_SESSION_RUN_TIME_VAL:
                    {
                        /// Characteristic id @ref cgms_char_type
                        ind->char_type = CGMS_CHAR_ID_SESSION_RUN_TIME_VAL;
                    } break;
                    default:
                    {
                    } break;
                }

                // Send the request indication to the application
                ke_msg_send(ind);

                // go to busy state
                ke_state_set(dest_id, CGMS_OP_READ);
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
__STATIC int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gattc_att_info_req_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Get the address of the environment
    uint8_t att_idx = cgms_handle2att(param->handle);
    struct gattc_att_info_cfm * cfm;

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;
    cfm->status = GAP_ERR_NO_ERROR;

    switch(att_idx)
    {
        case CGMS_IDX_MEASUREMENT_CCC:
        case CGMS_IDX_SPECIFIC_OPS_CTRL_PT_CCC:
        case CGMS_IDX_RACP_CCC:
        {
            // check if it's a client configuration char
            // CCC attribute length = 2
            cfm->length = sizeof(uint16_t);
        }break;

        case CGMS_IDX_RACP_VAL:
        {
            // control point 
            cfm->length = CGMS_IDX_RACP_SIZE_MAX;
        }break;

        case CGMS_IDX_SESSION_START_TIME_VAL:
        {
            // control point 
            cfm->length = CGMP_IDX_SESSION_START_TIME_SIZE_MAX;
        }break;

        case CGMS_IDX_SPECIFIC_OPS_CTRL_PT_VAL:
        {
            // control point 
            cfm->length = CGMS_IDX_OPS_CTRL_PT_SIZE_MAX;
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
__STATIC int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                       struct gattc_write_req_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);
    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;

    // Check the connection handle
    if (cgms_env != NULL)
    {
        uint8_t state = ke_state_get(dest_id);
        // check that task is in idle state
        if(state == CGMS_IDLE)
        {
            uint8_t conidx = KE_IDX_GET(src_id);
            // Status
            uint8_t status = GAP_ERR_NO_ERROR;
            
            uint8_t att_idx = cgms_handle2att(param->handle);

            switch(att_idx)
            {
                case CGMS_IDX_MEASUREMENT_CCC:
                case CGMS_IDX_SPECIFIC_OPS_CTRL_PT_CCC:
                case CGMS_IDX_RACP_CCC:
                {
                    uint16_t ntf_cfg;
                    // Inform the HL about the new configuration
                    struct cgms_wr_char_ccc_ind *ind = KE_MSG_ALLOC(CGMS_WR_CHAR_CCC_IND,
                            prf_dst_task_get(&(cgms_env->prf_env), conidx),
                            dest_id,
                            cgms_wr_char_ccc_ind);
                    // Get the value
                    co_write16p(&ntf_cfg, param->value[0]);

                    switch(att_idx)
                    {
                        case CGMS_IDX_MEASUREMENT_CCC:
                        {/// CCC for Measurement 
                            ind->char_type = CGMS_CHAR_ID_MEAS_CCC;
                            if (CMPF(ntf_cfg, CGMP_CCC_NTF_ENABLED))
                            {
                                cgms_env->prfl_state[conidx] |= CGMS_NTF_MEAS_ENABLED_BIT;
                            }
                        } break;
                        case CGMS_IDX_SPECIFIC_OPS_CTRL_PT_CCC:
                        {/// CCC for Specific OPS Ctrl Pt 
                            ind->char_type = CGMS_CHAR_ID_OPS_CTRL_PT_CCC;
                            if (CMPF(ntf_cfg, CGMP_CCC_IND_ENABLED))
                            {
                                cgms_env->prfl_state[conidx] |= CGMS_IND_SOPS_ENABLED_BIT;
                            }
                        } break;
                        case CGMS_IDX_RACP_CCC:
                        {/// CCC for RACP 
                            ind->char_type = CGMS_CHAR_ID_RACP_CCC;
                            if (CMPF(ntf_cfg, CGMP_CCC_IND_ENABLED))
                            {
                                cgms_env->prfl_state[conidx] |= CGMS_IND_RACP_ENABLED_BIT;
                            }
                        } break;
                        default: break;
                    }

                    ind->ind_cfg = ntf_cfg;

                    ke_msg_send(ind);
                }break;

                case CGMS_IDX_SESSION_START_TIME_VAL:
                {
                    if (cgms_env->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
                    {
                        if (param->length < CGMP_IDX_SESSION_START_TIME_SIZE_MAX)
                        {
                            status = CGM_ERROR_MISSING_CRC;
                        }
                        else
                        {
                            uint16_t crc = prf_e2e_crc(&param->value[0], CGMP_IDX_SESSION_START_TIME_SIZE_MAX - CGMP_CRC_LEN);
                            // check crc
                            if (crc != co_read16p(&(param->value[9])))
                            {
                                status = CGM_ERROR_INVALID_CRC;
                            }
                        }
                    }
                    
                    if (status == GAP_ERR_NO_ERROR)
                    {
                        int8_t  time_zone  = param->value[7];
                        uint8_t dst_offset = param->value[8];
                        if (!( (time_zone == CGM_TIME_ZONE_UNKNOWN) 
                            || ((time_zone >= CGM_TIME_ZONE_MIN) && (time_zone <= CGM_TIME_ZONE_MAX))))
                        {
                            // incorrect request
                            status = PRF_OUT_OF_RANGE;
                        }
                        if (!(  (dst_offset == CGM_DST_OFFSET_STANDARD_TIME) 
                              ||(dst_offset == CGM_DST_OFFSET_HALF_AN_HOUR_DAYLIGHT_TIME) 
                              ||(dst_offset == CGM_DST_OFFSET_DAYLIGHT_TIME) 
                              ||(dst_offset == CGM_DST_OFFSET_DOUBLE_DAYLIGHT_TIME) 
                              ||(dst_offset == CGM_DST_OFFSET_UNKNOWN_DAYLIGHT_TIME)))
                        {
                            // incorrect request
                            status = PRF_OUT_OF_RANGE;
                        }
                        if (status == GAP_ERR_NO_ERROR)
                        {
                            // Inform the HL about the new Time
                            struct cgms_wr_sess_start_time_ind *ind = KE_MSG_ALLOC(CGMS_WR_SESS_START_TIME_IND,
                                    prf_dst_task_get(&cgms_env->prf_env, conidx),
                                    prf_src_task_get(&cgms_env->prf_env, conidx),
                                    cgms_wr_sess_start_time_ind);
                            prf_unpack_date_time((uint8_t *) &param->value[0], &ind->session_start_time);
                            ind->time_zone  = time_zone;
                            ind->dst_offset = dst_offset;
                            
                            ke_msg_send(ind);
                        }
                    }
                }break;

                case CGMS_IDX_SPECIFIC_OPS_CTRL_PT_VAL:
                {
                    if (cgms_env->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
                    {
                        uint16_t length = CGMP_CP_MIN_LEN;
                        // length of the message base on the OpCode
                        switch (param->value[0])
                        {
                            case CGMP_OPS_CODE_SET_CGM_COMMUNICATION_INTERVAL:      // Communication interval in minutes
                            case CGMP_OPS_CODE_CGM_COMMUNICATION_INTERVAL_RESPONSE: // Communication interval in minutes
                            case CGMP_OPS_CODE_GET_GLUCOSE_CALIBRATION_VALUE:       // Calibration Data Record Number
                            case CGMP_OPS_CODE_SET_PATIENT_HIGH_ALERT_LEVEL:        // Patient High bG value in mg/dL
                            case CGMP_OPS_CODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE:   // Patient High bG value in mg/dL
                            case CGMP_OPS_CODE_SET_PATIENT_LOW_ALERT_LEVEL:         // Patient Low bG value in mg/dL
                            case CGMP_OPS_CODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE:    // Patient Low bG value in mg/dL
                            case CGMP_OPS_CODE_SET_HYPO_ALERT_LEVEL:                // Hypo Alert Level value in mg/dL
                            case CGMP_OPS_CODE_HYPO_ALERT_LEVEL_RESPONSE:           // Hypo Alert Level value in mg/dL
                            case CGMP_OPS_CODE_SET_HYPER_ALERT_LEVEL:               // Hyper Alert Level value in mg/dL
                            case CGMP_OPS_CODE_HYPER_ALERT_LEVEL_RESPONSE:          // Hyper Alert Level value in mg/dL
                            case CGMP_OPS_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL:     // value in mg/dL/min 
                            case CGMP_OPS_CODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE: // value in mg/dL/min 
                            case CGMP_OPS_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL:    //  value in mg/dL/min 
                            case CGMP_OPS_CODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE: // value in mg/dL/min 
                            case CGMP_OPS_CODE_RESPONSE_CODE:                       // Request Op Code, Response Code Value
                            {
                                length += 2; //sizeof(uint16_t)
                            }break;
                            case CGMP_OPS_CODE_SET_GLUCOSE_CALIBRATION_VALUE:      // defined in the Cal Value Fields
                            case CGMP_OPS_CODE_GLUCOSE_CALIBRATION_VALUE_RESPONSE: // Calibration Data
                            {
                                length += CGMP_OPS_CALIBRATION_SIZE;
                            }break;
                            default: break;//NA
                        }
                        
                        if (param->length < length)
                        {
                            status = CGM_ERROR_MISSING_CRC;
                        }
                        else
                        {
                            uint16_t crc = prf_e2e_crc(&param->value[0], param->length - CGMP_CRC_LEN);
                            // check crc
                            if (crc != co_read16p(&(param->value[length])))
                            {
                                status = CGM_ERROR_INVALID_CRC;
                            }
                        }
                    }
                    
                    if (status == GAP_ERR_NO_ERROR)
                    {
                        // Inform the HL about the new Time
                        struct cgms_wr_ops_ctrl_pt_req_ind *ind = KE_MSG_ALLOC(CGMS_WR_OPS_CTRL_PT_REQ_IND,
                                prf_dst_task_get(&cgms_env->prf_env, conidx),
                                prf_src_task_get(&cgms_env->prf_env, conidx),
                                cgms_wr_ops_ctrl_pt_req_ind);

                        // Fill data
                        memset(ind, 0, sizeof(struct cgms_wr_ops_ctrl_pt_req_ind));
                        // Op Code
                        ind->opcode = param->value[0];
                        // Response
                        ind->response = param->value[1];
                        // operand based on op code
                        switch (ind->opcode)
                        {
                            case CGMP_OPS_CODE_SET_CGM_COMMUNICATION_INTERVAL:      // Communication interval in minutes
                            case CGMP_OPS_CODE_CGM_COMMUNICATION_INTERVAL_RESPONSE: // Communication interval in minutes
                                ind->operand.interval = co_read16p(&(param->value[2]));
                                break;
                            case CGMP_OPS_CODE_GET_GLUCOSE_CALIBRATION_VALUE:      // Calibration Data Record Number
                                ind->operand.cal_data_record_number = co_read16p(&(param->value[2]));
                                break;
                            case CGMP_OPS_CODE_SET_PATIENT_HIGH_ALERT_LEVEL:       // Patient High bG value in mg/dL
                            case CGMP_OPS_CODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE:  // Patient High bG value in mg/dL
                                ind->operand.patient_high_bg_value = co_read16p(&(param->value[2]));
                                break;
                            case CGMP_OPS_CODE_SET_PATIENT_LOW_ALERT_LEVEL:        // Patient Low bG value in mg/dL
                            case CGMP_OPS_CODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE:   // Patient Low bG value in mg/dL
                                ind->operand.patient_low_bg_value = co_read16p(&(param->value[2]));
                                break;
                            case CGMP_OPS_CODE_SET_HYPO_ALERT_LEVEL:               // Hypo Alert Level value in mg/dL
                            case CGMP_OPS_CODE_HYPO_ALERT_LEVEL_RESPONSE:          // Hypo Alert Level value in mg/dL
                                ind->operand.hypo_alert_level_value = co_read16p(&(param->value[2]));
                                break;
                            case CGMP_OPS_CODE_SET_HYPER_ALERT_LEVEL:              // Hyper Alert Level value in mg/dL
                            case CGMP_OPS_CODE_HYPER_ALERT_LEVEL_RESPONSE:         // Hyper Alert Level value in mg/dL
                                ind->operand.hyper_alert_level_value = co_read16p(&(param->value[2]));
                                break;
                            case CGMP_OPS_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL:      // value in mg/dL/min 
                            case CGMP_OPS_CODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE: // value in mg/dL/min 
                                ind->operand.rate_of_decrease_alert_level_value = co_read16p(&(param->value[2]));
                                break;
                            case CGMP_OPS_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL:      // value in mg/dL/min 
                            case CGMP_OPS_CODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE: // value in mg/dL/min 
                                ind->operand.rate_of_increase_alert_level_value = co_read16p(&(param->value[2]));
                                break;
                            case CGMP_OPS_CODE_RESPONSE_CODE:                      // Request Op Code, Response Code Value
                                ind->operand.response_code.request_op_code     = param->value[2];
                                ind->operand.response_code.response_code_value = param->value[3];
                                break;
                            case CGMP_OPS_CODE_SET_GLUCOSE_CALIBRATION_VALUE:      // defined in Cal Value Fields.
                            case CGMP_OPS_CODE_GLUCOSE_CALIBRATION_VALUE_RESPONSE: // Calibration Data
                                ind->operand.calibration.cal_glucose_concentration = co_read16p(&(param->value[2]));
                                ind->operand.calibration.cal_time                  = co_read16p(&(param->value[4]));
                                ind->operand.calibration.cal_type_sample           = param->value[6];
                                ind->operand.calibration.cal_sample_location       = param->value[7];
                                ind->operand.calibration.next_cal_time             = co_read16p(&(param->value[8]));
                                ind->operand.calibration.cal_record_number         = co_read16p(&(param->value[10]));
                                ind->operand.calibration.cal_status                = param->value[12];
                                break;
                            default: break; //NA
                        }
                        ke_msg_send(ind);
                        // go to busy state
                        ke_state_set(dest_id, CGMS_OP_WRITE_OPS_CTRL_PT);
                    }

                }break;

                case CGMS_IDX_RACP_VAL:
                {
                    // Op Code
                    uint8_t opcode = param->value[0];
                    // Operator
                    uint8_t operator = param->value[1];
                    // if command to RACP and RACP is not configured for indication
                    // or Command to report Records and SPOT_MEAS CCC is not configured for indication
                    if (!CMPF(cgms_env->prfl_state[conidx], CGMS_IND_RACP_ENABLED_BIT) 
                        || (( opcode == CGMP_OPCODE_REPORT_STORED_RECORDS) 
                                && (!CMPF(cgms_env->prfl_state[conidx], CGMS_NTF_MEAS_ENABLED_BIT))
                            )
                        )
                    {// CPP improperly configured
                        status = PRF_CCCD_IMPR_CONFIGURED;
                    }
                    else if (CMPF(cgms_env->prfl_state[conidx], CGMS_RACP_IN_PROGRESS_BIT) 
                             && (opcode != CGMP_OPCODE_ABORT_OPERATION))
                    {// procedure already in progress
                        status = PRF_PROC_IN_PROGRESS;
                    }
                    // Check it is the valid request
                    else if ((( opcode >= CGMP_OPCODE_REPORT_STORED_RECORDS) 
                        && (opcode <= CGMP_OPCODE_REPORT_NUMBER_OF_STORED_RECORDS)) 
                        && (
                            ((opcode != CGMP_OPCODE_ABORT_OPERATION) 
                            && ((operator >= CGMP_OP_ALL_RECS) 
                            && (operator <= CGMP_OP_LAST_REC))) 
                            ||
                         ((opcode == CGMP_OPCODE_ABORT_OPERATION) && (operator == CGMP_OP_NULL))
                          )
                        )
                    {
                        // Inform the HL about the new Control Point Command
                        struct cgms_wr_racp_ind *ind = KE_MSG_ALLOC(CGMS_WR_RACP_IND,
                                prf_dst_task_get(&cgms_env->prf_env, conidx),
                                prf_src_task_get(&cgms_env->prf_env, conidx),
                                cgms_wr_racp_ind);
                        // Get the value
                        ind->cp_opcode = opcode;
                        ind->cp_operator = operator;
                        // Get the Filter Parameter values
                        switch (operator)
                        {
                            case CGMP_OP_LT_OR_EQ:
                            {
                                co_write16p(&ind->max_time_offset, param->value[3]);
                            }break;
                            case CGMP_OP_GT_OR_EQ:
                            {
                                co_write16p(&ind->min_time_offset, param->value[3]);
                            }break;
                            case CGMP_OP_WITHIN_RANGE_OF:
                            {
                                co_write16p(&ind->min_time_offset, param->value[3]);
                                co_write16p(&ind->max_time_offset, param->value[5]);
                            }break;
                            default:
                            { // this already checked
                            } break;
                        }

                        ke_msg_send(ind);
                        // set flag that procedure is started
                        cgms_env->prfl_state[conidx] |= CGMS_RACP_IN_PROGRESS_BIT;
                    }
                    else
                    { //error condition
                        uint8_t response_code = CGMP_RESP_INVALID_OPERATOR;
                        if (  (opcode < CGMP_OPCODE_REPORT_STORED_RECORDS) 
                            ||(opcode > CGMP_OPCODE_REPORT_NUMBER_OF_STORED_RECORDS))
                        { // incorrect opcode
                            response_code = CGMP_RESP_OP_CODE_NOT_SUPPORTED;
                        }
                        else if ((opcode != CGMP_OPCODE_ABORT_OPERATION) && (operator > CGMP_OP_LAST_REC))
                        {
                            response_code = CGMP_RESP_OPERATOR_NOT_SUPPORTED;
                        }
                        else //if ((opcode != CGMP_OPCODE_ABORT_OPERATION) && (operator == CGMP_OPERATOR_NULL))
                        {
                            response_code = CGMP_RESP_INVALID_OPERATOR;
                        }
                        // procedure complete
                        cgms_env->prfl_state[conidx] &= ~CGMS_RACP_IN_PROGRESS_BIT;
                        cgms_racp_resp_send(
                                            src_id,
                                            dest_id,
                                            CGMP_OPCODE_RESPONSE_CODE, opcode, response_code, 0);
                        // special state as we don't need response to the server
                        ke_state_set(dest_id, CGMS_OP_INDICATE_RACP_NO_CMP);

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
__STATIC int gattc_cmp_evt_handler(ke_msg_id_t const msgid,  struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    // Get the address of the environment
    struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);

    uint8_t state = ke_state_get(dest_id);

    if (cgms_env != NULL)
    {
        // Check if a connection exists
        if ((param->operation == GATTC_NOTIFY) || (param->operation == GATTC_INDICATE))
        {
            // Measurement sent or RACP sent
            // Inform the application that a procedure has been completed
            if ((state == CGMS_OP_INDICATE_RACP ) 
                || (state == CGMS_OP_NOTIFY ) 
                || (state == CGMS_OP_INDICATE_OPS_CTRL_PT ))
            {
                /// Send a command complete to the App
                struct cgms_cmp_evt *cmp_evt = KE_MSG_ALLOC(CGMS_CMP_EVT,
                        prf_dst_task_get(&cgms_env->prf_env, conidx),
                        prf_src_task_get(&cgms_env->prf_env, conidx),
                        cgms_cmp_evt);

                if (state == CGMS_OP_NOTIFY )
                { // Continuous Measurement sent
                    cmp_evt->operation = CGMS_MEAS_CMD_OP_CODE;
                }
                else if(state == CGMS_OP_INDICATE_RACP )
                { // Indication is RACP
                    cmp_evt->operation = CGMS_RACP_CMD_OP_CODE;
                }
                else if (state == CGMS_OP_INDICATE_OPS_CTRL_PT )
                { // Indication is Special OPS Control Point
                    cmp_evt->operation = CGMS_OPS_CTRL_PT_OP_CODE;
                }

                cmp_evt->status = param->status;

                ke_msg_send(cmp_evt);
            }
            else if (state != CGMS_OP_INDICATE_RACP_NO_CMP )
            {
                ASSERT_ERR(0);
            }
            // go to idle state
            ke_state_set(dest_id, CGMS_IDLE);
            // else ignore the message
        }
    }

    return (KE_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @see CGMS_MEAS_VALUE_CMD message.
 * @brief Send MEASUREMENT INDICATION/NOTIFICATION to the connected peer case if CCC enabled
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int cgms_meas_value_cmd_handler(ke_msg_id_t const msgid,
                                          struct cgms_meas_value_cmd *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    // Message status
    int msg_status = KE_MSG_CONSUMED;
    // State
    uint8_t state = ke_state_get(dest_id);

     // check that task is in idle state
    if(state == CGMS_IDLE)
    {
        // Connection index
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint16_t length;
        uint8_t status = ATT_ERR_NO_ERROR;

        // Get the address of the environment
        struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);
        uint16_t handle; // = cgms_env->shdl + CGMS_IDX_PRU_ALERT_VAL;
        // Allocate the GATT notification message
        struct gattc_send_evt_cmd *ind;
        // data pointer
        uint8_t * ptr;

        switch (param->operation)
        {
            case CGMS_MEAS_CMD_OP_CODE:
            {
                // check the CCC is enabled
                if (CMPF(cgms_env->prfl_state[conidx], CGMS_NTF_MEAS_ENABLED_BIT))
                {
                    length = CGMS_MEAS_CMD_MIN_LENGTH;
                    
                    // E2E-CRC
                    if (cgms_env->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
                    {
                        length += CGMP_CRC_LEN; // E2E-CRC size
                    }
                    // annunciation data
                    if (CMPF(param->flags, CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_WARNING_PRESENT))
                    {
                        length += 1; // sizeof(uint8_t)
                    }
                    
                    if (CMPF(param->flags, CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_CAL_TEMP_PRESENT))
                    {
                        length += 1; // sizeof(uint8_t)
                    }
                    
                    if (CMPF(param->flags, CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_STATUS_PRESENT))
                    {
                        length += 1; // sizeof(uint8_t)
                    }
                    
                    if ((CMPF(cgms_env->cgm_feature, CGM_FEATURE_CGM_TREND_INFORMATION_SUPPORTED)) 
                       && (CMPF(param->flags, CGMS_MEAS_FLAGS_CGM_TREND_INFORMATION_PRESENT)))
                    {
                        length += 2; // 2 x sizeof(uint8_t)
                    }
                    else
                    {
                        param->flags &= ~CGMS_MEAS_FLAGS_CGM_TREND_INFORMATION_PRESENT;
                    }
                    
                    if ((CMPF(cgms_env->cgm_feature, CGM_QUALITY_SUPPORTED)) 
                        && (CMPF(param->flags, CGMS_MEAS_FLAGS_CGM_QUALITY_PRESENT)))
                    {
                        length += 2; // 2 x sizeof(uint8_t)
                    }
                    else
                    {
                        param->flags &= ~CGMS_MEAS_FLAGS_CGM_QUALITY_PRESENT;
                    }
                    
                    param->flags &= CGMS_MEAS_FLAGS_VALID;
                
                    handle = cgms_att2handle(CGMS_IDX_MEASUREMENT_VAL);
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
                    //SIZE field
                    ind->value[0] = length;
                    // FLAGS field
                    ind->value[1] = param->flags;
                    // CGM Glucose Concentration (SFLOAT/16bit)
                    ptr = &(ind->value[2]);
                    co_write16p(ptr, param->glucose_concentration);
                    ptr +=2;
                    // Time Offset
                    co_write16p(ptr, param->time_offset);
                    ptr +=2;
                    
                    // annunciation data
                    if (CMPF(param->flags, CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_WARNING_PRESENT))
                    {
                        *ptr = param->warning;
                        ptr += 1;
                    }
                    if (CMPF(param->flags, CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_CAL_TEMP_PRESENT))
                    {
                        *ptr = param->cal_temp;
                        ptr += 1;
                    }
                    if (CMPF(param->flags, CGMS_MEAS_FLAGS_SENSOR_STATUS_ANNUNCIATION_FIELD_STATUS_PRESENT))
                    {
                        *ptr = param->status;
                        ptr += 1;
                    }
                    
                    // Trend data
                    if (CMPF(param->flags, CGMS_MEAS_FLAGS_CGM_TREND_INFORMATION_PRESENT))
                    {
                        co_write16p(ptr, param->trend_information);
                        ptr += 2;
                    }
                    
                    // Quality data
                    if (CMPF(param->flags, CGMS_MEAS_FLAGS_CGM_QUALITY_PRESENT))
                    {
                        co_write16p(ptr, param->quality);
                        ptr += 2;
                    }
                    
                    // E2E-CRC
                    if (cgms_env->cgm_feature & CGM_FEATURE_E2E_CRC_SUPPORTED)
                    {
                        uint16_t crc = prf_e2e_crc(&ind->value[0], length-CGMP_CRC_LEN);
                        co_write16p(ptr, crc);
                    }

                    // send notification to peer device
                    ke_msg_send(ind);

                    // go to busy state
                    ke_state_set(dest_id, CGMS_OP_NOTIFY);
                }
                else
                {
                    status = PRF_ERR_NTF_DISABLED;
                }
            } break;

            default:
            {
                status = PRF_ERR_INVALID_PARAM;
            } break;
        }
        
        // in error case immediately send response to HL
        if (status != ATT_ERR_NO_ERROR)
        {
            /// Send a command complete to the App with error status indicate msg could not be sent.
            struct cgms_cmp_evt *cmp_evt = KE_MSG_ALLOC(CGMS_CMP_EVT,
                    src_id,
                    dest_id, cgms_cmp_evt);

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
 * @brief Handles reception of the @see CGMS_RACP_RESP_CMD message.
 * @brief Send MEASUREMENT INDICATION to the connected peer case if CCC enabled
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int cgms_racp_resp_cmd_handler(ke_msg_id_t const msgid,
                                          struct cgms_racp_resp_cmd *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    // Message status
    int msg_status = KE_MSG_CONSUMED;
    // State
    uint8_t state = ke_state_get(dest_id);
     // check that task is in idle state

    if(state == CGMS_IDLE)
    {
        // Connection index
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t status = ATT_ERR_NO_ERROR;

        // Get the address of the environment
        struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);

        // do we have active procedure 
        // check the RACP procedure is ongoing
        if (CMPF(cgms_env->prfl_state[conidx], CGMS_RACP_IN_PROGRESS_BIT))
        {
            switch (param->operation)
            {
                
                case CGMS_RACP_CMD_OP_CODE:
                {
                    // check the CCC is enabled
                    if (CMPF(cgms_env->prfl_state[conidx], CGMS_IND_RACP_ENABLED_BIT))
                    {
                        // procedure complete
                        cgms_env->prfl_state[conidx] &= ~CGMS_RACP_IN_PROGRESS_BIT;
                        cgms_racp_resp_send(
                                            KE_BUILD_ID(TASK_GATTC, conidx), 
                                            dest_id, 
                                            param->cp_opcode, 
                                            param->request_cp_opcode, 
                                            param->response_code, 
                                            param->number_of_records);

                        // go to busy state
                        ke_state_set(dest_id, CGMS_OP_INDICATE_RACP);
                    }
                    else
                    {
                        status = PRF_ERR_IND_DISABLED;
                    }
                } break;
                
                default:
                {
                    status = PRF_ERR_INVALID_PARAM;
                } break;
            }
        }
        else
        {
            status = PRF_ERR_REQ_DISALLOWED;
        }
        
        // in error case immediately send response to HL
        if (status != ATT_ERR_NO_ERROR)
        {
            /// Send a command complete to the App with error indicating why msg could not be sent.
            struct cgms_cmp_evt *cmp_evt = KE_MSG_ALLOC(CGMS_CMP_EVT,
                    src_id,
                    dest_id, cgms_cmp_evt);

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
KE_MSG_HANDLER_TAB(cgms)
{
    {CGMS_ENABLE_REQ,            (ke_msg_func_t) cgms_enable_req_handler},
    /// Send the Measurement or Measurement Record
    {CGMS_MEAS_VALUE_CMD,        (ke_msg_func_t) cgms_meas_value_cmd_handler},
    /// Send Control Point Response
    {CGMS_RACP_RESP_CMD,         (ke_msg_func_t) cgms_racp_resp_cmd_handler},
    /// Read confirmation from the app with CGM Status, CGM Session Start Time, CGM Session Run Time
    {CGMS_RD_CHAR_CFM,           (ke_msg_func_t) cgms_rd_char_cfm_handler},
    /// Confirmation from OPS Control Point
    {CGMS_WR_OPS_CTRL_PT_CFM,    (ke_msg_func_t) cgms_wr_ops_ctrl_pt_cfm_handler},
  
    {GATTC_READ_REQ_IND,         (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_ATT_INFO_REQ_IND,     (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,        (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_CMP_EVT,              (ke_msg_func_t) gattc_cmp_evt_handler},
};

void cgms_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct cgms_env_tag *cgms_env = PRF_ENV_GET(CGMS, cgms);

    task_desc->msg_handler_tab = cgms_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(cgms_msg_handler_tab);
    task_desc->state           = cgms_env->state;
    task_desc->idx_max         = CGMS_IDX_MAX;
}

#endif //(BLE_CGM_SERVER)

/// @} CGMSTASK
