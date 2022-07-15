/* 
 liujian add
 2018/6/12
 version 1.0 
 
 voice of BLE
*/
//rdw_voice.c  和 rdw_voice_task.c 一般是一起合作, 两个文件共同完成一个 profile  的工作

#include "rwip_config.h"     // SW configuration

#if (RDW_VOICE_PROFILE)
#include "attm.h"
#include "rdw_voice.h"
#include "co_utils.h"
#include "prf_utils.h"
#include "rdw_voice_task.h"

#include "ke_mem.h"
#include "co_utils.h"
#include "gattc_task.h"

static uint16_t voice_compute_att_table(uint16_t features);


const struct attm_desc_128 voice_att_db[VOICE_IDX_NB] =
{


		[VOICE_IDX_SVC]            =   {{0x00, 0x28},  PERM(RD, ENABLE), 0, 0},
//声音开始采集
		[VOICE_START_IDX_TX_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},

		[VOICE_START_IDX_TX_VAL]         =   {  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0xB0, 0x00, 0x40, 0x51, 0x04, 0x01, 0xB0, 0x00, 0xF0}, PERM(RD, ENABLE)|PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
		//0xB001
		[VOICE_START_IDX_TX_VAL_IND_CFG]     =   {{0x02, 0x29}, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},

		
//声音采集的数据		
		[VOICE_AUDIO_IDX_TX_CHAR]        =   {{0x03, 0x28},   PERM(RD, ENABLE), 0, 0},

		[VOICE_AUDIO_IDX_TX_VAL]         =   { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0xB0, 0x00, 0x40, 0x51, 0x04, 0x02, 0xB0, 0x00, 0xF0},  PERM(RD, ENABLE)|PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
		
		[VOICE_AUDIO_IDX_TX_VAL_IND_CFG]     =   {{0x02, 0x29}, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0}
		
};


//注册VOICE profile  属性

static uint8_t rdw_voice_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl, struct voice_db_cfg* params)
{
    // Service content flag
    uint16_t cfg_flag;
    // DB Creation Status
    uint8_t status = ATT_ERR_NO_ERROR;

    cfg_flag = voice_compute_att_table(params->features);
	
//	uint8_t * p = ke_malloc(100, KE_MEM_ATT_DB);


	uint8_t  voice_uuid[16] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0xB0, 0x00, 0x40, 0x51, 0x04, 0x00, 0xB0, 0x00, 0xF0};

// { 0xF0, 0x00, 0xB0, 0x00, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	status = attm_svc_create_db_128(start_hdl, voice_uuid, (uint8_t *)&cfg_flag, VOICE_IDX_NB, NULL, env->task, &voice_att_db[0], sec_lvl);
		 

    if( status == ATT_ERR_NO_ERROR )
    {
        //-------------------- allocate memory required for the profile  ---------------------
        struct voice_env_tag*  voice_env =
                (struct voice_env_tag* ) ke_malloc(sizeof(struct voice_env_tag), KE_MEM_ATT_DB);

        // allocate PROXR required environment variable
        env->env = (prf_env_t*) voice_env;

        voice_env->shdl     = *start_hdl;
        voice_env->prf_env.app_task = app_task | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        voice_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_VOICE;          //这个ID  号要在 rwip_task.h 文件中定义
		
        voice_task_init(&(env->desc));              //注册消息函数, 在另外一个文件中

        //Save features on the environment
        voice_env->features      = params->features;
        voice_env->operation     = NULL;
        memset(voice_env->ntf_ind_cfg, 0 , sizeof(voice_env->ntf_ind_cfg));

        // service is ready, go into an Idle state
        ke_state_set(env->task, VOICE_IDLE);
    }
    return (status);
}



static uint16_t voice_compute_att_table(uint16_t features)
{
    //Temperature Measurement Characteristic is mandatory
    uint16_t att_table = VOICE_PROFILE_MASK;
    return att_table;
}

static void voice_destroy(struct prf_task_env* env)
{
    struct voice_env_tag* voice_env = (struct voice_env_tag*) env->env;

    // free profile environment variables
    if(voice_env->operation != NULL)
    {
        ke_free(voice_env->operation);
    }


    env->env = NULL;
    ke_free(voice_env);
}

static void voice_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Clear configuration for this connection */
    struct voice_env_tag* voice_env = (struct voice_env_tag*) env->env;
    voice_env->ntf_ind_cfg[conidx] = 0;
}

static void voice_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    /* Clear configuration for this connection */
    struct voice_env_tag* voice_env = (struct voice_env_tag*) env->env;
    voice_env->ntf_ind_cfg[conidx] = 0;
}


const struct prf_task_cbs voice_itf =
{
    (prf_init_fnct) rdw_voice_init,
    voice_destroy,
    voice_create,
    voice_cleanup,
};


//这个函数要添加到 prf.c 文件中去
const struct prf_task_cbs* voice_prf_itf_get(void)
{
   return &voice_itf;
}


void  voice_exe_operation(void)
{
    struct voice_env_tag* voice_env = PRF_ENV_GET(VOICE, voice);

    ASSERT_ERR(voice_env->operation != NULL);

    bool finished = true;

    while(voice_env->operation->cursor < BLE_CONNECTION_MAX)
    {
        // check if this type of event is enabled
        //if(((voice_env->ntf_ind_cfg[voice_env->operation->cursor] & voice_env->operation->op) != 0) && (voice_env->operation->conidx != voice_env->operation->cursor))
					
				if((voice_env->operation->conidx != voice_env->operation->cursor))
        {
            // trigger the event
            struct gattc_send_evt_cmd * evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                    KE_BUILD_ID(TASK_GATTC , voice_env->operation->cursor), prf_src_task_get(&voice_env->prf_env, 0),
                    gattc_send_evt_cmd, voice_env->operation->length);

            evt->operation = GATTC_NOTIFY;     //(uart_env->operation->op != UART_CFG_INTERM_MEAS_NTF) ? GATTC_INDICATE : GATTC_NOTIFY;
            evt->length    = voice_env->operation->length;
            evt->handle    = voice_env->operation->handle;
            memcpy(evt->value, voice_env->operation->data, evt->length);

            ke_msg_send(evt);

            finished = false;
            voice_env->operation->cursor++;
            break;
        }
        voice_env->operation->cursor++;
    }


    // check if operation is finished
    if(finished)
    {
        // do not send response if operation has been locally requested
        if(voice_env->operation->dest_id != prf_src_task_get(&voice_env->prf_env, 0))
        {
			if(voice_env->operation->op == VOICE_CFG_START_NTF)
			{
				struct voice_start_rsp * rsp = KE_MSG_ALLOC(VOICE_AUDIO_START_RSP,  voice_env->operation->dest_id, prf_src_task_get(&voice_env->prf_env, 0),
                                 voice_start_rsp);
								 
				rsp->status = GAP_ERR_NO_ERROR;
				ke_msg_send(rsp);
			}
			else if(voice_env->operation->op == VOICE_CFG_DATA_NTF)
			{
				struct voice_data_rsp * rsp = KE_MSG_ALLOC(VOICE_AUDIO_DATA_RSP,  voice_env->operation->dest_id, prf_src_task_get(&voice_env->prf_env, 0),
                                 voice_data_rsp);
								 
				rsp->status = GAP_ERR_NO_ERROR;
				ke_msg_send(rsp);
			}
        }

        // free operation
        ke_free(voice_env->operation);
        voice_env->operation = NULL;
        // go back to idle state
        ke_state_set(prf_src_task_get(&(voice_env->prf_env), 0), VOICE_IDLE);
    }
}


uint8_t voice_update_ntf_ind_cfg(uint8_t conidx, uint8_t cfg, uint16_t valid_val, uint16_t value)
{
    struct voice_env_tag*  voice_env = PRF_ENV_GET(VOICE, voice);
    uint8_t status = GAP_ERR_NO_ERROR;

    if((value != valid_val) && (value != PRF_CLI_STOP_NTFIND))
    {
        status = PRF_APP_ERROR;

    }
    else if (value == valid_val)
    {
        voice_env->ntf_ind_cfg[conidx] |= cfg;
    }
    else
    {
        voice_env->ntf_ind_cfg[conidx] &= ~cfg;
    }

    if(status == GAP_ERR_NO_ERROR)
    {
		#if 1
		//不需要通知上层应用层
        // inform application that notification/indication configuration has changed
        struct voice_cfg_indntf_ind * ind = KE_MSG_ALLOC(VOICE_CFG_INDNTF_IND,
                prf_dst_task_get(&voice_env->prf_env, conidx), prf_src_task_get(&voice_env->prf_env, conidx),
                voice_cfg_indntf_ind);
        ind->conidx      = conidx;
        ind->ntf_ind_cfg = voice_env->ntf_ind_cfg[conidx];
        ke_msg_send(ind);
		#endif
    }

    return (status);
}

//----------------------辅助函数--------------------------------------------------

//根据本地INDEX 来得到句柄
uint16_t voice_att_hdl_get(struct voice_env_tag*  voice_env, uint8_t att_idx)
{
    uint16_t handle = voice_env->shdl;

    do
    {
			 if(att_idx >= VOICE_IDX_NB)  return ATT_INVALID_HDL;
				
			 handle += att_idx;
			}while(0);
		
		 // check if handle found
    return handle;
}


//根据句柄得到 本地INDEX
uint8_t  voice_att_idx_get(struct voice_env_tag*  voice_env, uint16_t handle)
{
    uint16_t handle_ref = voice_env->shdl;
    uint8_t att_idx = ATT_INVALID_IDX;

    do
    {
        // not valid hande
        if(handle < handle_ref)
        {
            break;
        }
				
				att_idx = (handle - handle_ref) + VOICE_IDX_SVC;
				
				if(att_idx >= VOICE_IDX_NB) return ATT_INVALID_IDX;
				
				return att_idx;		
    } while (0);

    return att_idx;
}






#endif

