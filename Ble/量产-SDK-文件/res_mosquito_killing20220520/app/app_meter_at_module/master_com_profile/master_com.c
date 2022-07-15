/************************************************************************
Copyright (C)  RadiaWave Technology(Shenzhen) Co., Ltd.
Project Name:      
File name:   
Description:

Author:   LuFeng    						Date: 2021-January-18
************************************************************************/

/**************************** Edit History **********************************
DATE           	NAME             	DESCRIPTION                    *
2021-01-18     	LuFeng            Create.         
************************************************************************/

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))

#include "rwip_config.h"
#include "master_com.h"
#include "gap.h"

#include "ke_mem.h"
#include "co_utils.h"
#include "rdw_mcu.h"

static uint8_t  device1_uart_client_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  void* params)
{
		uint8_t idx;
    //-------------------- allocate memory required for the profile  ---------------------

    struct device1_uart_client_env_tag* device1_uart_env =
            (struct device1_uart_client_env_tag* ) ke_malloc(sizeof(struct device1_uart_client_env_tag), KE_MEM_ATT_DB);
		
		memset(device1_uart_env, 0 , sizeof(struct device1_uart_client_env_tag));

    // allocate BLPC required environment variable
    env->env = (prf_env_t*) device1_uart_env;

    device1_uart_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    device1_uart_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // initialize environment variable
    env->id                     = TASK_ID_DEVICE1_COLLECTOR_UART;
    device1_connector_uart_task_init(&(env->desc));

    for(idx = 0; idx < UART_COUNT_IDX_MAX ; idx++)
    {
        device1_uart_env->env[idx] = NULL;
        // service is ready, go into an Idle state
        ke_state_set(KE_BUILD_ID(env->task, idx), DEVICE1_UART_FREE);
    }

    return GAP_ERR_NO_ERROR;
	
}

static void device1_uart_client_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct device1_uart_client_env_tag* device1_uart_env = (struct device1_uart_client_env_tag*) env->env;

    // clean-up environment variable allocated for task instance
    if(device1_uart_env->env[conidx] != NULL)
    {
			 // printf("device1_uart_client_cleanup conidx = %d \n", conidx);
        ke_free(device1_uart_env->env[conidx]);
        device1_uart_env->env[conidx] = NULL;
    }
    /* Put UART Client in Free state */
    ke_state_set(KE_BUILD_ID(env->task, conidx), DEVICE1_UART_FREE);
		
}


static void device1_uart_client_destroy(struct prf_task_env* env)
{
    uint8_t idx;
    struct device1_uart_client_env_tag* device1_uart_env = (struct device1_uart_client_env_tag*) env->env;

	//	printf("device1_uart_client_destroy \n");
	
    // cleanup environment variable for each task instances
    for(idx = 0; idx < UART_COUNT_IDX_MAX ; idx++)
    {
        device1_uart_client_cleanup(env, idx, 0);
    }

    // free profile environment variables
    env->env = NULL;
    ke_free(device1_uart_env);
}

	
static void device1_uart_client_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Put BLP Client in Idle state */
	//  printf("device1_uart_client_create  conidx = %d \n", conidx);
	  struct device1_uart_client_env_tag* device1_uart_env = (struct device1_uart_client_env_tag*) env->env;
    ke_state_set(KE_BUILD_ID(env->task, conidx), DEVICE1_UART_FREE);
}


const struct prf_task_cbs device1_uart_collector_itf =
{
        device1_uart_client_init,
        device1_uart_client_destroy,
        device1_uart_client_create,
        device1_uart_client_cleanup,
};



const struct prf_task_cbs* master_uart_collector_prf_itf_get(void)
{
   return &device1_uart_collector_itf;
}




void device1_uart_client_enable_rsp_send(struct device1_uart_client_env_tag *device_uart_env, uint8_t conidx, uint8_t status)
{
    // Send to APP the details of the discovered attributes on UART server
    struct device1_client_uart_enable_rsp * rsp = KE_MSG_ALLOC(
            DEVICE1_CLIENT_ENABLE_RSP,
            prf_dst_task_get(&(device_uart_env->prf_env), conidx),
            prf_src_task_get(&(device_uart_env->prf_env), conidx),
            device1_client_uart_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->bps = device_uart_env->env[conidx]->bps;

        prf_register_atthdl2gatt(&(device_uart_env->prf_env), conidx, &(device_uart_env->env[conidx]->bps.svc));

        // Go to connected state
        ke_state_set(prf_src_task_get(&(device_uart_env->prf_env), conidx), DEVICE1_UART_IDLE);
    }

    ke_msg_send(rsp);
}

void device1_uart_client_send_cmp_evt(struct device1_uart_client_env_tag *p_uart_env, uint8_t conidx, uint8_t operation, uint8_t status)
{
    struct uart_client_cmp_evt *p_evt;
    ke_task_id_t src_id = prf_src_task_get(&(p_uart_env->prf_env), conidx);
    ke_task_id_t dest_id = prf_dst_task_get(&(p_uart_env->prf_env), conidx);

    // Come back to the Connected state if the state was busy.
    if (ke_state_get(src_id) == DEVICE1_UART_BUSY)
    {
        ke_state_set(src_id, DEVICE1_UART_IDLE);
    }

    // Send the message to the application
    p_evt = KE_MSG_ALLOC(UART_CLIENT_CMP_EVT, dest_id, src_id, uart_client_cmp_evt);

    p_evt->operation   = operation;
    p_evt->status      = status;

    ke_msg_send(p_evt);
}







#endif

