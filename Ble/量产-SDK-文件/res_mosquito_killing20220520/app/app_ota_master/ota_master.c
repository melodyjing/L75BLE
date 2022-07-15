/******************************************************************************
	File Name	:	ota_master.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/8/26

	Description :   
	    用于发现 OTA 设备, 这个是 Master, 用来连接 OTA  设备

	Changed Log	:
		Liujian  2020/8/26		- Creation

*****************************************************************************/
#include "ota_master.h"

#if (APP_OTA_MATSER_ENABLE == 1)

static uint8_t  ota_master_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  void* params)
{
		uint8_t idx;
    //-------------------- allocate memory required for the profile  ---------------------

    struct ota_master_env_tag* ota_master_env =
            (struct ota_master_env_tag* ) ke_malloc(sizeof(struct ota_master_env_tag), KE_MEM_ATT_DB);

    // allocate voice master required environment variable
    env->env = (prf_env_t*) ota_master_env;

    ota_master_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    ota_master_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // initialize environment variable
    env->id  = TASK_ID_OTA_MASTER;
		
    ota_master_task_init(&(env->desc));

    for(idx = 0; idx < OTA_COUNT_IDX_MAX ; idx++)
    {
        ota_master_env->env[idx] = NULL;
        // service is ready, go into an Idle state
        ke_state_set(KE_BUILD_ID(env->task, idx), OTA_MASTER_FREE);
    }

    return GAP_ERR_NO_ERROR;
	
}

static void ota_master_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct ota_master_env_tag* ota_master_env = (struct ota_master_env_tag*) env->env;

    // clean-up environment variable allocated for task instance
    if(ota_master_env->env[conidx] != NULL)
    {
        ke_free(ota_master_env->env[conidx]);
        ota_master_env->env[conidx] = NULL;
    }

    ke_state_set(KE_BUILD_ID(env->task, conidx), OTA_MASTER_FREE);
}


static void ota_master_destroy(struct prf_task_env* env)
{
    uint8_t idx;
    struct ota_master_env_tag* ota_master_env = (struct ota_master_env_tag*) env->env;

    // cleanup environment variable for each task instances
    for(idx = 0; idx < OTA_COUNT_IDX_MAX ; idx++)
    {
        ota_master_cleanup(env, idx, 0);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(ota_master_env);
}

static void  ota_master_create(struct prf_task_env* env, uint8_t conidx)
{
    ke_state_set(KE_BUILD_ID(env->task, conidx), OTA_MASTER_FREE);
}


const struct prf_task_cbs ota_master_collector_itf =
{
        ota_master_init,
        ota_master_destroy,
        ota_master_create,
        ota_master_cleanup,
};



const struct prf_task_cbs* ota_master_prf_itf_get(void)
{
   return &ota_master_collector_itf;
}


void ota_master_enable_rsp_send(struct ota_master_env_tag *ota_master_env, uint8_t conidx, uint8_t status)
{
    // Send to APP the details of the discovered attributes on OTA server
    struct ota_master_enable_rsp * rsp = KE_MSG_ALLOC(
            OTA_MASTER_ENABLE_RSP,
            prf_dst_task_get(&(ota_master_env->prf_env), conidx),
            prf_src_task_get(&(ota_master_env->prf_env), conidx),
            ota_master_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->bps = ota_master_env->env[conidx]->bps;

        prf_register_atthdl2gatt(&(ota_master_env->prf_env), conidx, &(ota_master_env->env[conidx]->bps.svc));

        // Go to connected state
        ke_state_set(prf_src_task_get(&(ota_master_env->prf_env), conidx), OTA_MASTER_IDLE);
    }

    ke_msg_send(rsp);
}

void  ota_send_cmp_evt(struct ota_master_env_tag *ota_master_env, uint8_t conidx,
                        uint8_t operation, uint8_t status)
{
    // Go back to the CONNECTED state if the state is busy
    if (ke_state_get(prf_src_task_get(&(ota_master_env->prf_env), conidx)) == OTA_MASTER_BUSY)
    {
        ke_state_set(prf_src_task_get(&ota_master_env->prf_env, conidx), OTA_MASTER_IDLE);
    }

    // Send the message
    struct ota_cmp_evt *evt = KE_MSG_ALLOC(OTA_MASTER_CMP_EVT,
            prf_dst_task_get(&(ota_master_env->prf_env), conidx),
            prf_src_task_get(&(ota_master_env->prf_env), conidx),
            ota_cmp_evt);

    evt->operation  = operation;
    evt->status     = status;

    ke_msg_send(evt);
}



#endif

