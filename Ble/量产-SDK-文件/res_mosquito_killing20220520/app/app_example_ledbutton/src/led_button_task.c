/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	led_button_task.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/6/28

	Description :   Led and button service task implementation
	Changed Log	:

		Liujian  2020/6/28		- Creation
*****************************************************************************/

#include "rwip_config.h"

#if defined(CFG_PRF_LED_BUTTON)
#include "gap.h"
#include "gattc_task.h"
#include "gap.h"
#include "attm.h"
#include "led_button_task.h"
#include "led_button.h"
#include "rdw_mcu.h"
#include "prf_utils.h"
#include "prf.h"
#include "co_utils.h"
#include "ke_mem.h"

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref LED_BUTTON_ENABLE_REQ message.
 * The handler enables the Led Button Profile.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int led_button_enable_req_handler(ke_msg_id_t const msgid,
                                   struct led_button_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

	  struct led_button_env_tag* led_button_env = PRF_ENV_GET(LED_BUTTON, led_button);
	
	  led_button_env->led_value = param->led_value;
	
    // check state of the task
    if(gapc_get_conhdl(param->conidx) != GAP_INVALID_CONHDL)
    {
        // restore Bond Data
        struct led_button_env_tag* led_button_env =  PRF_ENV_GET(LED_BUTTON, led_button);
        led_button_env->ntf_ind_cfg[param->conidx] = param->ntf_ind_cfg;
        status = GAP_ERR_NO_ERROR;

    }

    // send response
    struct led_button_enable_rsp *rsp = KE_MSG_ALLOC(LED_BUTTON_ENABLE_RSP, src_id, dest_id, led_button_enable_rsp);
    rsp->conidx = param->conidx;
    rsp->status = status;
    ke_msg_send(rsp);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref LED_BUTTON_BUTTON_SEND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int led_button_button_send_req_handler(ke_msg_id_t const msgid,
                                      struct led_button_button_send_req const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // Status
    int msg_status = KE_MSG_SAVED;
    uint8_t state = ke_state_get(dest_id);

    // check state of the task
    if(state == LED_BUTTON_IDLE)
    {
        // Get the address of the environment
        struct led_button_env_tag *led_button_env = PRF_ENV_GET(LED_BUTTON, led_button);

        {
            // allocate operation to execute
            led_button_env->operation    = (struct led_button_op *) ke_malloc(sizeof(struct led_button_op) + LED_BUTTON_LED_MAX_LEN, KE_MEM_ATT_DB);

            // Initialize operation parameters
            led_button_env->operation->cursor  = 0;
            led_button_env->operation->dest_id = src_id;
            led_button_env->operation->conidx  = GAP_INVALID_CONIDX;

       
            led_button_env->operation->op      = LED_BUTTON_CFG_BUTTON_NTF;
            led_button_env->operation->handle  = LED_BUTTON_HANDLE(LED_BUTTON_IDX_BUTTON_VAL);
        
						led_button_env->operation->data[0] = param->key_value;
            //Pack the temperature measurement value
            led_button_env->operation->length  = 1;

            // put task in a busy state
            ke_state_set(dest_id, LED_BUTTON_BUSY);

            // execute operation
            led_button_exe_operation();
        }

        msg_status = KE_MSG_CONSUMED;
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
    struct led_button_env_tag* led_button_env = PRF_ENV_GET(LED_BUTTON, led_button);
    uint8_t att_idx = LED_BUTTON_IDX(param->handle);
    struct gattc_att_info_cfm * cfm;

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;
	
    switch(att_idx)
    {
        case LED_BUTTON_IDX_BUTTON_IND_CFG:
        {
            cfm->length = LED_BUTTON_IND_NTF_CFG_MAX_LEN;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;

        default:
        {
            cfm->status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }break;
    }

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GL2C_CODE_ATT_WR_CMD_IND message.
 * The handler compares the new values with current ones and notifies them if they changed.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{

    struct led_button_env_tag* led_button_env = PRF_ENV_GET(LED_BUTTON, led_button);
    uint8_t conidx  = KE_IDX_GET(src_id);
    uint8_t status = ATT_ERR_NO_ERROR;
    int msg_status = KE_MSG_CONSUMED;

    // to check if confirmation message should be send
    bool send_cfm = true;

    // retrieve handle information
    uint8_t att_idx = LED_BUTTON_IDX(param->handle);

		switch(att_idx)
		{
				case LED_BUTTON_IDX_LED_VAL:
				{
						if(param->length != 1)
						{
								status = PRF_ERR_UNEXPECTED_LEN;
							  break;
						}
						
						uint8_t led_value = param->value[0];

					  struct led_button_led_req_ind * req_ind = KE_MSG_ALLOC(LED_BUTTON_LED_REQ_IND,
												prf_dst_task_get(&led_button_env->prf_env, conidx), dest_id, led_button_led_req_ind);
								req_ind->conidx = conidx;
								req_ind->led_value   = led_value;
								ke_msg_send(req_ind);
								led_button_env->led_value = led_value;
						
				}break;

				case LED_BUTTON_IDX_BUTTON_IND_CFG:
				{
						status = led_button_update_ntf_ind_cfg(conidx, LED_BUTTON_CFG_BUTTON_NTF, PRF_CLI_START_NTF, co_read16p(param->value));
				}break;

				
				default:
				{
						status = ATT_ERR_REQUEST_NOT_SUPPORTED;
				}break;
		}
    

    if(send_cfm)
    {
        //Send write response
        struct gattc_write_cfm * cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
        cfm->handle = param->handle;
        cfm->status = status;
        ke_msg_send(cfm);
    }

    return (msg_status);
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
__STATIC int gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct led_button_env_tag* led_button_env = PRF_ENV_GET(LED_BUTTON, led_button);
    uint8_t conidx  = KE_IDX_GET(src_id);
    uint8_t value[2];
    uint8_t value_size = 0;
    uint8_t status = ATT_ERR_NO_ERROR;

    // retrieve handle information
    uint8_t att_idx = LED_BUTTON_IDX(param->handle);
	
    switch(att_idx)
    {
        case LED_BUTTON_IDX_LED_VAL:     //¶ÁÈ¡LED  µÄÖµ
        {
            value_size = 1;
            co_write8(&(value[0]), led_button_env->led_value);
        }break;

       
        case LED_BUTTON_IDX_BUTTON_IND_CFG:
        {
            value_size = 2;
            co_write16p(value, ((led_button_env->ntf_ind_cfg[conidx]) != 0) ? PRF_CLI_START_IND : PRF_CLI_STOP_NTFIND);
        }break;

        default:
        {
            status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }break;
    }

    // Send data to peer device
    struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, value_size);
    cfm->length = value_size;
    memcpy(cfm->value, value, value_size);
    cfm->handle = param->handle;
    cfm->status = status;

    // Send value to peer device.
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles @ref GATTC_CMP_EVT for GATTC_NOTIFY and GATT_INDICATE message meaning
 * that Measurement notification/indication has been correctly sent to peer device
 *
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int gattc_cmp_evt_handler(ke_msg_id_t const msgid, struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // continue operation execution
    led_button_exe_operation();

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition

KE_MSG_HANDLER_TAB(led_button){
    {LED_BUTTON_ENABLE_REQ,      (ke_msg_func_t)led_button_enable_req_handler},

    {GATTC_ATT_INFO_REQ_IND,     (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,        (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,         (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,              (ke_msg_func_t) gattc_cmp_evt_handler},

    {LED_BUTTON_BUTTON_SEND_REQ, (ke_msg_func_t)led_button_button_send_req_handler},

};


void led_button_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct led_button_env_tag *led_button_env = PRF_ENV_GET(LED_BUTTON, led_button);

    task_desc->msg_handler_tab = led_button_msg_handler_tab;
    task_desc->msg_cnt = ARRAY_LEN(led_button_msg_handler_tab);
    task_desc->state = led_button_env->state;
    task_desc->idx_max = LED_BUTTON_IDX_MAX;
}


#endif