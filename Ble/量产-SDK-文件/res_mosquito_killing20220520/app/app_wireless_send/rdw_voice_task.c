
//liujian  2018/06/12
//v1.0

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (RDW_VOICE_PROFILE)

#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "prf_utils.h"
#include "ke_mem.h"
#include "co_utils.h"
#include <string.h>
#include "gapc.h"

#include "rdw_voice.h"
#include "rdw_voice_task.h"


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

__STATIC int voice_enable_req_handler(ke_msg_id_t const msgid,
                                   struct voice_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    // check state of the task
    if(gapc_get_conhdl(param->conidx) != GAP_INVALID_CONHDL)
    {
        // restore Bond Data
        struct voice_env_tag* voice_env =  PRF_ENV_GET(VOICE, voice);
        voice_env->ntf_ind_cfg[param->conidx] = param->ntf_ind_cfg;
        status = GAP_ERR_NO_ERROR;
    }
    // send response
    struct voice_enable_rsp *rsp = KE_MSG_ALLOC(VOICE_ENABLE_RSP, src_id, dest_id, voice_enable_rsp);
    rsp->conidx = param->conidx;
    rsp->status = status;
    ke_msg_send(rsp);

    return (KE_MSG_CONSUMED);
}


//发送录音开始和结束信号
__STATIC int voice_send_start_handler(ke_msg_id_t const msgid,
                                      struct voice_start_req const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // Status
    int msg_status = KE_MSG_SAVED;
    uint8_t state = ke_state_get(dest_id);

    // check state of the task
    if(state == VOICE_IDLE)
    {
        // Get the address of the environment
        struct voice_env_tag *voice_env = PRF_ENV_GET(VOICE, voice);

        {
            // allocate operation to execute
            voice_env->operation    = (struct voice_op *) ke_malloc(sizeof(struct voice_op) + sizeof(struct voice_start_req ), KE_MEM_ATT_DB);

            // Initialize operation parameters
            voice_env->operation->cursor  = 0;
            voice_env->operation->dest_id = src_id;
            voice_env->operation->conidx  = GAP_INVALID_CONIDX;
        
            voice_env->operation->op      = VOICE_CFG_START_NTF;
            voice_env->operation->handle  = VOICE_HANDLE(VOICE_START_IDX_TX_VAL);
           
            voice_env->operation->length  = 1; 
						voice_env->operation->data[0] = param->start_state;
            // put task in a busy state
            ke_state_set(dest_id, VOICE_BUSY);

            // execute operation
            voice_exe_operation();
        }

        msg_status = KE_MSG_CONSUMED;
    }

    return (msg_status);
}


//发送语音数据
__STATIC int voice_send_data_handler(ke_msg_id_t const msgid,
                                      struct voice_data_req const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // Status
    int msg_status = KE_MSG_CONSUMED; //KE_MSG_SAVED; 测试
    uint8_t state = ke_state_get(dest_id);

    // check state of the task
    if(state == VOICE_IDLE)
    {
        // Get the address of the environment
        struct voice_env_tag *voice_env = PRF_ENV_GET(VOICE, voice);

        {
            // allocate operation to execute
            voice_env->operation    = (struct voice_op *) ke_malloc(sizeof(struct voice_op) + sizeof(struct voice_data_req ), KE_MEM_ATT_DB);

            // Initialize operation parameters
            voice_env->operation->cursor  = 0;
            voice_env->operation->dest_id = src_id;
            voice_env->operation->conidx  = GAP_INVALID_CONIDX;
        
            voice_env->operation->op      = VOICE_CFG_DATA_NTF;
            voice_env->operation->handle  = VOICE_HANDLE(VOICE_AUDIO_IDX_TX_VAL);
           
            voice_env->operation->length  = param->length; 
						memcpy(&(voice_env->operation->data[0]), param->audio_data, param->length);   
            // put task in a busy state
            ke_state_set(dest_id, VOICE_BUSY);

            // execute operation
            voice_exe_operation();
        }

        msg_status = KE_MSG_CONSUMED;
    }
		else
		{
				printf("save .... \n");
		}

    return (msg_status);
}


//这个是对方设备,也就是客户端想要知道属性能够接纳写的最大长度
__STATIC int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gattc_att_info_req_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    struct voice_env_tag* voice_env = PRF_ENV_GET(VOICE, voice);
    uint8_t att_idx = VOICE_IDX(param->handle);
    struct gattc_att_info_cfm * cfm;
	
    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

    switch(att_idx)
    {
        case VOICE_START_IDX_TX_VAL_IND_CFG:
        {
            cfm->length = VOICE_IND_NTF_CFG_MAX_LEN;
            cfm->status = GAP_ERR_NO_ERROR;
        }break;
		
		case VOICE_AUDIO_IDX_TX_VAL_IND_CFG:
        {
            cfm->length = VOICE_IND_NTF_CFG_MAX_LEN;
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


//改变控制属性

__STATIC int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{

    struct  voice_env_tag* voice_env = PRF_ENV_GET(VOICE, voice);
    uint8_t conidx  = KE_IDX_GET(src_id);
    uint8_t status = ATT_ERR_NO_ERROR;
    int msg_status = KE_MSG_CONSUMED;
    // to check if confirmation message should be send
    bool send_cfm = true;

    // retrieve handle information
    uint8_t att_idx = VOICE_IDX(param->handle);

		switch(att_idx)
		{
				case VOICE_START_IDX_TX_VAL_IND_CFG:
				{
						if(param->length != VOICE_IND_NTF_CFG_MAX_LEN)
						{
								status = PRF_ERR_UNEXPECTED_LEN;
						}
						else
						{

							status = voice_update_ntf_ind_cfg(conidx, VOICE_CFG_START_NTF, PRF_CLI_START_NTF, co_read16p(param->value));   //liujian notify
						}
				}
				break;
			
				case VOICE_AUDIO_IDX_TX_VAL_IND_CFG:
				{
						if(param->length != VOICE_IND_NTF_CFG_MAX_LEN)
						{
								status = PRF_ERR_UNEXPECTED_LEN;
						}
						else
						{

							status = voice_update_ntf_ind_cfg(conidx, VOICE_CFG_DATA_NTF, PRF_CLI_START_NTF, co_read16p(param->value));   //liujian notify
						}
						
				}		
				break;

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

//读取属性
__STATIC int gattc_read_req_ind_handler(ke_msg_id_t const msgid, struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    struct  voice_env_tag* voice_env = PRF_ENV_GET(VOICE, voice);
    uint8_t conidx  = KE_IDX_GET(src_id);
    uint8_t value[10];
    uint8_t value_size = 0;
    uint8_t status = ATT_ERR_NO_ERROR;

    // retrieve handle information
    uint8_t att_idx = VOICE_IDX(param->handle);
	//printf("att_idx = %d \n", att_idx);
    switch(att_idx)
    {
        case VOICE_START_IDX_TX_VAL_IND_CFG:
        {
            value_size = VOICE_IND_NTF_CFG_MAX_LEN;
						co_write16p(value, ((voice_env->ntf_ind_cfg[conidx] & VOICE_CFG_START_NTF) != 0) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND);    //liujian notify 
					
        }break;

		case VOICE_AUDIO_IDX_TX_VAL_IND_CFG:
        {
            value_size = VOICE_IND_NTF_CFG_MAX_LEN;
       		
			co_write16p(value, ((voice_env->ntf_ind_cfg[conidx] & VOICE_CFG_DATA_NTF) != 0) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND);    //liujian notify 
					
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




__STATIC int gattc_cmp_evt_handler(ke_msg_id_t const msgid, struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    // continue operation execution
    voice_exe_operation();

    return (KE_MSG_CONSUMED);
}

__STATIC int gattc_write_without_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_cmd const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
	
	 return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
KE_MSG_HANDLER_TAB(voice)
{
    {VOICE_ENABLE_REQ,            (ke_msg_func_t) voice_enable_req_handler},

    {GATTC_ATT_INFO_REQ_IND,     (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,        (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,         (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,              (ke_msg_func_t) gattc_cmp_evt_handler},
	{GATTC_WRITE_CMD,            (ke_msg_func_t) gattc_write_without_handler},

    {VOICE_AUDIO_START_REQ,         (ke_msg_func_t) voice_send_start_handler},
	
	{VOICE_AUDIO_DATA_REQ,         (ke_msg_func_t) voice_send_data_handler},
	
 
};

void voice_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct voice_env_tag * voice_env = PRF_ENV_GET(VOICE, voice);

    task_desc->msg_handler_tab = voice_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(voice_msg_handler_tab);
    task_desc->state           = voice_env->state;
    task_desc->idx_max         = VOICE_IDX_MAX;
}

#endif 


