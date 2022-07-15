/******************************************************************************
	File Name	:	voice_master.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/07

	Description :   连接声音模块的 host profile
	用于主控端 ,作为host 的profile ,用来连接外设, 相当
	于手机侧的 profile

	Changed Log	:
		Liujian  2020/03/07		- Creation

*****************************************************************************/

#include "voice_master.h"

#if BLE_APP_RECEIVE_VOICE

static uint8_t  voice_master_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  void* params)
{
		uint8_t idx;
    //-------------------- allocate memory required for the profile  ---------------------

    struct voice_master_env_tag* voice_master_env =
            (struct voice_master_env_tag* ) ke_malloc(sizeof(struct voice_master_env_tag), KE_MEM_ATT_DB);

    // allocate voice master required environment variable
    env->env = (prf_env_t*) voice_master_env;

    voice_master_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    voice_master_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // initialize environment variable
    env->id  = TASK_ID_VOICE_MASTER;
		
    voice_master_task_init(&(env->desc));

    for(idx = 0; idx < VOICE_COUNT_IDX_MAX ; idx++)
    {
        voice_master_env->env[idx] = NULL;
        // service is ready, go into an Idle state
        ke_state_set(KE_BUILD_ID(env->task, idx), VOICE_MASTER_FREE);
    }

    return GAP_ERR_NO_ERROR;
	
}

static void voice_master_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct voice_master_env_tag* voice_master_env = (struct voice_master_env_tag*) env->env;

    // clean-up environment variable allocated for task instance
    if(voice_master_env->env[conidx] != NULL)
    {
        ke_free(voice_master_env->env[conidx]);
        voice_master_env->env[conidx] = NULL;
    }

    ke_state_set(KE_BUILD_ID(env->task, conidx), VOICE_MASTER_FREE);
}


static void voice_master_destroy(struct prf_task_env* env)
{
    uint8_t idx;
    struct voice_master_env_tag* voice_master_env = (struct voice_master_env_tag*) env->env;

    // cleanup environment variable for each task instances
    for(idx = 0; idx < VOICE_COUNT_IDX_MAX ; idx++)
    {
        voice_master_cleanup(env, idx, 0);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(voice_master_env);
}

static void voice_master_create(struct prf_task_env* env, uint8_t conidx)
{
    ke_state_set(KE_BUILD_ID(env->task, conidx), VOICE_MASTER_FREE);
}


const struct prf_task_cbs voice_master_collector_itf =
{
        voice_master_init,
        voice_master_destroy,
        voice_master_create,
        voice_master_cleanup,
};



const struct prf_task_cbs* voice_master_prf_itf_get(void)
{
   return &voice_master_collector_itf;
}





void voice_master_enable_rsp_send(struct voice_master_env_tag *voice_master_env, uint8_t conidx, uint8_t status)
{
    // Send to APP the details of the discovered attributes on UART server
    struct voice_master_enable_rsp * rsp = KE_MSG_ALLOC(
            VOICE_MASTER_ENABLE_RSP,
            prf_dst_task_get(&(voice_master_env->prf_env), conidx),
            prf_src_task_get(&(voice_master_env->prf_env), conidx),
            voice_master_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->bps = voice_master_env->env[conidx]->bps;

        prf_register_atthdl2gatt(&(voice_master_env->prf_env), conidx, &(voice_master_env->env[conidx]->bps.svc));

        // Go to connected state
        ke_state_set(prf_src_task_get(&(voice_master_env->prf_env), conidx), VOICE_MASTER_IDLE);
    }

    ke_msg_send(rsp);
}





#endif
