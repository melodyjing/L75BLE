/**@file            ble_wechat_task.c
* @brief            
* @details          
* @author      			Liujian
* @date             2020-11-26
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.

                   使用微信服务 的任务
**********************************************************************************/
#include "rwip_config.h"

#if (BLE_WECHAT_ENABLE == 1)

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "ble_wechat_service.h"
#include "ble_wechat_task.h"
#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"
#include <string.h>
#include "nvds.h"
__STATIC int wechat_enable_req_handler(ke_msg_id_t const msgid,
                                     struct wechat_enable_req const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    // check state of the task
    if (gapc_get_conhdl(param->conidx) != GAP_INVALID_CONHDL)
    {
        // restore Bond Data
        struct wechat_env_tag *wechat_env = PRF_ENV_GET(WECHAT, wechat);
        wechat_env->ntf_ind_cfg[param->conidx] = param->ntf_ind_cfg;
        status = GAP_ERR_NO_ERROR;
    }

    // send response
    struct wechat_enable_rsp *rsp = KE_MSG_ALLOC(WECHAT_ENABLE_RSP, src_id, dest_id, wechat_enable_rsp);
    rsp->conidx = param->conidx;
    rsp->status = status;
    ke_msg_send(rsp);

    return (KE_MSG_CONSUMED);
}


__STATIC int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
                                            struct gattc_att_info_req_ind *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    struct wechat_env_tag *wechat_env = PRF_ENV_GET(WECHAT, wechat);
    uint8_t att_idx = WECHAT_IDX(param->handle);
    struct gattc_att_info_cfm *cfm;

    printf("info att_idx = %d \n", att_idx);

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    switch (att_idx)
    {
    case WECHAT_IDX_IND_CFG:
    {
        cfm->length = WECHAT_IND_NTF_CFG_MAX_LEN;
        cfm->status = GAP_ERR_NO_ERROR;
    }
    break;

    case WECHAT_IDX_WRITE_FEATURE_VAL:
    {
        cfm->length = BLE_WECHAT_MAX_DATA_LEN;
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

    struct wechat_env_tag *wechat_env = PRF_ENV_GET(WECHAT, wechat);
    uint8_t conidx = KE_IDX_GET(src_id);
    uint8_t status = ATT_ERR_NO_ERROR;
    int msg_status = KE_MSG_CONSUMED;

    // to check if confirmation message should be send
    bool send_cfm = true;

    // retrieve handle information
    uint8_t att_idx = WECHAT_IDX(param->handle);

//	  printf("gattc_write_req_ind_handler att_idx = %d ..... \n", att_idx);
    switch (att_idx)
    {

    case WECHAT_IDX_IND_CFG:
    {

        if (param->length != WECHAT_IND_NTF_CFG_MAX_LEN)
        {
            status = PRF_ERR_UNEXPECTED_LEN;
        }
        else
        {

           //    status = uart_update_ntf_ind_cfg(conidx, UART_CFG_STABLE_MEAS_IND, PRF_CLI_START_IND, co_read16p(param->value));   //liujian indication

            status = wechat_update_ntf_ind_cfg(conidx, WECHAT_CFG_DATA_IND, PRF_CLI_START_NTF, co_read16p(param->value)); //liujian notify
        }
    }
    break;

    case WECHAT_IDX_WRITE_FEATURE_VAL:
    {
           struct wechat_rx_data *rx_data = KE_MSG_ALLOC(WECHAT_RX_DATA_IND,
                                                    prf_dst_task_get(&(wechat_env->prf_env), conidx),
                                                    dest_id, wechat_rx_data);
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
    struct wechat_env_tag *wechat_env = PRF_ENV_GET(WECHAT, wechat);
    uint8_t conidx = KE_IDX_GET(src_id);
    uint8_t value[10];
    uint8_t value_size = 0;
    uint8_t status = ATT_ERR_NO_ERROR;

    // retrieve handle information
    uint8_t att_idx = WECHAT_IDX(param->handle);

    printf("wechat att_idx = %d \n", att_idx);

    switch (att_idx)
    {

      case WECHAT_IDX_IND_CFG:
      {
         value_size = WECHAT_IND_NTF_CFG_MAX_LEN;
        
			   co_write16p(value, ((wechat_env->ntf_ind_cfg[conidx] & WECHAT_CFG_DATA_IND) != 0) ? PRF_CLI_START_IND : PRF_CLI_STOP_NTFIND);       //liujian indication

        //co_write16p(value, ((wechat_env->ntf_ind_cfg[conidx] & WECHAT_CFG_DATA_IND) != 0) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND); //liujian notify
      }
      break;
		
			case WECHAT_IDX_READ_FEATURE_VAL:
			{
				  value_size = 6;
				  uint8_t temp_length;
		      struct bd_addr  temp_bdaddr;
          temp_length = BD_ADDR_LEN;
          nvds_get(PARAM_ID_BD_ADDRESS, &temp_length, (uint8_t *)&temp_bdaddr.addr);
		
          value[0] =  temp_bdaddr.addr[5];
		      value[1] =  temp_bdaddr.addr[4];
		      value[2] =  temp_bdaddr.addr[3];
		      value[3] =  temp_bdaddr.addr[2];
		      value[4] =  temp_bdaddr.addr[1];
		      value[5] =  temp_bdaddr.addr[0];	
				
				  break;
			}

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


__STATIC int wechat_send_data_req_handler(ke_msg_id_t const msgid,
                                        struct wechat_data_send_req const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    // Status
    int msg_status = KE_MSG_SAVED;
    uint8_t state = ke_state_get(dest_id);

    // check state of the task
    if (state == WECHAT_IDLE)
    {
        // Get the address of the environment
        struct wechat_env_tag *wechat_env = PRF_ENV_GET(WECHAT, wechat);

        {
            // allocate operation to execute
            wechat_env->operation = (struct wechat_op *)ke_malloc(sizeof(struct wechat_op) + sizeof(struct wechat_data_send_req), KE_MEM_ATT_DB);

            // Initialize operation parameters
            wechat_env->operation->cursor = 0;
            wechat_env->operation->dest_id = src_id;
            wechat_env->operation->conidx = GAP_INVALID_CONIDX;

            wechat_env->operation->op = WECHAT_CFG_DATA_IND;

            wechat_env->operation->handle = WECHAT_HANDLE(WECHAT_IDX_IND_VAL);

            wechat_env->operation->length = param->length;
            memcpy(&(wechat_env->operation->data[0]), param->tx_data, param->length);
            // put task in a busy state
            ke_state_set(dest_id, WECHAT_BUSY);

            // execute operation
            wechat_exe_operation();
        }

        msg_status = KE_MSG_CONSUMED;
    }

    return (msg_status);
}


__STATIC int gattc_cmp_evt_handler(ke_msg_id_t const msgid, struct gattc_cmp_evt const *param,
                                   ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // continue operation execution
    wechat_exe_operation();

    return (KE_MSG_CONSUMED);
}


__STATIC int gattc_write_without_handler(ke_msg_id_t const msgid,
                                         struct gattc_write_cmd const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
       printf("gattc_write_without_handler ..... \n");
    return (KE_MSG_CONSUMED);
}

/// Default State handlers definition
KE_MSG_HANDLER_TAB(wechat)
{
      {WECHAT_ENABLE_REQ, (ke_msg_func_t)wechat_enable_req_handler},

      {GATTC_ATT_INFO_REQ_IND, (ke_msg_func_t)gattc_att_info_req_ind_handler},
			
      {GATTC_WRITE_REQ_IND, (ke_msg_func_t)gattc_write_req_ind_handler},
			
      {GATTC_READ_REQ_IND, (ke_msg_func_t)gattc_read_req_ind_handler},
	
      {GATTC_CMP_EVT, (ke_msg_func_t)gattc_cmp_evt_handler},
	
      {GATTC_WRITE_CMD, (ke_msg_func_t)gattc_write_without_handler},

      {WECHAT_DATA_SEND_REQ, (ke_msg_func_t)wechat_send_data_req_handler},

};

void wechat_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct wechat_env_tag *wechat_env = PRF_ENV_GET(WECHAT, wechat);

    task_desc->msg_handler_tab = wechat_msg_handler_tab;
    task_desc->msg_cnt = ARRAY_LEN(wechat_msg_handler_tab);
    task_desc->state = wechat_env->state;
    task_desc->idx_max = WECHAT_IDX_MAX;
}

#endif
