// 参考 Dialog 的代码
//liujian
//2018/4/17
//ver1.0

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_OTA_PROFILE)

#include <string.h>
#include <stdio.h>

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "rdw_ota.h"
#include "rdw_ota_task.h"
#include "prf_utils.h"
#include "gattc.h"
#include "gapc.h"

#include "ke_mem.h"
#include "co_utils.h"


static int ota_enable_req_handler(ke_msg_id_t const msgid,
                                   struct ota_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    // check state of the task
    if(gapc_get_conhdl(param->conidx) != GAP_INVALID_CONHDL)
    {
        struct ota_env_tag* ota_env =(struct ota_env_tag*)prf_env_get(TASK_ID_OTA);
        ota_env->ntf_ind_cfg[param->conidx] = param->ntf_ind_cfg;
        status = GAP_ERR_NO_ERROR;

    }

    // send response
    struct ota_enable_rsp *rsp = KE_MSG_ALLOC(OTA_ENABLE_RSP, src_id, dest_id, ota_enable_rsp);
    rsp->conidx = param->conidx;
    rsp->status = status;
    ke_msg_send(rsp);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_REQ_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_CONSUMED;

    if(ke_state_get(dest_id) == OTA_IDLE)
    {
        struct ota_env_tag* ota_env =(struct ota_env_tag*)prf_env_get(TASK_ID_OTA);
        uint8_t att_idx = param->handle - ota_env->shdl;
        uint8_t status = PRF_APP_ERROR;
        struct gattc_read_cfm* cfm;

        att_size_t length = 0;
        uint8_t *value = NULL;

        uint16_t mtu = 0;

        // Get the requested attribute value
        if (att_idx == SUOTA_IDX_PATCH_STATUS_NTF_CFG)
        {
            status = ATT_ERR_NO_ERROR;
            length = sizeof(uint16_t);
            value = (uint8_t *) &ota_env->ntf_ind_cfg[KE_IDX_GET(src_id)];
        }

        else if(att_idx == SUOTA_IDX_MTU_VAL)
        {
            status = ATT_ERR_NO_ERROR;
            length = sizeof(uint16_t);
            mtu = gattc_get_mtu(KE_IDX_GET(src_id));
            value = (uint8_t*)&mtu;
        }
				else
				{
						status = attm_get_value(param->handle, &length, &value);
				}

        // Send data to peer device
        cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, length);
        cfm->handle = param->handle;
        cfm->status = status;
        cfm->length = length;

        if (length > 0)
        {
            memcpy(cfm->value, value, length);
        }

        ke_msg_send(cfm);
    }
    else if(ke_state_get(dest_id) == OTA_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATT_WRITE_CMD_IND message.
 * The handler will analyse what has been set and decide alert level
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

static int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{

    int msg_status = KE_MSG_CONSUMED;
	
    if(ke_state_get(dest_id) == OTA_IDLE)
    {

        struct gattc_write_cfm *cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id,
                                                   gattc_write_cfm);

        struct ota_env_tag* ota_env =(struct ota_env_tag*)prf_env_get(TASK_ID_OTA);
        uint8_t att_idx = param->handle - ota_env->shdl;
        uint8_t conidx = KE_IDX_GET(src_id);
			
        // Fill in the parameter structure
        cfm->handle = param->handle;
        cfm->status = PRF_APP_ERROR;

        if (att_idx == SUOTA_IDX_PATCH_MEM_DEV_VAL)
        {
            cfm->status = attm_att_set_value(param->handle, param->length,
                                               param->offset,
                                               (uint8_t *) &param->value[0]);
           	
            // Inform APP. Allocate the Patch Mem value change indication
            struct ota_patch_mem_dev_ind *ind = KE_MSG_ALLOC(OTA_PATCH_MEM_DEV_IND,
                                        prf_dst_task_get(&(ota_env->prf_env), conidx),
                                        TASK_ID_OTA, ota_patch_mem_dev_ind);

            // Fill in the parameter structure
            ind->conhdl = gapc_get_conhdl(conidx);

            ind->mem_dev  = 0;
            ind->mem_dev  = (uint32_t) (param->value[3] << 24);
            ind->mem_dev |= (uint32_t) (param->value[2] << 16);
            ind->mem_dev |= (uint32_t) (param->value[1] << 8);
            ind->mem_dev |= (uint32_t) (param->value[0]);
            ind->char_code = OTA_PATCH_MEM_DEV_CHAR;

            ke_msg_send(ind);
								
        }
        else if (att_idx == SUOTA_IDX_GPIO_MAP_VAL)
        {
					
					
            cfm->status = attm_att_set_value(param->handle, param->length,
                                               param->offset,
                                               (uint8_t *) &param->value[0]);


            struct ota_gpio_map_ind *ind = KE_MSG_ALLOC(OTA_GPIO_MAP_IND,
                                        prf_dst_task_get(&(ota_env->prf_env), conidx),
                                        TASK_ID_OTA, ota_gpio_map_ind);


            ind->conhdl = gapc_get_conhdl(conidx);

            ind->gpio_map  = 0;
            ind->gpio_map  = (uint32_t) (param->value[3] << 24);
            ind->gpio_map |= (uint32_t) (param->value[2] << 16);
            ind->gpio_map |= (uint32_t) (param->value[1] << 8);
            ind->gpio_map |= (uint32_t) (param->value[0]);
            ind->char_code = OTA_GPIO_MAP_CHAR;

            ke_msg_send(ind);

					
        }
        else if (att_idx == SUOTA_IDX_PATCH_DATA_VAL )
        {
            cfm->status = attm_att_set_value(param->handle, param->length,
                                               param->offset,
                                               (uint8_t *) &param->value[0]);
					
				
            struct ota_patch_data_ind *ind = KE_MSG_ALLOC_DYN(OTA_PATCH_DATA_IND,
                                        prf_dst_task_get(&(ota_env->prf_env), conidx),
                                        TASK_ID_OTA, ota_patch_data_ind, param->length);

            // Fill in the parameter structure
            ind->conhdl = gapc_get_conhdl(conidx);

            memcpy(ind->pd, &param->value[0], param->length);
            ind->len = param->length;
            ind->char_code = OTA_PATCH_DATA_CHAR;

            // Send the message
            ke_msg_send(ind);


        }
        else if (att_idx == SUOTA_IDX_PATCH_LEN_VAL)
        {
            cfm->status = attm_att_set_value(param->handle, param->length,
                                               param->offset,
                                               (uint8_t *) &param->value[0]);

            // Inform APP. Allocate the Patch Data value change indication
            struct ota_patch_len_ind *ind = KE_MSG_ALLOC(OTA_PATCH_LEN_IND,
                                        prf_dst_task_get(&(ota_env->prf_env), conidx),
                                        TASK_ID_OTA, ota_patch_len_ind);
						cfm->status = ATT_ERR_NO_ERROR;
            // Fill in the parameter structure
            ind->conhdl = gapc_get_conhdl(conidx);

            ind->len  = 0;
            ind->len  = (uint16_t) (param->value[1] << 8);
            ind->len |= (uint16_t) (param->value[0]);
            ind->char_code = OTA_PATCH_LEN_CHAR;

            // Send the message
            ke_msg_send(ind);
			
        }
        else if (att_idx == SUOTA_IDX_PATCH_STATUS_NTF_CFG)
        {
            ota_env->ntf_ind_cfg[conidx] = co_read16p(&param->value[0]);
            cfm->status = ATT_ERR_NO_ERROR;
        }
        else
        {
            cfm->status = ATT_ERR_APP_ERROR;

        }

        ke_msg_send(cfm);
    }
    else if(ke_state_get(dest_id) == OTA_BUSY)
    {

        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}


/**
 ****************************************************************************************
 * @brief Update Memory info characteristic value.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int  ota_mem_info_update_req_handler(ke_msg_id_t const msgid,
                                    struct ota_patch_mem_info_update_req const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{

    struct ota_env_tag* ota_env =(struct ota_env_tag*)prf_env_get(TASK_ID_OTA);

  
    // Update Memory info charecteristic value
    uint8_t status = attm_att_set_value(ota_env->shdl + SUOTA_IDX_PATCH_MEM_INFO_VAL,
                                          sizeof(uint32_t), 0,
                                          (uint8_t *) &param->mem_info);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref SUOTAR_STATUS_UPDATE_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

static int ota_status_update_req_handler (ke_msg_id_t const msgid,
                                         struct ota_status_update_req const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
      
    struct ota_env_tag* ota_env =(struct ota_env_tag*)prf_env_get(TASK_ID_OTA);
	  uint8_t conidx = KE_IDX_GET(src_id);

    // Update the value in the attribute database
    uint8_t status = attm_att_set_value(ota_env->shdl + SUOTA_IDX_PATCH_STATUS_VAL,
                                  sizeof(uint8_t), 0, (uint8_t *) &param->status);


    if (ota_env->ntf_ind_cfg[conidx] != PRF_CLI_STOP_NTFIND)
    {
    
        // Allocate the GATT notification message
        struct gattc_send_evt_cmd *req = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                                         KE_BUILD_ID(TASK_GATTC, KE_IDX_GET(src_id)),
                                         dest_id, gattc_send_evt_cmd,
                                         sizeof(param->status));

        // Fill in the parameter structure
        req->operation = (ota_env->ntf_ind_cfg[conidx] == PRF_CLI_START_NTF) ? GATTC_NOTIFY :
                                                 GATTC_INDICATE;
        req->seq_num = 0;
        req->handle = (ota_env->shdl + SUOTA_IDX_PATCH_STATUS_VAL);
        req->length = sizeof(param->status);
        memcpy(req->value, &param->status, req->length);

        // Send the event
        ke_msg_send(req);
		
    }

    return (KE_MSG_CONSUMED);
}




static  int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gattc_att_info_req_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
	  return (KE_MSG_CONSUMED);
	
}

/// Default State handlers definition
KE_MSG_HANDLER_TAB(ota)
{
    {OTA_ENABLE_REQ,                 (ke_msg_func_t)  ota_enable_req_handler},

    {GATTC_ATT_INFO_REQ_IND,         (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,            (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,             (ke_msg_func_t) gattc_read_req_ind_handler},
	  {OTA_PATCH_MEM_INFO_UPDATE_REQ,  (ke_msg_func_t) ota_mem_info_update_req_handler},
    {OTA_STATUS_UPDATE_REQ,          (ke_msg_func_t) ota_status_update_req_handler},
	

};

void ota_task_init(struct ke_task_desc *task_desc)
{

    struct ota_env_tag* ota_env =(struct ota_env_tag*)prf_env_get(TASK_ID_OTA);

    task_desc->msg_handler_tab = ota_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(ota_msg_handler_tab);
    task_desc->state           = ota_env->state;
    task_desc->idx_max         = OTA_IDX_MAX;
}

#endif 



