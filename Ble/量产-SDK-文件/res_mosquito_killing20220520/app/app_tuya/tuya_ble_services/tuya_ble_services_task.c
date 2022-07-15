/******************************************************************************
	Confidential and copyright 2019-2025 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	  tuya_ble_services_task.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2021/5/18

	Description :   
	      ÒÆÖ²µ½Í¿Ñ» SDK 

	Changed Log	:

		Liujian  2021/5/18			- Creation
*****************************************************************************/

#include "tuya_ble_services_task.h"
#include "tuya_ble_services.h"
#if(TUYA_BLE_SDK_ENABLE == 1)

#include "gap.h"
#include "gapc.h"
#include "gattc_task.h"
#include "attm.h"
#include "prf_utils.h"
#include "ke_mem.h"
#include "co_utils.h"
#include <string.h>
#include <stdio.h>


__STATIC int tuya_ble_enable_req_handler(ke_msg_id_t const msgid,
                                     struct tuya_ble_enable_req const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    // check state of the task
    if (gapc_get_conhdl(param->conidx) != GAP_INVALID_CONHDL)
    {
        // restore Bond Data
        struct tuya_ble_env_tag *tuya_ble_env = PRF_ENV_GET(TUYA_BLE, tuya_ble);
			
        tuya_ble_env->ntf_ind_cfg[param->conidx] = param->ntf_ind_cfg;
			
        status = GAP_ERR_NO_ERROR;
    }

    // send response
    struct tuya_ble_enable_rsp *rsp = KE_MSG_ALLOC(TUYA_BLE_ENABLE_RSP, src_id, dest_id, tuya_ble_enable_rsp);
		
    rsp->conidx = param->conidx;
		
    rsp->status = status;
		
    ke_msg_send(rsp);

    return (KE_MSG_CONSUMED);
}


__STATIC int tuya_ble_data_send_req_handler(ke_msg_id_t const msgid,
                                        struct tuya_ble_data_send_req const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    // Status
    int msg_status = KE_MSG_SAVED;
	
    uint8_t state = ke_state_get(dest_id);

    // check state of the task
    if (state == TUYA_BLE_IDLE)
    {
        // Get the address of the environment
        struct tuya_ble_env_tag *tuya_ble_env = PRF_ENV_GET(TUYA_BLE, tuya_ble);

        {
            // allocate operation to execute
            tuya_ble_env->operation = (struct tuya_ble_op *)ke_malloc(sizeof(struct tuya_ble_op) + sizeof(struct tuya_ble_data_send_req), KE_MEM_ATT_DB);

            // Initialize operation parameters
            tuya_ble_env->operation->cursor = 0;
            tuya_ble_env->operation->dest_id = src_id;
            tuya_ble_env->operation->conidx = GAP_INVALID_CONIDX;

            tuya_ble_env->operation->op = TUYA_BLE_CFG_STABLE_MEAS_IND;
            tuya_ble_env->operation->handle = TUYA_BLE_HANDLE(TUYA_BLE_IDX_TX_VAL);

            tuya_ble_env->operation->length = param->length;
            memcpy(&(tuya_ble_env->operation->data[0]), param->tx_data, param->length);
            // put task in a busy state
            ke_state_set(dest_id, TUYA_BLE_BUSY);

            // execute operation
            tuya_ble_exe_operation();
        }

        msg_status = KE_MSG_CONSUMED;
    }

    return (msg_status);
}

__STATIC int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
                                            struct gattc_att_info_req_ind *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
	
    struct tuya_ble_env_tag *tuya_ble_env = PRF_ENV_GET(TUYA_BLE, tuya_ble);
	
    uint8_t att_idx = TUYA_BLE_IDX(param->handle);
	
    struct gattc_att_info_cfm *cfm;

    printf("info att_idx = %d \n", att_idx);

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    switch (att_idx)
    {
        case TUYA_BLE_IDX_TX_VAL_IND_CFG:
        {
             cfm->length = TUYA_BLE_IND_NTF_CFG_MAX_LEN;
             cfm->status = GAP_ERR_NO_ERROR;
        }
        break;

        case TUYA_BLE_IDX_RX_VAL:
        {
              cfm->length = MAX_RX_TUYA_BLE_SIZE;
              cfm->status = GAP_ERR_NO_ERROR;
        }
        break;

        default:
        {
             cfm->status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }
        break;
    }

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}


__STATIC int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                         struct gattc_write_req_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{

    struct tuya_ble_env_tag *tuya_ble_env = PRF_ENV_GET(TUYA_BLE, tuya_ble);
	
    uint8_t conidx = KE_IDX_GET(src_id);
	
    uint8_t status = ATT_ERR_NO_ERROR;
	
    int msg_status = KE_MSG_CONSUMED;

    // to check if confirmation message should be send
    bool send_cfm = true;

    // retrieve handle information
    uint8_t att_idx = TUYA_BLE_IDX(param->handle);

    switch (att_idx)
    {

    case TUYA_BLE_IDX_TX_VAL_IND_CFG:
    {

        if (param->length != TUYA_BLE_IND_NTF_CFG_MAX_LEN)
        {
            status = PRF_ERR_UNEXPECTED_LEN;
        }
        else
        {
            status = tuya_ble_update_ntf_ind_cfg(conidx, TUYA_BLE_CFG_STABLE_MEAS_IND, PRF_CLI_START_NTF, co_read16p(param->value)); //liujian notify
        }
    }
    break;

    case TUYA_BLE_IDX_RX_VAL:
    {
        struct tuya_ble_rx_data *rx_data = KE_MSG_ALLOC(TUYA_BLE_RX_DATA_IND,
                                                    prf_dst_task_get(&(tuya_ble_env->prf_env), conidx),
                                                    dest_id, tuya_ble_rx_data);

        rx_data->conidx = conidx;
        rx_data->length = param->length;
        memcpy(rx_data->rx_data, param->value, rx_data->length);

        ke_msg_send(rx_data);
        status = GAP_ERR_NO_ERROR;
    }
    break;

    default:
    {
        status = ATT_ERR_REQUEST_NOT_SUPPORTED;
    }
    break;
    }

    if (send_cfm)
    {
        //Send write response
        struct gattc_write_cfm *cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
        cfm->handle = param->handle;
        cfm->status = status;
        ke_msg_send(cfm);
    }

    return (msg_status);
}

__STATIC int gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_write_req_ind const *param,
                                        ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct tuya_ble_env_tag *tuya_ble_env = PRF_ENV_GET(TUYA_BLE, tuya_ble);
    uint8_t conidx = KE_IDX_GET(src_id);
    uint8_t value[10];
    uint8_t value_size = 0;
    uint8_t status = ATT_ERR_NO_ERROR;

    // retrieve handle information
    uint8_t att_idx = TUYA_BLE_IDX(param->handle);

    printf("att_idx = %d \n", att_idx);

    switch (att_idx)
    {

    case TUYA_BLE_IDX_TX_VAL_IND_CFG:
    {
        value_size = TUYA_BLE_IND_NTF_CFG_MAX_LEN;
        //  co_write16p(value, ((uart_env->ntf_ind_cfg[conidx] & UART_CFG_INTERM_MEAS_NTF) != 0) ? PRF_CLI_START_IND : PRF_CLI_STOP_NTFIND);       //liujian indication

        co_write16p(value, ((tuya_ble_env->ntf_ind_cfg[conidx] & TUYA_BLE_CFG_INTERM_MEAS_NTF) != 0) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND); //liujian notify
    }
    break;

    default:
    {
        status = ATT_ERR_REQUEST_NOT_SUPPORTED;
    }
    break;
    }

    // Send data to peer device
    struct gattc_read_cfm *cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, value_size);
    cfm->length = value_size;
    memcpy(cfm->value, value, value_size);
    cfm->handle = param->handle;
    cfm->status = status;

    // Send value to peer device.
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}


__STATIC int gattc_cmp_evt_handler(ke_msg_id_t const msgid, struct gattc_cmp_evt const *param,
                                   ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // continue operation execution
    tuya_ble_exe_operation();

    return (KE_MSG_CONSUMED);
}


__STATIC int gattc_write_without_handler(ke_msg_id_t const msgid,
                                         struct gattc_write_cmd const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{

    return (KE_MSG_CONSUMED);
}

KE_MSG_HANDLER_TAB(tuya_ble)
{
      {TUYA_BLE_ENABLE_REQ,     (ke_msg_func_t)tuya_ble_enable_req_handler},

      {GATTC_ATT_INFO_REQ_IND,  (ke_msg_func_t)gattc_att_info_req_ind_handler},
	
      {GATTC_WRITE_REQ_IND,     (ke_msg_func_t)gattc_write_req_ind_handler},
	
      {GATTC_READ_REQ_IND,      (ke_msg_func_t)gattc_read_req_ind_handler},
	
      {GATTC_CMP_EVT,           (ke_msg_func_t)gattc_cmp_evt_handler       },
	
      {GATTC_WRITE_CMD,         (ke_msg_func_t)gattc_write_without_handler },

      {TUYA_BLE_DATA_SEND_REQ,  (ke_msg_func_t)tuya_ble_data_send_req_handler},

};


void tuya_ble_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct tuya_ble_env_tag *tuya_ble_env = PRF_ENV_GET(TUYA_BLE, tuya_ble);

    task_desc->msg_handler_tab = tuya_ble_msg_handler_tab;
    task_desc->msg_cnt = ARRAY_LEN(tuya_ble_msg_handler_tab);
    task_desc->state = tuya_ble_env->state;
    task_desc->idx_max = TUYA_BLE_IDX_MAX;
}

#endif